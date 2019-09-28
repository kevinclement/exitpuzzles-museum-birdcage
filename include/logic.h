#pragma once

#include "SerialManager.h"
#include "AudioPlayer.h"
#include "Stepper.h"
#include "Notes.h"
#include "LightSensor.h"
#include "wifi.h"

class Logic {
public:
  Logic();
  SerialManager serial;
  AudioPlayer audio;
  Stepper stepmotor;
  Notes notes;
  LightSensor lightsensor;
  Wifi wifi;

  void setup();
  void handle();

  void close();
  void solved();
  void status();

private:

};

