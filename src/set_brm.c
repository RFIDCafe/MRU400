/*
 * set_brm.c (probe/diagnostic version)
 *
 * Tries multiple [0x8A] Read Configuration frame formats to find
 * which parameters the MRU400 accepts, then writes BRM config.
 *
 * Run over SSH on the reader:
 *   ssh app0@192.168.1.18
 *   cd /home/app0
 *   ./set_brm
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define READER_IP    "127.0.0.1"
#define READER_PORT  10001
#define CFG_BLOCK    19
#define BLOCK_SIZE   62
#define BRM_MODE     0x80

static uint16_t crc16(const uint8_t *data, int len)
{
    uint16_t crc = 0xFFFF;
    for (int i = 0; i < len; i++) {
        crc ^= data[i];
        for (int j = 0; j < 8; j++)
            crc = (crc & 1) ? (crc >> 1) ^ 0x8408 : (crc >> 1);
    }
    return crc;
}

static int build_frame(uint8_t *frame, const uint8_t *payload, int plen)
{
    int total = 1 + 2 + 1 + plen + 2;
    frame[0] = 0x02;
    frame[1] = (total >> 8) & 0xFF;
    frame[2] =  total & 0xFF;
    frame[3] = 0x00;
    memcpy(&frame[4], payload, plen);
    uint16_t crc = crc16(frame, 4 + plen);
    frame[4 + plen]     =  crc & 0xFF;
    frame[4 + plen + 1] = (crc >> 8) & 0xFF;
    return total;
}

static void hex_dump(const char *label, const uint8_t *buf, int len)
{
    printf("%s (%d bytes):", label, len);
    for (int i = 0; i < len; i++) printf(" %02X", buf[i]);
    printf("\n");
}

static int send_all(int fd, const uint8_t *buf, int len)
{
    int sent = 0;
    while (sent < len) {
        int n = (int)write(fd, buf + sent, (size_t)(len - sent));
        if (n <= 0) return -1;
        sent += n;
    }
    return 0;
}

static int recv_exact(int fd, uint8_t *buf, int len)
{
    int got = 0;
    while (got < len) {
        int n = (int)read(fd, buf + got, (size_t)(len - got));
        if (n <= 0) return -1;
        got += n;
    }
    return 0;
}

static int recv_frame(int fd, uint8_t *frame, int maxlen)
{
    if (recv_exact(fd, frame, 3) < 0) return -1;
    if (frame[0] != 0x02) { fprintf(stderr, "  Bad PFI: 0x%02X\n", frame[0]); return -1; }
    int total = (frame[1] << 8) | frame[2];
    if (total < 6 || total > maxlen) { fprintf(stderr, "  Bad frame length: %d\n", total); return -1; }
    if (recv_exact(fd, frame + 3, total - 3) < 0) return -1;
    return total;
}

static int try_read_cfg(int sock, const char *label,
                        const uint8_t *payload, int plen,
                        uint8_t *block_data_out)
{
    uint8_t frame[256];
    int frame_len = build_frame(frame, payload, plen);

    printf("\n--- %s ---\n", label);
    hex_dump("SEND", frame, frame_len);

    if (send_all(sock, frame, frame_len) < 0) { printf("  send failed\n"); return -1; }

    int rc = recv_frame(sock, frame, (int)sizeof(frame));
    if (rc < 0) { printf("  recv failed\n"); return -1; }

    hex_dump("RECV", frame, rc);
    if (rc < 6) { printf("  response too short\n"); return -1; }

    uint8_t status = frame[5];
    printf("  STATUS: 0x%02X %s\n", status,
           status == 0x00 ? "(OK)" :
           status == 0x11 ? "(Parameter Range Error)" :
           status == 0x15 ? "(Read Protect)" :
           status == 0x4D ? "(Login Required)" : "");

    if (status == 0x00 && block_data_out && rc >= 10 + BLOCK_SIZE) {
        memcpy(block_data_out, &frame[10], BLOCK_SIZE);
        printf("  OperatingMode.Mode (block_data[0]): 0x%02X\n", frame[10]);
    }
    return status;
}

static int write_brm(int sock, const uint8_t *read_payload, int rplen,
                     uint8_t *block_data)
{
    uint8_t frame[256];
    uint8_t payload[128];
    int pi = 0;

    block_data[0] = BRM_MODE;

    payload[pi++] = 0x8B;
    payload[pi++] = rplen > 1 ? read_payload[1] : 0x00; /* DEVICE */
    payload[pi++] = rplen > 2 ? read_payload[2] : 0x01; /* BANK */
    payload[pi++] = 0x09; /* MODE: START(bit3) + COMMIT(bit0) for single block */
    payload[pi++] = 0x01; /* BLOCK-COUNT */
    payload[pi++] = BLOCK_SIZE; /* BLOCK-SIZE = 62 */

    /* Copy block address bytes from working read payload */
    int addr_start = 4;
    int addr_end   = rplen - 1;
    for (int i = addr_start; i < addr_end; i++)
        payload[pi++] = read_payload[i];

    memcpy(&payload[pi], block_data, BLOCK_SIZE);
    pi += BLOCK_SIZE;

    int frame_len = build_frame(frame, payload, pi);
    printf("\n--- [0x8B] Write Configuration ---\n");
    hex_dump("SEND", frame, frame_len);

    if (send_all(sock, frame, frame_len) < 0) { printf("  send failed\n"); return -1; }

    int rc = recv_frame(sock, frame, (int)sizeof(frame));
    if (rc < 0) { printf("  recv failed\n"); return -1; }

    hex_dump("RECV", frame, rc);
    uint8_t status = frame[5];
    printf("  STATUS: 0x%02X %s\n", status,
           status == 0x00 ? "(OK - SUCCESS)" :
           status == 0x11 ? "(Parameter Range Error)" :
           status == 0x16 ? "(Write Protect)" : "");
    return status;
}

int main(void)
{
    uint8_t block_data[BLOCK_SIZE];
    uint8_t payload[16];
    uint8_t working_payload[16];
    int pi, status, working_plen = 0;

    printf("=== FEIG MRU400 - BRM Probe Tool ===\n");
    printf("Connecting to %s:%d ...\n", READER_IP, READER_PORT);

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) { perror("socket"); return 1; }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port   = htons(READER_PORT);
    inet_pton(AF_INET, READER_IP, &addr.sin_addr);

    if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("connect"); close(sock); return 1;
    }
    printf("Connected.\n");

    /* A: DEV=0x00 BANK=0x01 MODE=0x01 ADDR(2bytes)=19 COUNT=1 */
    pi = 0;
    payload[pi++] = 0x8A; payload[pi++] = 0x00; payload[pi++] = 0x01;
    payload[pi++] = 0x01; payload[pi++] = 0x00; payload[pi++] = CFG_BLOCK; payload[pi++] = 0x01;
    status = try_read_cfg(sock, "A: DEV=00 BANK=01 MODE=01 ADDR(2)=19 COUNT=1", payload, pi, block_data);
    if (status == 0) { memcpy(working_payload, payload, pi); working_plen = pi; goto write_it; }

    /* B: DEV=0x02 BANK=0x01 MODE=0x01 ADDR(2bytes)=19 COUNT=1 */
    pi = 0;
    payload[pi++] = 0x8A; payload[pi++] = 0x02; payload[pi++] = 0x01;
    payload[pi++] = 0x01; payload[pi++] = 0x00; payload[pi++] = CFG_BLOCK; payload[pi++] = 0x01;
    status = try_read_cfg(sock, "B: DEV=02 BANK=01 MODE=01 ADDR(2)=19 COUNT=1", payload, pi, block_data);
    if (status == 0) { memcpy(working_payload, payload, pi); working_plen = pi; goto write_it; }

    /* C: DEV=0x00 BANK=0x01 MODE=0x01 ADDR(1byte)=19 COUNT=1 */
    pi = 0;
    payload[pi++] = 0x8A; payload[pi++] = 0x00; payload[pi++] = 0x01;
    payload[pi++] = 0x01; payload[pi++] = CFG_BLOCK; payload[pi++] = 0x01;
    status = try_read_cfg(sock, "C: DEV=00 BANK=01 MODE=01 ADDR(1)=19 COUNT=1", payload, pi, block_data);
    if (status == 0) { memcpy(working_payload, payload, pi); working_plen = pi; goto write_it; }

    /* D: DEV=0x00 BANK=0x01 MODE=0x00 ADDR(2bytes)=19 COUNT=1 */
    pi = 0;
    payload[pi++] = 0x8A; payload[pi++] = 0x00; payload[pi++] = 0x01;
    payload[pi++] = 0x00; payload[pi++] = 0x00; payload[pi++] = CFG_BLOCK; payload[pi++] = 0x01;
    status = try_read_cfg(sock, "D: DEV=00 BANK=01 MODE=00 ADDR(2)=19 COUNT=1", payload, pi, block_data);
    if (status == 0) { memcpy(working_payload, payload, pi); working_plen = pi; goto write_it; }

    /* E: No DEV/BANK - just MODE=0x01 ADDR(2bytes)=19 COUNT=1 */
    pi = 0;
    payload[pi++] = 0x8A; payload[pi++] = 0x01;
    payload[pi++] = 0x00; payload[pi++] = CFG_BLOCK; payload[pi++] = 0x01;
    status = try_read_cfg(sock, "E: (no DEV/BANK) MODE=01 ADDR(2)=19 COUNT=1", payload, pi, block_data);
    if (status == 0) { memcpy(working_payload, payload, pi); working_plen = pi; goto write_it; }

    /* F: DEV=0x00 BANK=0x01 MODE=0x01 ADDR(2bytes)=0 COUNT=1 (probe block 0 to confirm format works) */
    pi = 0;
    payload[pi++] = 0x8A; payload[pi++] = 0x00; payload[pi++] = 0x01;
    payload[pi++] = 0x01; payload[pi++] = 0x00; payload[pi++] = 0x00; payload[pi++] = 0x01;
    status = try_read_cfg(sock, "F: DEV=00 BANK=01 MODE=01 ADDR(2)=0 COUNT=1 (probe block 0)", payload, pi, block_data);
    if (status == 0) { memcpy(working_payload, payload, pi); working_plen = pi; }

    printf("\n=== No working format found for block 19. Share output above for diagnosis. ===\n");
    close(sock);
    return 1;

write_it:
    printf("\n=== Working format found! ===\n");
    if (block_data[0] == BRM_MODE) {
        printf("Already in BRM mode (0x80). Run: ./rfid_mqtt.sh <laptop_ip>\n");
        close(sock);
        return 0;
    }
    printf("Current mode: 0x%02X. Writing 0x80 (BRM)...\n", block_data[0]);
    status = write_brm(sock, working_payload, working_plen, block_data);
    if (status == 0) {
        printf("\n*** SUCCESS - BRM mode set! Config saved to EEPROM. ***\n");
        printf("Run: ./rfid_mqtt.sh <laptop_ip>\n");
    } else {
        printf("\nWrite failed (0x%02X). Share full output above.\n", status);
    }
    close(sock);
    return (status == 0) ? 0 : 1;
}
