#include "VoiceEngine.h"
#include "Config.h"
#include <DFRobotDFPlayerMini.h>
#include <cstring>

namespace VoiceEngine {

namespace {
HardwareSerial su03tSerial(1);
HardwareSerial dfPlayerSerial(2);
DFRobotDFPlayerMini dfPlayer;

// See docs/HARDWARE.md "SD card MP3 tracks" for the full 0001.mp3.. mapping.
void playNumber(uint16_t fileNumber) {
  dfPlayer.play(fileNumber);
  delay(200); // give the DFPlayer time to accept the command
}
} // namespace

void begin() {
  su03tSerial.begin(9600, SERIAL_8N1, PIN_SU03T_RX, PIN_SU03T_TX);
  dfPlayerSerial.begin(9600, SERIAL_8N1, PIN_DFPLAYER_RX, PIN_DFPLAYER_TX);

  if (!dfPlayer.begin(dfPlayerSerial)) {
    Serial.println("DFPlayer not responding — check wiring/SD card");
  } else {
    dfPlayer.volume(20);
    dfPlayer.EQ(DFPLAYER_EQ_NORMAL);
  }
}

String readCommand() {
  if (!su03tSerial.available()) return "";
  String cmd = su03tSerial.readStringUntil('\n');
  cmd.trim();
  return cmd;
}

void say(const char *phrase) {
  Serial.printf("[VOICE] %s\n", phrase);
  if (strcmp(phrase, "welcome") == 0) playNumber(1);
  else if (strcmp(phrase, "unknown_visitor") == 0) playNumber(2);
  else if (strcmp(phrase, "profile_saved") == 0) playNumber(3);
  else if (strcmp(phrase, "take_5_steps") == 0) playNumber(4);
  else if (strcmp(phrase, "color_changed") == 0) playNumber(5);
  else if (strcmp(phrase, "profile_deleted") == 0) playNumber(6);
  else if (strcmp(phrase, "leak_warning") == 0) playNumber(7);
  else if (strcmp(phrase, "no_room_for_profile") == 0) playNumber(9);
  else Serial.println("  (no DFPlayer track mapped for this phrase)");
}

void sayGoodbye(const char *name) {
  if (strcmp(name, "Anna") == 0) playNumber(10);
  else if (strcmp(name, "Ivan") == 0) playNumber(11);
  else if (strcmp(name, "Host") == 0) playNumber(12);
  else playNumber(13); // "Guest"
  delay(300);
  playNumber(8); // "... left the bathroom"
}

} // namespace VoiceEngine
