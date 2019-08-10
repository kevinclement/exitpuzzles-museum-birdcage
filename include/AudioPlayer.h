#pragma once

#include "Arduino.h"

class Logic;

class AudioPlayer {
  public:
    AudioPlayer(Logic &logic);
    void setup();
    void handle();

    int track_lengths_ms[8] = { 600,1500,1800,450,3850,24200,1100,4200 };

  private:
    Logic &_logic;

};
