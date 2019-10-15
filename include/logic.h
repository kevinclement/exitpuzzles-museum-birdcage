#pragma once

#include "SerialManager.h"
#include "AudioPlayer.h"
#include "Stepper.h"
#include "Notes.h"
#include "LightSensor.h"

class Logic {
public:
  Logic();
  SerialManager serial;
  AudioPlayer audio;
  Stepper stepmotor;
  Notes notes;
  LightSensor lightsensor;

  void setup();
  void handle();

  void close();
  void solved();
  void status();

  String cur_password;

private:

  bool _isLight = true;
};

