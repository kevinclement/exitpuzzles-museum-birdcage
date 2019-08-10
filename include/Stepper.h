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
    
    bool tray_out = false;
  private:
    Logic &_logic;
    
};
