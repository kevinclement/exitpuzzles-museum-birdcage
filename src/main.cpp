#include <Arduino.h>
#include "logic.h"
#include "consts.h"
#include "version.h"

// Globals
Logic logic;

void reboot(int) {
  ESP.restart();
}

void status(int) {
  logic.status();
}

void solve(int) {
  logic.serial.print("forcing a puzzle solve...%s", CRLF);
  logic.solved();
}

void closeTray(int) {
  logic.serial.print("closing the device tray...%s", CRLF);
  logic.close();
}

void forward(int) {
  logic.serial.print("forward now...%s", CRLF);
  logic.stepmotor.forward();
}

void back(int) {
  logic.serial.print("back now...%s", CRLF);
  logic.stepmotor.back();
}

void toggleIsLight(int) {
  logic.serial.print("toggling override of isLight...%s", CRLF);
  logic.lightsensor.override_light_detected = !logic.lightsensor.override_light_detected;
}

void debug(int) {
  logic.serial.print("toggling debug of light sensor...%s", CRLF);
  logic.lightsensor.debug = !logic.lightsensor.debug;
}

void setup() {
  logic.setup();
  logic.serial.print("Museum Birdcage by kevinc...\n");
  Serial.println(getFullVersion("museum-birdcage"));

  logic.serial.registerCommand(SerialCommand("status",  's', &status,        "status",  "gets the status of device"));
  logic.serial.registerCommand(SerialCommand("solve",   'v', &solve,         "solve",   "force a puzzle solve of the device"));
  logic.serial.registerCommand(SerialCommand("close",   'c', &closeTray,     "close",   "close the device tray"));
  logic.serial.registerCommand(SerialCommand("forward", 'f', &forward,       "forward", "move the tray forward a small amount"));
  logic.serial.registerCommand(SerialCommand("back",    'b', &back,          "back",    "move the tray backward a small amount"));
  logic.serial.registerCommand(SerialCommand("light",   'l', &toggleIsLight, "light",   "toggle override of isLight detection"));
  logic.serial.registerCommand(SerialCommand("debug",   'x', &debug,         "debug",   "toggle debug of light sensor"));
  logic.serial.registerCommand(SerialCommand("reboot",  'r', &reboot,        "reboot",  "software reboot the device"));

  logic.serial.printHelp();
  logic.status();
}

void loop() {
  logic.handle();
}