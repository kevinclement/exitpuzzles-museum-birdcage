#pragma once

#include "Arduino.h"

class Logic;

class WifiUpdate {
  public:
    WifiUpdate(Logic &logic);
    void setup();
    void handle();

  private:
    Logic &_logic;
};

