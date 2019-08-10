#pragma once

#include "SerialManager.h"

class Logic {
public:
  Logic();
  SerialManager serial;

  void setup();
  void handle();

private:
  void readStoredVariables();
  void printVariables();
};

