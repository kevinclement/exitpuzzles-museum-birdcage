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
    bool override_light_detected = true; // for now, starts off with isLight true, so it doesnt play

    bool isLight();

  private:
    Logic &_logic;

    bool lightDetected = true;
};
