#!/bin/bash
# Build the ESP32 firmware inside Docker. No board needs to be attached.
set -e
cd "$(dirname "${BASH_SOURCE[0]}")/../docker"
docker compose build
docker compose run --rm dev pio run -e esp32dev
