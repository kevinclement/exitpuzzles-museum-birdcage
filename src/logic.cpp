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
  serial.setup("");

  audio.setup();
  lightsensor.setup();
  notes.setup();
  stepmotor.setup();
}

void Logic::solved() {
  serial.print("Solved!\n");

  SOLVED = true;
  solved_at = millis();
}

void Logic::close() {
  serial.print("Resetting tray...\n");
  stepmotor.close();
  SOLVED_TRAY_IN = true;
}

void Logic::handle() {
  serial.handle();
  lightsensor.handle();
  notes.handle();
  stepmotor.handle();
  audio.handle();

  if (_isLight != lightsensor.isLight()) {
    serial.print("Light change detected\n");
    _isLight = !_isLight;
    
    if (!SOLVED && !_isLight) {
      solved();
    } else {
      status();
    }
  }

  if (SOLVED) {
    if (!stepmotor.tray_out && !SOLVED_TRAY_IN)  {
      audio.play(5, true); 
      delay(3800);
      audio.play(3, true); 
      delay(2000);
      audio.play(1, true); 
      delay(1000);
      audio.play(8, false); 
      stepmotor.open();
    } else if (!SOLVED_TRAY_IN && millis() - solved_at > RESET_TIME) {
      close();
    }

    // NOOP the rest if we've solved it
    return;
  }

  // // check for light, only play sound when its dark
  // if (_isLight) {
  //   if (audio.playing) {
  //     serial.print("Stopping song since its disabled\n");
  //     audio.stop();
  //   }
  // } else {
  //   if (!audio.playing) {
  //     if (notes.waitedLongEnough()) {
  //       serial.print("playing song...\n");
  //       audio.play(audio.TRACK_FULL, true);
  //     }
  //   } else {

  //     // if totally timed out, restart it
  //     if (audio.finished()) {
  //       serial.print("done with song, starting again...\n");
  //       audio.stop();
  //     }

  //     // if there was a button press between our start time, then restart it
  //     if (notes.buttonPressedDuringSong(audio.playing_song_at)) {
  //       serial.print("restarting song from button press...\n");
  //       audio.stop();
  //     }
  //   }
  // }

  // int buttonPressed = notes.checkButtons();
  // if (buttonPressed != 0) {
  //   audio.play(buttonPressed, true);

  //   // reset current pass when we reset the index
  //   if (notes.touch_current_pass_index == 0) {
  //     cur_password = String();
  //   }
    
  //   cur_password.concat(buttonPressed);

  //   int res = notes.checkPassword(buttonPressed, audio.track_lengths_ms[buttonPressed-1]);
    
  //   // only check the password if bird is in the dark
  //   if (!_isLight) {
  //     if (res >= 0) {
  //       if (res == 1) {
  //         solved();
  //       } else {
  //         audio.play(audio.TRACK_FAILED, false);
  //       }
  //     }
  //   }

  //   status();
  // }

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
      "isLight:%s,"
      "darkDetection:%s,"
      "trayOpened:%s,"
      "password:%s"

      "%s"
    , GIT_HASH,
      GIT_DATE,
      DATE_NOW,

      solved_at > 0 ? "true" : "false",
      lightsensor.light_value,
      _isLight ? "true" : "false",
      lightsensor.darkDetectionEnabled ? "true" : "false",
      stepmotor.tray_out ? "true" : "false",
      cur_password.c_str(),

      CRLF);

  serial.print(cMsg);
}
