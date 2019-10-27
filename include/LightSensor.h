#pragma once

#include "Arduino.h"

class Logic;

class LightSensor {
  public:
    LightSensor(Logic &logic);
    void setup();
    void handle();

    int light_value = 0;
    bool debug = false;
    bool darkDetectionEnabled = true; // starts out with dark detection logic enabled

    bool isLight();

  private:
    Logic &_logic;

    bool lightDetected = true;
};
