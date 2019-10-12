#pragma once

#include "SerialManager.h"
#include "AudioPlayer.h"
#include "Stepper.h"
#include "Notes.h"
#include "LightSensor.h"
#include "WifiUpdate.h"

class Logic {
public:
  Logic();
  SerialManager serial;
  AudioPlayer audio;
  Stepper stepmotor;
  Notes notes;
  LightSensor lightsensor;
  WifiUpdate wifi;

  void setup();
  void handle();

  void close();
  void solved();
  void status();

  bool override_light_sensor = false;
  bool override_play = false;
  bool override_stop = false;
  String cur_password;

private:

};

