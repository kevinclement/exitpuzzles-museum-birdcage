#include "Arduino.h"
#include "LightSensor.h"
#include "logic.h"

#define PR_PIN 36
#define PR_DARK_THRESHOLD 10

enum STATE {
  UNKNOWN,
  DARK,
  LIGHT
};

unsigned long timer = millis();
double avg_total = 0;
double total_measures = 0;

STATE current_state = UNKNOWN;

LightSensor::LightSensor(Logic &logic)
: _logic(logic)
{
}

void LightSensor::setup() {
  pinMode(PR_PIN, INPUT_PULLDOWN);
}

void LightSensor::handle() {

  light_value = analogRead(PR_PIN);
  avg_total += light_value;
  total_measures++;

  if (debug) {
    _logic.serial.print("light: %d\n", light_value);
  }

  // check values every 1s
  if (millis() - timer >= 4000) {
    double avg = avg_total / total_measures;

    if (debugAvg) {
      _logic.serial.print("average: %f\n", avg);
    }

    STATE _state = avg <= PR_DARK_THRESHOLD ? DARK : LIGHT;

    if (_state == current_state) {
      lightDetected = _state == LIGHT;
    }

    current_state = _state;

    // reset timer and counts just so we don't hit some overflow condition
    timer = millis();
    avg_total = 0;
    total_measures = 0;
  }
}

bool LightSensor::isLight() {
  return lightDetected || !darkDetectionEnabled;
}