#!/bin/bash
# Flash the firmware onto an ESP32 connected at /dev/ttyUSB0 (override with
# $PORT). Needs the board attached, unlike build.sh/test.sh.
set -e
PORT="${PORT:-/dev/ttyUSB0}"
cd "$(dirname "${BASH_SOURCE[0]}")/../docker"

if [ ! -e "$PORT" ]; then
  echo "No device at $PORT — plug in the ESP32, or set PORT=/dev/ttyACM0 etc." >&2
  exit 1
fi

docker compose build
docker compose run --rm --device "$PORT":/dev/ttyUSB0 dev \
  pio run -e esp32dev --target upload --upload-port /dev/ttyUSB0
