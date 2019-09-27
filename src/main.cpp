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

void setup() {
  logic.setup();
  logic.serial.print("Museum Birdcage by kevinc...\n");
  Serial.println(getFullVersion("museum-birdcage"));

  logic.serial.registerCommand(SerialCommand("status", 's', &status,    "status", "gets the status of device"));
  logic.serial.registerCommand(SerialCommand("solve",  'v', &solve,     "solve",  "force a puzzle solve of the device"));
  logic.serial.registerCommand(SerialCommand("close",  'c', &closeTray, "close",  "close the device tray"));
  logic.serial.registerCommand(SerialCommand("reboot", 'r', &reboot,    "reboot", "software reboot the device"));

  logic.serial.printHelp();
  logic.status();
}

void loop() {
  logic.handle();
}

  // else if (command == "open" || command == "o") {
  //   print("opening device now...%s", CRLF);
  //   _logic.open();
  
  // else if (command == "close" || command == "c") {
  //   print("closing device now...%s", CRLF);
  //   _logic.close();
  
  // else if (command == "b") {
  //   print("back now...%s", CRLF);
  //   _logic.stepmotor.back();
  
  // else if (command == "f") {
  //   print("forward now...%s", CRLF);
  //   _logic.stepmotor.forward();
  
  
  // else if (command == "status") {
  //   //printVariables();
  
  // else if (command == "reset" || command == "r") {
  //   ESP.restart();
  