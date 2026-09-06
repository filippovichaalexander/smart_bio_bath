#pragma once
#include <Arduino.h>

// Offline voice I/O: SU-03T for command recognition (UART1), DFPlayer Mini
// for spoken responses (UART2 + SD card MP3s). No network, no cloud speech
// API — see docs/HARDWARE.md for the MP3 track numbering this maps to.
namespace VoiceEngine {

void begin();

// Non-blocking. Returns the next full line received from the SU-03T module
// (trimmed), or an empty string if nothing is waiting.
String readCommand();

// Plays the DFPlayer track mapped to a known phrase; logs to Serial either way.
void say(const char *phrase);

// Plays "<name>" followed by the "left the bathroom" track.
void sayGoodbye(const char *name);

} // namespace VoiceEngine
