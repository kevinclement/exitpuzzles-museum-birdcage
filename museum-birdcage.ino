#define LED_PIN 2
#define BUTTON_THRESH 30
#define BUTTON_DELAY 50

int touch_ports[5]                = { 33, 32, 27, 14, 12 };
unsigned long touch_last_seen[5]  = { 0, 0, 0, 0, 0 };
bool touch_rising_reported[5]     = { 0, 0, 0, 0, 0 };
unsigned long touch_first_seen[5] = { 0, 0, 0, 0, 0 };

void setup() {
  Serial.begin(115200);
  Serial.println("Museum Birdcage by kevinc...");
  pinMode(LED_PIN, OUTPUT);
  digitalWrite (LED_PIN, LOW);
}

void checkButtons() {
  // note: looks like 32 and 33 are switched physically.  32 will read from 33 pin, and vice versa
  for(int i=0; i<5; i++) {
    int v = touchRead(touch_ports[i]);
    int tls = touch_last_seen[i];

    if (v > BUTTON_THRESH) {

      // check on rising edge if we've been touching it already
      if (touch_first_seen[i] != 0 && (millis() - touch_first_seen[i] > BUTTON_DELAY)) {
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
        if (millis() - touch_last_seen[i] > BUTTON_DELAY && !touch_rising_reported[i]) {
          Serial.print(i + 1);
          Serial.println(" rising edge");
          touch_rising_reported[i] = true;
        }
      } else {
        touch_last_seen[i] = millis();
      }
    }
  }
}

void loop() {
  checkButtons();
  
}
