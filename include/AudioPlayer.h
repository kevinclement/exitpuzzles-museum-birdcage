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
    bool finished();

    int track_lengths_ms[8] = { 600,1500,1800,450,3850,24200,1100,4200 };
    bool playing = false;
    unsigned long playing_song_at = 0;

    const int8_t TRACK_FULL = 6;
    const int8_t TRACK_FAILED = 7;
    const int8_t TRACK_WINNING = 8;

  private:
    Logic &_logic;

};
