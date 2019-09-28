#include "WifiUpdate.h"
#include "logic.h"
#include <WiFi.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

const char* ssid = "ExitPuzzles";
const char* password = "bossdj311";

WifiUpdate::WifiUpdate(Logic &logic)
: _logic(logic)
{  
}

void WifiUpdate::setup() {
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed!");
    // delay(5000);
    // ESP.restart();
  }

  // setup wifi updates
  ArduinoOTA.setHostname("museum-bird");
  ArduinoOTA
    .onStart([]() {
      Serial.println("Start updating...");
    })
    .onEnd([]() {
      Serial.println("\r\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR)         Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR)   Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR)     Serial.println("End Failed");
    });

  ArduinoOTA.begin();
}

void WifiUpdate::handle() {
  ArduinoOTA.handle();
}