#include "Arduino.h"
#include "EEPROM.h"
#include "logic.h"
#include "consts.h"

int  FOO_VAR;                  // some foo desc
int  FOO_VAR_ADDR = 0;         // where to store foo in eeprom

Logic::Logic() 
  : serial(*this),
    audio(*this),
    stepmotor(*this),
    notes(*this),
    lightsensor(*this)
{
}

void Logic::setup() {
  serial.setup();
  audio.setup();
  lightsensor.setup();
  notes.setup();
  stepmotor.setup();

  readStoredVariables();

  serial.printHelp();
  printVariables();
}

void Logic::readStoredVariables() {
  EEPROM.begin(64); // don't need a big size
  //EEPROM.get(FOO_VAR_ADDR, FOO_VAR);
}

void Logic::printVariables() { 
  serial.print(CRLF);
  serial.print("Current Variables:%s", CRLF);
  serial.print("  foo:  %d%s", FOO_VAR, CRLF);
}

void Logic::open() {
  stepmotor.open();
}

void Logic::close() {
  stepmotor.close();
}

void Logic::handle() {
  serial.handle();
  lightsensor.handle();
  notes.handle();
  stepmotor.handle();
  audio.handle();

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
    //playTrack(buttonPressed, true);

    int res = notes.checkPassword(buttonPressed, audio.track_lengths_ms[buttonPressed-1]);
    if (res > 0) {
      if (res == 1) {
        //      SOLVED = true;
        //      solved_at = millis();
      } else {
        //playTrack(TRACK_FAILED, false);
        //reset();
      }
    }
  }

}
