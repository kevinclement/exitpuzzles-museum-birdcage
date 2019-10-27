#include "Arduino.h"
#include "LightSensor.h"
#include "logic.h"

#define PR_PIN 36
//#define PR_DARK_THRESHOLD 5
#define PR_DARK_THRESHOLD 100
#define PR_DARK_BOUNCE 2000
#define PR_LIGHT_BOUNCE 2000

enum STATE {
  UNKNOWN,
  DARK,
  LIGHT
};

unsigned long dark_seen = 0;
unsigned long light_seen = 0;
unsigned long timer = millis();
static bool isDark = false;
double avg_total = 0;
double avg_light_value = 0;
double total_measures = 0;
STATE last_signal = UNKNOWN;

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
// isDark = light_value <= PR_DARK_THRESHOLD;
  if (debug) {
    _logic.serial.print("light: %d\n", light_value);
  }

  // check values every 1s
  if (millis() - timer >= 1000) {
    double avg = avg_total / total_measures;
    
    if (debugAvg) {
      _logic.serial.print("average: %f\n", avg);
    }

    static bool firstRead = true;
    static bool alerted = false;
    bool _isDark = avg <= PR_DARK_THRESHOLD;
    
    if (firstRead) {
      firstRead = false;
    } else if (isDark == _isDark) {
      if (!alerted) {
        if (isDark) {
          if (last_signal != DARK) {
            Serial.println("DARK");
          }

          last_signal = DARK;
        } else {
          if (last_signal != LIGHT) {
            Serial.println("LIGHT");
          }

          last_signal = LIGHT;
        }
        alerted = true;
      }
    } else {
      alerted = false;
    }

    isDark = _isDark;
    
    // reset timer and counts just so we don't hit some overflow condition
    timer = millis();
    avg_total = 0;
    total_measures = 0;
  }
  // if (timer == 0) {
  //   timer = millis();
  //   avg_light_value = 0;
  //   avg_light_total = 0;
  //   total_measures = 0;
  // } else {
  //   avg_light_total += light_value;
  //   avg_light_value = avg_light_total / total_measures;
  //   if (millis() - timer >= 2000) {
  //     Serial.println("!! its been two seconds !!");
  //     timer = 0;
  //   }
  // }

  // check for light
  // if (!isDark && light_seen == 0) {
  //   light_seen = millis();
  // } else if (!isDark &&  millis() - light_seen > PR_LIGHT_BOUNCE) {
  //   if (!lightDetected) {
  //     _logic.serial.print("detected light long enough.\n");
  //   }
    
  //   lightDetected = true;
  // } else if (isDark) {
  //   light_seen = 0;
  // }

  // // check for dark
  // if (isDark && dark_seen == 0) {
  //   dark_seen = millis();
  // } else if (isDark && millis() - dark_seen > PR_DARK_BOUNCE) {
  //   if (lightDetected) {
  //     _logic.serial.print("detected dark long enough.\n");
  //   }

  //   lightDetected = false;
  // } else if (!isDark) {
  //   dark_seen = 0;
  // }
}

bool LightSensor::isLight() {
  return lightDetected || !darkDetectionEnabled;
}