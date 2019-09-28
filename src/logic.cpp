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
    lightsensor(*this),
    wifi(*this)
{
}

void Logic::setup() {
  serial.setup("ExitMuseumBirdcage");

  audio.setup();
  lightsensor.setup();
  notes.setup();
  stepmotor.setup();
  wifi.setup();
}

void Logic::solved() {
  serial.print("Solved!\n");

  SOLVED = true;
  solved_at = millis();

  status();
}

void Logic::close() {
  serial.print("Resetting tray...\n");
  stepmotor.close();
  SOLVED_TRAY_IN = true;
  status();
}

void Logic::handle() {
  serial.handle();
  wifi.handle();
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
  if (lightsensor.lightDetected && !override_light_sensor) {
    if (audio.playing) {
      serial.print("Stopping song since its disabled\n");
      audio.stop();
    }
  } else {
    if (!audio.playing) {
      if (notes.waitedLongEnough()) {
        serial.print("playing song...\n");
        audio.play(audio.TRACK_FULL, true);
      }
    } else {

      // if totally timed out, restart it
      if (audio.finished()) {
        serial.print("done with song, starting again...\n");
        audio.stop();
      }

      // if there was a button press between our start time, then restart it
      if (notes.buttonPressedDuringSong(audio.playing_song_at)) {
        serial.print("restarting song from button press...\n");
        audio.stop();
      }
    }
  }

  int buttonPressed = notes.checkButtons();
  if (buttonPressed != 0) {
    audio.play(buttonPressed, true);

    // reset current pass when we reset the index
    if (notes.touch_current_pass_index == 0) {
      cur_password = String();
    }
    
    cur_password.concat(buttonPressed);

    int res = notes.checkPassword(buttonPressed, audio.track_lengths_ms[buttonPressed-1]);
    
    // only check the password if bird is in the dark, or we're overriding it
    if (!lightsensor.lightDetected || override_light_sensor) {
      if (res >= 0) {
        if (res == 1) {
          solved();
        } else {
          audio.play(audio.TRACK_FAILED, false);
        }
      }
    }

    status();
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

      "solved:%s,"
      "lightValue:%d,"
      "trayOpened:%s,"
      "password:%s"

      "%s"
    , GIT_HASH,
      GIT_DATE,
      DATE_NOW,

      solved_at > 0 ? "true" : "false",
      lightsensor.light_value,
      stepmotor.tray_out ? "true" : "false",
      cur_password.c_str(),

      CRLF);

  serial.print(cMsg);
}
