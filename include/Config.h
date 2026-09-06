#pragma once

// ==================== Pin map ====================
//
// NOTE — fixed a wiring conflict from the original spec: it put the SD
// card's SPI bus on the default VSPI pins (SCK=18, MISO=19), which are the
// exact same GPIOs it also assigned to the SU-03T voice module's UART
// (RX=18, TX=19). Both peripherals cannot share those pins. Since the SD
// card is driven through a software-selected SPIClass instance anyway (see
// main.cpp), it costs nothing to move it off the conflicting pins instead
// of touching the SU-03T wiring. See docs/HARDWARE.md for the full map.
constexpr int PIN_PIEZO       = 34;  // ADC in:  piezo gait sensor, via INA128/AD620 amplifier
constexpr int PIN_MIC         = 35;  // ADC in:  water-noise microphone
constexpr int PIN_RGB         = 26;  // digital out: WS2812 RGB strip data
constexpr int PIN_BTN_NO      = 25;  // digital in:  "No" button (negative reinforcement)
constexpr int PIN_BUZZER      = 27;  // digital out: leak-alarm buzzer
constexpr int PIN_BIO_STIM    = 32;  // digital out: Physarum electrode stimulation (through a series resistor — see docs/WETWARE.md)

constexpr int PIN_DFPLAYER_RX = 16;  // UART2 RX <- DFPlayer TX
constexpr int PIN_DFPLAYER_TX = 17;  // UART2 TX -> DFPlayer RX
constexpr int PIN_SU03T_RX    = 18;  // UART1 RX <- SU-03T TX
constexpr int PIN_SU03T_TX    = 19;  // UART1 TX -> SU-03T RX

constexpr int PIN_SD_CS       = 5;
constexpr int PIN_SD_MOSI     = 23;
constexpr int PIN_SD_MISO     = 4;   // moved from 19 — was colliding with SU-03T TX
constexpr int PIN_SD_SCK      = 14;  // moved from 18 — was colliding with SU-03T RX

constexpr int PIN_RTC_SDA     = 21;  // I2C (default)
constexpr int PIN_RTC_SCL     = 22;  // I2C (default)

// ==================== Constants ====================
constexpr int NUM_LEDS         = 60;
constexpr int MAX_PROFILES     = 50;
constexpr int STEPS_TO_LEARN   = 5;
constexpr int FORGET_DAYS      = 30;
constexpr int WATER_TIMEOUT_S  = 600;  // 10 minutes of continuous noise -> leak alarm
constexpr int NIGHT_START_HOUR = 0;
constexpr int NIGHT_END_HOUR   = 5;
constexpr int FFT_SAMPLES      = 256;
constexpr int SAMPLING_FREQ_HZ = 1000;
constexpr float MATCH_THRESHOLD = 0.3f; // cosine-distance match threshold (~70% similarity)
constexpr int STEP_DEBOUNCE_MS  = 200;
constexpr int PIEZO_TRIGGER_THRESHOLD = 100; // raw ADC counts
