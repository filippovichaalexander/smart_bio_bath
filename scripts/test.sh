#!/bin/bash
# Run the native (host-side) unit tests inside Docker. Fast — no ESP32
# toolchain download, no board attached.
set -e
cd "$(dirname "${BASH_SOURCE[0]}")/../docker"
docker compose build
docker compose run --rm dev pio test -e native
