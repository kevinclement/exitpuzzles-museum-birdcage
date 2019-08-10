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

  private:
    Logic &_logic;
};
