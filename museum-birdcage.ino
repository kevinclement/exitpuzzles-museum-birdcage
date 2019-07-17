#include <Arduino.h>

#define LED_PIN 2
#define BUTTON_THRESH 30

int lastButtonState = LOW;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;
int ledState = HIGH;
int buttonState;

int touch_ports[5]                = { 33, 32, 27, 14, 12 };
unsigned long touch_last_seen[5]  = { 0, 0, 0, 0, 0 };
bool touch_rising_reported[5]     = { 0, 0, 0, 0, 0 };
unsigned long touch_first_seen[5] = { 0, 0, 0, 0, 0 };

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("cap touch test by kevinc...");
  pinMode(LED_PIN, OUTPUT);
  digitalWrite (LED_PIN, LOW);
}

void checkv(int v, int l) {
  if (v <= BUTTON_THRESH) {
    Serial.print(l);
    Serial.println(" below threshold");
  }
}

void loop() {
  
  // # note: looks like 32 and 33 are switched physically.  32 will read from 33 pin, and vice versa
  for(int i=0; i<5; i++) {
    int v = touchRead(touch_ports[i]);
    int tls = touch_last_seen[i];

    if (v > BUTTON_THRESH) {

      // check on rising edge if we've been touching it already
      if (touch_first_seen[i] != 0 && (millis() - touch_first_seen[i] > debounceDelay)) {
        Serial.print(i + 1);
        Serial.println(" failing edge");
      }

      // reset last seen
      touch_last_seen[i] = 0;
      touch_first_seen[i] = 0;
      touch_rising_reported[i] = false;
    } else {

      if (touch_first_seen[i] == 0) {
        touch_first_seen[i] = millis();       
      }

      // below thresh, so lets check how long
      if (touch_last_seen[i] != 0) {
        // rising edge
        if (millis() - touch_last_seen[i] > debounceDelay && !touch_rising_reported[i]) {
          Serial.print(i + 1);
          Serial.println(" rising edge");
          touch_rising_reported[i] = true;
        }
      } else {
        touch_last_seen[i] = millis();
      }
    }
  }
  
//  int five_value  = touchRead(12);    // white
//  int four_value  = touchRead(14);    // yellow  
//  int three_value  = touchRead(27);    // green
//  int two_value  = touchRead(32);    // blue 
//  int one_value = touchRead(33);    // black
//
//  checkv(five_value, "five");
//  checkv(four_value, "four");
//  checkv(three_value, "three");
//  checkv(two_value, "two");
//  checkv(one_value, "one");

//  int reading = LOW;
//  if (g_value <= BUTTON_THRESH) {
//    reading = HIGH;
//  }
//
//  // Serial.println(g_value);
//  
//  if (reading != lastButtonState) {
//    // reset the debouncing timer
//    lastDebounceTime = millis();
//  }
//
//
//
// if ((millis() - lastDebounceTime) > debounceDelay) {
//    // whatever the reading is at, it's been there for longer than the debounce
//    // delay, so take it as the actual current state:
//
//    // if the button state has changed:
//    if (reading != buttonState) {
//      buttonState = reading;
//
//      // only toggle the LED if the new button state is HIGH
//      if (buttonState == HIGH) {
//        ledState = !ledState;
//        Serial.println("toggled");
//      }
//    }
//  }
//
//  digitalWrite(LED_PIN, ledState);
//  lastButtonState = reading;

//  Serial.print("green: ");
//  Serial.println(g_value); 
//  if (y_value < 30 || w_value < 30 || g_value < 30 || b_value < 30 || bl_value < 30)
//  {
//    digitalWrite (LED_PIN, HIGH);
//  }
//  else
//  {
//    digitalWrite (LED_PIN, LOW);
//  }
}
