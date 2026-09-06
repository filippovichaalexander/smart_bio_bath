# Hardware — wiring guide

This device mixes mains-adjacent electronics with **water**, in a
**bathroom**. Read the safety section before connecting anything.

## Safety first (non-negotiable)

- **Power the whole system from a low-voltage DC source only** (USB/5V
  adapter or battery, ≤5V). Never wire anything in this project to mains
  (110/230V) yourself.
- If your bathroom's power outlets are on a **GFCI/RCD** circuit, keep it
  that way — don't bypass it. If you're not sure whether your bathroom
  circuit is GFCI-protected, have an electrician check before installing
  any electronics near the sink/shower.
- **Enclose everything.** The ESP32, amplifier board, DFPlayer, SU-03T and
  their wiring must sit inside a splash-proof enclosure, mounted away from
  direct water spray. Only the RGB strip, piezo sensor, and (if you route it
  outside the enclosure) the wetware culture should be exposed.
- The Physarum culture container (docs/WETWARE.md) should also be covered,
  both to protect the culture and to keep condensation off the electrode
  wiring.
- This is a hobbyist DIY build, not a certified appliance. If in doubt about
  any mains-adjacent wiring, stop and consult a licensed electrician —
  don't guess in a room with running water.

## Pin map

| Signal | ESP32 pin | Notes |
|---|---|---|
| Piezo gait sensor (signal) | GPIO34 (ADC) | Through the INA128/AD620 amplifier — never straight into the ADC |
| Amplifier power | 3.3V + GND | |
| WS2812 RGB strip (DI) | GPIO26 | Add a ~470Ω resistor in series on the data line |
| "No" button | GPIO25 | `INPUT_PULLUP`, button to GND |
| Leak buzzer | GPIO27 | |
| Physarum stimulation electrode | GPIO32 | **Through a series resistor — see docs/WETWARE.md** |
| DFPlayer RX | GPIO16 | ESP32 UART2 RX ← DFPlayer TX |
| DFPlayer TX | GPIO17 | ESP32 UART2 TX → DFPlayer RX |
| DFPlayer VCC/GND | 5V / GND | |
| SU-03T RX | GPIO18 | ESP32 UART1 RX ← SU-03T TX |
| SU-03T TX | GPIO19 | ESP32 UART1 TX → SU-03T RX |
| SU-03T VCC/GND | 5V / GND | |
| Water-noise microphone | GPIO35 (ADC) | |
| SD card CS | GPIO5 | |
| SD card MOSI | GPIO23 | |
| SD card MISO | **GPIO4** | ⚠️ see note below |
| SD card SCK | **GPIO14** | ⚠️ see note below |
| RTC DS3231 SDA | GPIO21 | I2C |
| RTC DS3231 SCL | GPIO22 | I2C |

### Pin-conflict fix (read this if you're wiring from the original chat log)

The original design conversation (`docs/ORIGINAL_SPEC.md`) put the SD card
on ESP32's default VSPI pins — **SCK=GPIO18, MISO=GPIO19** — which are the
*exact same pins* it also assigned to the SU-03T's UART (RX=18, TX=19). Two
peripherals can't share those GPIOs; wired as originally written, either the
SD card or the voice module (or both) would fail intermittently.

This firmware fixes it by driving the SD card off a second,
software-selected SPI bus on **GPIO14 (SCK) / GPIO4 (MISO)** instead (see
`sdSPI.begin(...)` in `src/main.cpp`) — SU-03T keeps GPIO18/19 unchanged.
Wire your SD breakout's SCK and MISO to 14 and 4, not 18 and 19.

## Bio-signal amplifier (piezo → ESP32)

The piezo signal is millivolt-scale; the ESP32 ADC needs volts. You need an
instrumentation amplifier between them — **do not connect the piezo
directly to GPIO34**.

```
[Piezo electrode 1] --> AD620/INA128 +IN
[Piezo electrode 2] --> AD620/INA128 -IN
AD620/INA128 Vout --> RC low-pass filter (~50Hz cutoff) --> ESP32 GPIO34
```

- Gain resistor (`R_G`) for ~500-1000x gain: roughly 50-100Ω (check your
  amplifier's datasheet gain formula).
- The RC low-pass filter rejects 50/60Hz mains hum, which is the dominant
  noise source in a bathroom full of wiring.

## SD card — MP3 tracks for DFPlayer

Format the microSD as FAT32, create an `MP3/` folder, and add these files
(DFPlayer expects zero-padded 4-digit names):

| File | Content | Used by (`VoiceEngine::say`) |
|---|---|---|
| `0001.mp3` | "Welcome to the smart bathroom" | `welcome` |
| `0002.mp3` | "Warning: unknown visitor" | `unknown_visitor` |
| `0003.mp3` | "Profile saved" | `profile_saved` |
| `0004.mp3` | "Take 5 steps to train" | `take_5_steps` |
| `0005.mp3` | "Color changed" | `color_changed` |
| `0006.mp3` | "Profile deleted" | `profile_deleted` |
| `0007.mp3` | "Warning: possible water leak" | `leak_warning` |
| `0008.mp3` | "... left the bathroom" (suffix) | `VoiceEngine::sayGoodbye` |
| `0009.mp3` | "No room for a new profile" | `no_room_for_profile` |
| `0010.mp3`-`0013.mp3` | Individual names + "Guest" | `VoiceEngine::sayGoodbye` |

Record these in whatever language you like — the firmware only ever plays
them by track number, it never synthesizes speech.

## Bring-up checklist

Do this before anything touches the Physarum culture — verify the ESP32
side works standalone first:

1. Flash the firmware (`./scripts/upload.sh`) with nothing but the RGB
   strip, button and buzzer wired. Confirm boot logs over serial
   (`./scripts/monitor.sh`) and that the strip lights up in the default
   "Host" profile color.
2. Wire the RTC; confirm the serial log stops printing "RTC not found!".
3. Wire the SD card on the corrected pins (4/14, see above); confirm no
   "SD card not found!" on boot, and that DFPlayer plays `0001.mp3` at
   startup.
4. Wire the piezo sensor **through the amplifier** and tap it — confirm the
   serial monitor prints a `Step: dur=... amp=...` line per tap (see
   `docs/TESTING.md` for expected ranges).
5. Only after 1-4 all check out, connect the wetware electrodes
   (docs/WETWARE.md).
