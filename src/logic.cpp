#include "Arduino.h"
#include "EEPROM.h"
#include "logic.h"
#include "consts.h"

int  FOO_VAR;                  // some foo desc
int  FOO_VAR_ADDR = 0;         // where to store foo in eeprom

Logic::Logic() 
  : serial(*this)
{
}

void Logic::setup() {
    serial.setup();

    readStoredVariables();

    serial.printHelp();
    printVariables();
}

void Logic::handle() {
    serial.handle();
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