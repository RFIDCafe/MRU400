/*
 * set_brm.c
 *
 * One-time configuration tool for FEIG MRU400 RFID reader.
 * Sets OperatingMode.Mode = 0x80 (Buffered Read Mode).
 *
 * Run this ONCE over SSH on the reader itself:
 *   ssh app0@192.168.1.18
 *   cd /home/app0
 *   ./set_brm
 *
 * Config is written to EEPROM and survives reboots.
 * No FEIG SDK required - uses raw FEIG Advanced Protocol over TCP.
 * Connects to the mru400_daemon already running on 127.0.0.1:10001.
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

/* CFG block 19, byte 0 = OperatingMode.Mode */
#define CFG_BLOCK    19
#define BLOCK_SIZE   62    /* All config blocks are 62 bytes */
#define HOST_MODE    0x00  /* Default - reader only reads on explicit command */
#define BRM_MODE     0x80  /* Buffered Read Mode - reader scans autonomously */

/* -------------------------------------------------------------------------
 * CRC16
 * Polynomial: x16 + x12 + x5 + 1 = 0x8408 (reflected)
 * Preset: 0xFFFF
 * Result placed in frame LSB first
 * ------------------------------------------------------------------------- */
static uint16_t crc16(const uint8_t *data, int len)
{
    uint16_t crc = 0xFFFF;
    for (int i = 0; i < len; i++) {
        crc ^= data[i];
        for (int j = 0; j < 8; j++) {
            if (crc & 0x0001)
                crc = (crc >> 1) ^ 0x8408;
            else
                crc >>= 1;
        }
    }
    return crc;
}

/* -------------------------------------------------------------------------
 * Build an Advanced Protocol Frame (PFI = 0x02):
 *   PFI(1) | LEN(2) | ADR(1) | payload(n) | CRC16(2)
 *
 * payload must include the COMMAND byte as its first byte.
 * Returns total frame length in bytes.
 * ------------------------------------------------------------------------- */
static int build_frame(uint8_t *frame, const uint8_t *payload, int payload_len)
{
    int total = 1 + 2 + 1 + payload_len + 2;   /* PFI+LEN+ADR+payload+CRC */
    frame[0] = 0x02;                             /* PFI: Advanced Protocol Frame */
    frame[1] = (total >> 8) & 0xFF;             /* LEN MSB */
    frame[2] =  total & 0xFF;                   /* LEN LSB */
    frame[3] = 0x00;                             /* COM-ADR: 0 for TCP */
    memcpy(&frame[4], payload, payload_len);
    uint16_t crc = crc16(frame, 4 + payload_len);
    frame[4 + payload_len]     =  crc & 0xFF;   /* CRC LSB first */
    frame[4 + payload_len + 1] = (crc >> 8) & 0xFF;
    return total;
}

static int send_all(int fd, const uint8_t *buf, int len)
{
    int sent = 0;
    while (sent < len) {
        int n = (int)write(fd, buf + sent, (size_t)(len - sent));
        if (n <= 0) { perror("write"); return -1; }
        sent += n;
    }
    return 0;
}

static int recv_exact(int fd, uint8_t *buf, int len)
{
    int got = 0;
    while (got < len) {
        int n = (int)read(fd, buf + got, (size_t)(len - got));
        if (n <= 0) { perror("read"); return -1; }
        got += n;
    }
    return 0;
}

/* Read a complete Advanced Protocol Frame.
 * Reads the 3-byte header first to get total length, then the rest.
 * Returns total frame length, or -1 on error. */
static int recv_frame(int fd, uint8_t *frame, int maxlen)
{
    if (recv_exact(fd, frame, 3) < 0) return -1;
    if (frame[0] != 0x02) {
        fprintf(stderr, "ERROR: Unexpected frame identifier: 0x%02X (expected 0x02)\n",
                frame[0]);
        return -1;
    }
    int total = (frame[1] << 8) | frame[2];
    if (total < 8 || total > maxlen) {
        fprintf(stderr, "ERROR: Invalid frame length: %d\n", total);
        return -1;
    }
    if (recv_exact(fd, frame + 3, total - 3) < 0) return -1;
    return total;
}

int main(void)
{
    uint8_t frame[256];
    uint8_t payload[128];
    uint8_t block_data[BLOCK_SIZE];
    int rc;

    printf("=== FEIG MRU400 - Set Buffered Read Mode ===\n\n");
    printf("Connecting to %s:%d ...\n", READER_IP, READER_PORT);

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) { perror("socket"); return 1; }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port   = htons(READER_PORT);
    inet_pton(AF_INET, READER_IP, &addr.sin_addr);

    if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("connect");
        fprintf(stderr, "Is mru400_daemon running? Try: ps aux | grep mru400\n");
        close(sock);
        return 1;
    }
    printf("Connected.\n\n");

    /* ------------------------------------------------------------------
     * Step 1: [0x8A] Read Configuration - get current block 19
     *
     * REQUEST payload (after ADR):
     *   CMD(0x8A) DEVICE(0x00) BANK(0x01) MODE(0x01)
     *   BLOCK-ADDR-HI(0x00) BLOCK-ADDR-LO(0x13) BLOCK-COUNT(0x01)
     * ------------------------------------------------------------------ */
    int pi = 0;
    payload[pi++] = 0x8A;       /* Command: Read Configuration */
    payload[pi++] = 0x00;       /* Device: always 0x00 */
    payload[pi++] = 0x01;       /* Bank: MAIN = 0x01 */
    payload[pi++] = 0x01;       /* Mode: always 0x01 */
    payload[pi++] = 0x00;       /* Block address MSB */
    payload[pi++] = CFG_BLOCK;  /* Block address LSB = 19 = 0x13 */
    payload[pi++] = 0x01;       /* Block count: read 1 block */

    int frame_len = build_frame(frame, payload, pi);

    printf("[0x8A] Reading CFG block %d (OperatingMode) ...\n", CFG_BLOCK);
    if (send_all(sock, frame, frame_len) < 0) { close(sock); return 1; }

    rc = recv_frame(sock, frame, (int)sizeof(frame));
    if (rc < 0) { close(sock); return 1; }

    /*
     * RESPONSE frame layout:
     *   [0]  PFI
     *   [1,2] LEN
     *   [3]  ADR
     *   [4]  CMD  (0x8A)
     *   [5]  STATUS
     *   [6]  BLOCK-COUNT
     *   [7]  BLOCK-SIZE (62)
     *   [8,9] BLOCK-ADR
     *   [10..71] BLOCK-DATA (62 bytes)
     *   [72,73] CRC
     */
    uint8_t status = frame[5];
    if (status != 0x00) {
        fprintf(stderr, "ERROR: [0x8A] Read Configuration failed. Status: 0x%02X\n", status);
        close(sock);
        return 1;
    }

    uint8_t blk_count = frame[6];
    uint8_t blk_size  = frame[7];
    if (blk_count != 1 || blk_size != BLOCK_SIZE) {
        fprintf(stderr, "ERROR: Unexpected response: block_count=%d block_size=%d\n",
                blk_count, blk_size);
        close(sock);
        return 1;
    }

    memcpy(block_data, &frame[10], BLOCK_SIZE);

    uint8_t current_mode = block_data[0];
    printf("Current OperatingMode.Mode: 0x%02X  (%s)\n\n",
           current_mode,
           current_mode == BRM_MODE  ? "Buffered Read Mode" :
           current_mode == HOST_MODE ? "Host Mode" : "Unknown mode");

    if (current_mode == BRM_MODE) {
        printf("Already in Buffered Read Mode. Nothing to change.\n");
        printf("You can run: ./rfid_mqtt.sh <laptop_ip>\n");
        close(sock);
        return 0;
    }

    /* ------------------------------------------------------------------
     * Step 2: [0x8B] Write Configuration - set byte 0 to 0x80 (BRM)
     *
     * REQUEST payload (after ADR):
     *   CMD(0x8B) DEVICE(0x00) BANK(0x01) MODE(0x01) BLOCK-COUNT(0x01)
     *   BLOCK-SIZE(0x3E) BLOCK-ADDR-HI(0x00) BLOCK-ADDR-LO(0x13)
     *   BLOCK-DATA (62 bytes, byte 0 changed to 0x80)
     * ------------------------------------------------------------------ */
    block_data[0] = BRM_MODE;  /* OperatingMode.Mode = 0x80 */

    pi = 0;
    payload[pi++] = 0x8B;       /* Command: Write Configuration */
    payload[pi++] = 0x00;       /* Device: always 0x00 */
    payload[pi++] = 0x01;       /* Bank: MAIN = 0x01 */
    payload[pi++] = 0x01;       /* Mode: 0x01 */
    payload[pi++] = 0x01;       /* Block count: write 1 block */
    payload[pi++] = BLOCK_SIZE; /* Block size: 62 = 0x3E */
    payload[pi++] = 0x00;       /* Block address MSB */
    payload[pi++] = CFG_BLOCK;  /* Block address LSB = 19 = 0x13 */
    memcpy(&payload[pi], block_data, BLOCK_SIZE);
    pi += BLOCK_SIZE;

    frame_len = build_frame(frame, payload, pi);

    printf("[0x8B] Writing CFG block %d with OperatingMode.Mode = 0x80 ...\n", CFG_BLOCK);
    if (send_all(sock, frame, frame_len) < 0) { close(sock); return 1; }

    rc = recv_frame(sock, frame, (int)sizeof(frame));
    if (rc < 0) { close(sock); return 1; }

    /*
     * RESPONSE frame layout:
     *   [0]  PFI
     *   [1,2] LEN
     *   [3]  ADR
     *   [4]  CMD  (0x8B)
     *   [5]  STATUS
     *   [6,7] CRC
     */
    uint8_t write_status = frame[5];
    if (write_status != 0x00) {
        fprintf(stderr, "ERROR: [0x8B] Write Configuration failed. Status: 0x%02X\n",
                write_status);
        if (write_status == 0x11)
            fprintf(stderr, "       0x11 = Parameter range error\n");
        else if (write_status == 0x14)
            fprintf(stderr, "       0x14 = Write protected\n");
        else if (write_status == 0x16)
            fprintf(stderr, "       0x16 = Parameter mismatch\n");
        close(sock);
        return 1;
    }

    printf("\n*** SUCCESS ***\n");
    printf("OperatingMode.Mode set to 0x80 (Buffered Read Mode).\n");
    printf("Config saved to EEPROM - survives reboots.\n\n");
    printf("Next step:\n");
    printf("  ./rfid_mqtt.sh <laptop_ip>\n");
    printf("  e.g. ./rfid_mqtt.sh 192.168.1.55\n");

    close(sock);
    return 0;
}
