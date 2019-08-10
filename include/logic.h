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

  void open();
  void close();

private:
  void readStoredVariables();
  void printVariables();
};

