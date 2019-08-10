#include "Arduino.h"
#include "LightSensor.h"

#define PR_PIN 36
#define PR_DARK_THRESHOLD 100

LightSensor::LightSensor(Logic &logic)
: _logic(logic)
{
}

void LightSensor::setup() {
  pinMode(PR_PIN, INPUT);
}

void LightSensor::handle() {

  int light_value = analogRead(PR_PIN);  
  lightDetected = light_value > PR_DARK_THRESHOLD;

  //Serial.printf("light: %d\n", light_value);
}
