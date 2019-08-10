#include "Arduino.h"
#include "Stepper.h"
#include "A4988.h"

#define MOTOR_STEPS 200
#define MOTOR_TRAVEL 3200
#define RPM 400
#define MICROSTEPS 1
#define DIR 5
#define STEP 18
#define MS1 21
#define MS2 19
#define MS3 25
#define MOTOR_ENABLE_PIN 4

A4988 stepper(MOTOR_STEPS, DIR, STEP, MS1, MS2, MS3);

Stepper::Stepper(Logic &logic)
: _logic(logic)
{  
}

void Stepper::setup() {
  stepper.setSpeedProfile(BasicStepperDriver::LINEAR_SPEED);
  stepper.begin(RPM, 16);
}

void Stepper::open() {
    stepper.rotate(MOTOR_TRAVEL);
}

void Stepper::close() {
    stepper.rotate(-MOTOR_TRAVEL);
}

void Stepper::handle() {
}
