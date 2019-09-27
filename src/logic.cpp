#include "Arduino.h"
#include "EEPROM.h"
#include "logic.h"
#include "consts.h"

#define RESET_TIME 180000      // 3 minutes

bool SOLVED = false;
bool SOLVED_TRAY_IN = false;
unsigned long solved_at = 0;

Logic::Logic() 
  : serial(),
    audio(*this),
    stepmotor(*this),
    notes(*this),
    lightsensor(*this)
{
}

void Logic::setup() {
  serial.setup("ExitMuseumBirdcage");

  audio.setup();
  lightsensor.setup();
  notes.setup();
  stepmotor.setup();
}

void Logic::solved() {
  serial.print("Solved!\n");

  SOLVED = true;
  solved_at = millis();

  status();
}

void Logic::close() {
  Serial.printf("Resetting tray...\n");
  stepmotor.close();
  SOLVED_TRAY_IN = true;
}

void Logic::handle() {
  serial.handle();
  lightsensor.handle();
  notes.handle();
  stepmotor.handle();
  audio.handle();

  if (SOLVED) {
    if (!stepmotor.tray_out && !SOLVED_TRAY_IN)  {
      audio.play(audio.TRACK_WINNING, false);
      stepmotor.open();
    } else if (!SOLVED_TRAY_IN && millis() - solved_at > RESET_TIME) {
      close();
    }

    // NOOP the rest if we've solved it
    return;
  }

  // check for light, only enable the device when its dark
  // if (lightsensor.lightDetected) {
  //   if (audio.playing) {
  //     Serial.printf("Stopping song since its disabled\n");
  //     audio.stop();
  //   }
  //   return;
  // }

  if (!audio.playing) {
    if (notes.waitedLongEnough()) {
      Serial.printf("playing song...\n");
      audio.play(audio.TRACK_FULL, true);
    }
  } else {

    // if totally timed out, restart it
    if (audio.finished()) {
      Serial.printf("done with song, starting again...\n");
      audio.stop();
    }

    // if there was a button press between our start time, then restart it
    if (notes.buttonPressedDuringSong(audio.playing_song_at)) {
      Serial.printf("restarting song from button press...\n");
      audio.stop();
    }
  }

  int buttonPressed = notes.checkButtons();
  if (buttonPressed != 0) {
    audio.play(buttonPressed, true);

    int res = notes.checkPassword(buttonPressed, audio.track_lengths_ms[buttonPressed-1]);
    if (res >= 0) {
      if (res == 1) {
        solved();
      } else {
        audio.play(audio.TRACK_FAILED, false);
      }
    }
  }

  delay(100);
}

void Logic::status() {
  char cMsg[254];
  sprintf(cMsg, 
    "status="
      "version:%s,"
      "gitDate:%s,"
      "buildDate:%s,"
      "solved:%s"
      "%s"
    , GIT_HASH,
      GIT_DATE,
      DATE_NOW,
      solved_at > 0 ? "true" : "false",
      CRLF);

  serial.print(cMsg);
}
