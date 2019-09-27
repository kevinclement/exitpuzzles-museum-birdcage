#pragma once

#include "Arduino.h"

class Logic;

class Notes {
  public:
    Notes(Logic &logic);
    void setup();
    void handle();

    int checkButtons();
    int checkPassword(int,int);
    bool waitedLongEnough();
    bool buttonPressedDuringSong(unsigned long);

    int touch_currently_typed[6] = { 0, 0, 0, 0, 0, 0 };

  private:
    Logic &_logic;
    
    bool isPasswordCorrect();
};
