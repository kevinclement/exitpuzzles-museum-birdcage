#pragma once

#include "Arduino.h"

class Logic;

class Stepper {
  public:
    Stepper(Logic &logic);
    void setup();
    void handle();

    void open();
    void close();

  private:
    Logic &_logic;
};
