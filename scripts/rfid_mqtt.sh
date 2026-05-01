#!/bin/sh
# RFID to MQTT bridge script
# Runs the reader binary and publishes each tag read to the MQTT broker

BROKER_IP="${1:-192.168.1.55}"
READER_IP="192.168.1.18"
TOPIC="rfid/${READER_IP}/tags"
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
BINARY="${SCRIPT_DIR}/rfid_reader"
LIB_DIR="${SCRIPT_DIR}"

echo "Starting RFID MQTT bridge..."
echo "Broker: ${BROKER_IP}"
echo "Topic:  ${TOPIC}"

# Run the reader binary and publish each JSON line to MQTT
LD_LIBRARY_PATH="${LIB_DIR}" "${BINARY}" | while IFS= read -r line; do
    if [ -n "$line" ]; then
        LD_LIBRARY_PATH="/home/app0" /home/app0/mosquitto_pub \
            -h "${BROKER_IP}" \
            -p 1883 \
            -t "${TOPIC}" \
            -m "${line}"
    fi
done
