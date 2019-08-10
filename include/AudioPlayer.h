#pragma once

#include "Arduino.h"

class Logic;

class AudioPlayer {
  public:
    AudioPlayer(Logic &logic);
    void setup();
    void handle();

    void play(int8_t track, bool loud);
    void stop();

    int track_lengths_ms[8] = { 600,1500,1800,450,3850,24200,1100,4200 };
    bool playing = false;

  private:
    Logic &_logic;

};
