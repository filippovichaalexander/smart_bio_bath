#!/bin/bash
# Open the serial monitor for a connected ESP32 (115200 baud).
set -e
PORT="${PORT:-/dev/ttyUSB0}"
cd "$(dirname "${BASH_SOURCE[0]}")/../docker"

if [ ! -e "$PORT" ]; then
  echo "No device at $PORT — plug in the ESP32, or set PORT=/dev/ttyACM0 etc." >&2
  exit 1
fi

docker compose build
docker compose run --rm --device "$PORT":/dev/ttyUSB0 dev \
  pio device monitor --baud 115200 --port /dev/ttyUSB0
