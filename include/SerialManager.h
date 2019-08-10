#pragma once

class Logic;

class SerialManager {
public:
  SerialManager(Logic &logic);
  void setup();
  void handle();
  void print(const char *fmt, ...);
  void printHelp();

private:
  Logic &_logic;

  void handleMessage(String msg);
  void readAnyBluetoothMessage();
  void readAnySerialMessage();
};

