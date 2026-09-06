# Smart Bio-Bathroom — PlatformIO dev/build environment.
# Kept deliberately small: build tooling + PlatformIO only, no IDE, no
# Fritzing/Jupyter/etc. `docker build` + `pio run` should be all this needs.
FROM python:3.11-slim

ENV DEBIAN_FRONTEND=noninteractive \
    PLATFORMIO_CORE_DIR=/root/.platformio

RUN apt-get update && apt-get install -y --no-install-recommends \
        build-essential \
        git \
        udev \
    && rm -rf /var/lib/apt/lists/*

RUN pip install --no-cache-dir platformio

# udev rules so the ESP32's USB-serial chip (CP210x / CH340) is writable by
# a non-root user on the host once passed through to the container.
RUN echo 'SUBSYSTEM=="tty", ATTRS{idVendor}=="10c4", ATTRS{idProduct}=="ea60", MODE="0666", GROUP="dialout"' > /etc/udev/rules.d/99-esp32.rules && \
    echo 'SUBSYSTEM=="tty", ATTRS{idVendor}=="1a86", ATTRS{idProduct}=="7523", MODE="0666", GROUP="dialout"' >> /etc/udev/rules.d/99-esp32.rules

WORKDIR /workspace
CMD ["/bin/bash"]
