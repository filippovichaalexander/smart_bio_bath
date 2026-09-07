# Smart Bio-Bathroom

An offline, gait-recognizing bathroom lighting system for ESP32 — built
around a *Physarum polycephalum* slime-mold plasmodium as the adaptive
"brain," instead of any digital AI/ML model. No WiFi, no BLE, no cloud: the
whole thing runs standalone on the microcontroller plus the wetware.

- **Recognizes people by how they walk** — a piezo sensor + FFT extracts a
  7-parameter gait fingerprint per step, matched by cosine similarity
  against up to 50 saved profiles.
- **Adapts lighting per person and per person's biology** — each profile
  gets its own RGB color, corrected for time of day and season.
- **Learns via reinforcement, not training data** — a "No" button and
  correct-recognition events send punish/reward pulses to a living
  Physarum culture wired to the board; the plasmodium itself is the
  adaptive element (see [`docs/WETWARE.md`](docs/WETWARE.md)).
- **Offline voice I/O** — SU-03T for command recognition, DFPlayer Mini +
  pre-recorded MP3s for spoken responses.
- **Safety features** — a night mode (dim red, 00:00-05:00), and a water-
  leak detector that raises a buzzer after 10 minutes of continuous noise.

This project mixes electronics and water in a bathroom. Read
[`docs/HARDWARE.md`](docs/HARDWARE.md)'s safety section before wiring
anything.

## Repository layout

```
src/
  core/            Pure logic — gait matching, light-color calculation.
                    No Arduino/ESP32 dependency; unit-tested on the host.
  *.cpp/.h         Hardware modules (StepAnalyzer, ProfileManager,
                    LightController, VoiceEngine, BioAdapter,
                    SafetyMonitor) — thin wrappers around src/core plus
                    the actual sensor/actuator I/O.
  main.cpp         setup()/loop(), wiring the modules together.
include/Config.h   Pin map and tunable constants.
test/test_native/  Host-side Unity tests for src/core.
docker/            Dev/build environment (Dockerfile + docker-compose.yml).
scripts/           build.sh / test.sh / upload.sh / monitor.sh, all Docker-based.
docs/
  HARDWARE.md      Wiring, the SD/SU-03T pin-conflict fix, bring-up checklist.
  WETWARE.md       Growing and wiring the Physarum culture.
  TESTING.md       What to run at each stage, and what's expected.
  ORIGINAL_SPEC.md Raw design conversation this project was built from.
.github/workflows/ci.yml   Native tests + esp32dev build, on every push.
```

## Quick start

```bash
# Fast, no hardware: run the unit tests for the gait-matching/lighting logic
./scripts/test.sh

# Compile the full firmware (still no hardware needed)
./scripts/build.sh

# With an ESP32 connected via USB:
./scripts/upload.sh
./scripts/monitor.sh
```

All four scripts run inside Docker (`python:3.11-slim` + PlatformIO), so
you don't need PlatformIO installed on your host. See
[`docs/TESTING.md`](docs/TESTING.md) for what to expect at each stage, and
[`docs/HARDWARE.md`](docs/HARDWARE.md) / [`docs/WETWARE.md`](docs/WETWARE.md)
for what to wire up and how to verify it.

## Design notes

- **Why the code is split into `src/core` vs. hardware modules:** the gait
  cosine-matching and the day/season light-color math have no business
  touching `analogRead()` or `FastLED.show()`. Keeping them pure means they
  run — and are actually tested — on a laptop in milliseconds, not just on
  a flashed board.
- **A real bug, fixed:** the original design (`docs/ORIGINAL_SPEC.md`) put
  the SD card and the SU-03T voice module on the same two GPIOs (18/19).
  This build moves the SD card's SPI bus to GPIO4/14 instead — see
  `docs/HARDWARE.md` for the full explanation.
- **`findClosestProfile` picks the closest match, not the first one** under
  threshold — a small but real correctness fix over the original spec,
  which could otherwise misidentify someone when two profiles both fell
  within the match distance.
- **CI builds directly on the Actions runner, not through Docker** — the
  runner is already ephemeral, so Docker would add build time without
  adding reproducibility. Docker's job in this repo is making local builds
  reproducible across different developers' machines, which is a different
  problem.
- **No nginx / web dashboard.** This is embedded firmware with no server
  component — there's nothing for a reverse proxy to sit in front of. If a
  future companion dashboard (e.g. viewing profiles/logs pulled off the SD
  card) gets built, that's when nginx would earn a place here.
