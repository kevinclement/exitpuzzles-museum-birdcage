#pragma once

#include "Arduino.h"

class Logic;

class Notes {
  public:
    Notes(Logic &logic);
    void setup();
    int handle();
    
  private:
    Logic &_logic;
};
