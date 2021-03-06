#include "Arduino.h"
#include "Stepper.h"
#include "A4988.h"
#include "logic.h"
#include <Preferences.h>

#define MOTOR_STEPS 200
#define MOTOR_TRAVEL 3600
#define RPM 400
#define MICROSTEPS 1
#define DIR 5
#define STEP 18
#define MS1 21
#define MS2 19
#define MS3 25
#define MOTOR_ENABLE_PIN 4

Preferences preferences;
A4988 stepper(MOTOR_STEPS, DIR, STEP, MS1, MS2, MS3);

Stepper::Stepper(Logic &logic)
: _logic(logic)
{  
}

void Stepper::setup() {
  pinMode(MOTOR_ENABLE_PIN, OUTPUT);

  stepper.setSpeedProfile(BasicStepperDriver::LINEAR_SPEED);
  stepper.begin(RPM, 16);

  // pull tray state out of memory that lasts between reboot
  preferences.begin("museum-birdcage", false);
  tray_out = preferences.getBool("tray_out", false);

  // if the tray is out and we've rebooted, we should close it
  if (tray_out) {
    _logic.serial.print("WARN: tray was opened on reboot.  closing now...\n");
    close();
  }
}

void Stepper::open() {
  digitalWrite(MOTOR_ENABLE_PIN, LOW);
  stepper.rotate(MOTOR_TRAVEL);
  digitalWrite(MOTOR_ENABLE_PIN, HIGH);
  tray_out = true;
  preferences.putBool("tray_out", true);
  _logic.status();
}

void Stepper::close() {
  digitalWrite(MOTOR_ENABLE_PIN, LOW);
  stepper.rotate(-MOTOR_TRAVEL);
  digitalWrite(MOTOR_ENABLE_PIN, HIGH);
  tray_out = false;
  preferences.putBool("tray_out", false);
  _logic.status();
}

void Stepper::back() {
  digitalWrite(MOTOR_ENABLE_PIN, LOW);
  stepper.rotate(-100);
  digitalWrite(MOTOR_ENABLE_PIN, HIGH);
}

void Stepper::forward() {
  digitalWrite(MOTOR_ENABLE_PIN, LOW);
  stepper.rotate(100);
  digitalWrite(MOTOR_ENABLE_PIN, HIGH);
}

void Stepper::handle() {
  // keep motor off to reduce wine, only turn it on before we are going to use it  
  digitalWrite(MOTOR_ENABLE_PIN, HIGH);
}
