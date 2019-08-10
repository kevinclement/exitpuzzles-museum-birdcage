#include <Arduino.h>
#include "logic.h"

// Globals
Logic logic;

void setup() {
  logic.setup();
}

void loop() {
  logic.handle();
}