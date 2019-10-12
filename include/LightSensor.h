#pragma once

#include "Arduino.h"

class Logic;

class LightSensor {
  public:
    LightSensor(Logic &logic);
    void setup();
    void handle();

    bool lightDetected = true;
    int light_value = 0;
    bool debug = false;

  private:
    Logic &_logic;
};
