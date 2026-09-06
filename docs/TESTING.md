# Testing

Three layers, from fastest/no-hardware to slowest/full-hardware.

## 1. Native unit tests (no ESP32, no Docker even required if you have a
   local `pio`)

Covers the pure logic in `src/core`: gait-vector cosine matching and the
night/season light-color calculation.

```bash
./scripts/test.sh
# or, without Docker, if you have PlatformIO installed locally:
pio test -e native
```

Expect all 8 cases in `test/test_native/test_core_logic.cpp` to pass in a
few seconds. This is what CI runs on every push (`.github/workflows/ci.yml`,
`native-tests` job) and what you should run after touching anything in
`src/core`.

## 2. Firmware build (ESP32 toolchain, still no board required)

```bash
./scripts/build.sh
# or: pio run -e esp32dev
```

This compiles the full firmware and produces `firmware.bin`, but doesn't
flash anything — useful for catching wiring-of-pins/library-API mistakes
without a board attached. CI runs this too (`firmware-build` job).

## 3. Hardware bring-up (needs the physical board)

Follow `docs/HARDWARE.md`'s bring-up checklist in order — light/button/
buzzer first, then RTC, then SD+DFPlayer, then the piezo sensor, and only
then the wetware electrodes (`docs/WETWARE.md`). Use the serial monitor
throughout:

```bash
./scripts/upload.sh
./scripts/monitor.sh
```

### What to look for at each stage

| Stage | Expected serial output |
|---|---|
| Boot, no peripherals wired | `=== Smart Bio-Bathroom ===` ... `System ready.` (RTC/SD "not found" warnings are expected until wired) |
| RTC wired | No more `RTC not found!` line |
| SD + DFPlayer wired (pins 4/14, not 18/19 — see HARDWARE.md) | No more `SD card not found!`; welcome track plays |
| Piezo + amplifier wired, tap the sensor | One `Step: dur=... amp=... slope=... vib=... interval=... freq=... snr=...` line per tap |
| Someone steps on it repeatedly | `Recognized: Host` (the default profile) if their gait is within `MATCH_THRESHOLD`, otherwise `Unknown visitor` |
| "No" button pressed | `BioAdapter: punish pulse ("No" button pressed)` |
| Mic hears running water for >10 min | `Warning: possible water leak` spoken, buzzer on; stops within one loop iteration of the noise stopping |

### Training a new profile

Say (or send over the SU-03T's serial link, if you're bench-testing without
the voice module) the trigger phrase `Запомни меня как <Имя>`. The firmware
will prompt for 5 steps, average their gait parameters, and save a new
profile — watch for `Профиль сохранен` / the `profile_saved` track.

### Wetware-specific tests

See `docs/WETWARE.md`'s "Testing the wetware connection" section — those
checks (continuity, pulse response, multi-day behavioral shift) are
separate from the ESP32-side tests above because the plasmodium is a
living system on its own timescale, not something a unit test can cover.

## What isn't covered

There's no hardware-in-the-loop CI (GitHub Actions obviously can't wire a
piezo sensor or grow a slime mold) — the `firmware-build` CI job only
proves the code compiles for the target, not that recognition accuracy or
the bio-stimulation circuit behaves as expected. That validation is the
bring-up checklist above, done by hand once the board exists.
