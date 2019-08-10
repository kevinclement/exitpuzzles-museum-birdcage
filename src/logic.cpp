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
  if (lightsensor.lightDetected) {
    Serial.println("LIGHT ON");
  } else {
    Serial.println("LIGHT OFF");
  }
}
