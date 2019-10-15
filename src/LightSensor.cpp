#include "Arduino.h"
#include "LightSensor.h"
#include "logic.h"

#define PR_PIN 36
#define PR_DARK_THRESHOLD 5
#define PR_DARK_BOUNCE 500
#define PR_LIGHT_BOUNCE 500

unsigned long dark_seen = 0;
unsigned long light_seen = 0;
LightSensor::LightSensor(Logic &logic)
: _logic(logic)
{
}

void LightSensor::setup() {
  pinMode(PR_PIN, INPUT);
}

void LightSensor::handle() {

  light_value = analogRead(PR_PIN);
  static bool isDark = light_value <= PR_DARK_THRESHOLD;

  // check for light
  if (!isDark && light_seen == 0) {
    light_seen = millis();
  } else if (!isDark &&  millis() - light_seen > PR_LIGHT_BOUNCE) {
    if (!lightDetected) {
      _logic.serial.print("detected light long enough.\n");
    }
    
    lightDetected = true;
  } else if (isDark) {
    light_seen = 0;
  }

  // check for dark
  if (isDark && dark_seen == 0) {
    dark_seen = millis();
  } else if (isDark && millis() - dark_seen > PR_DARK_BOUNCE) {
    if (lightDetected) {
      _logic.serial.print("detected dark long enough.\n");
    }

    lightDetected = false;
  } else if (!isDark) {
    dark_seen = 0;
  }

  if (debug) {
    _logic.serial.print("light: %d\n", light_value);
  }
}

bool LightSensor::isLight() {
  // TODO: do this for real.  right now just use override
  return override_light_detected;
}