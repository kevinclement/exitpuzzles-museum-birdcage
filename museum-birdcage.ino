#define LED_PIN 2
#define BUTTON_THRESH 30
#define BUTTON_DELAY 50

int touch_password[6]             = { 5, 4, 2, 4, 1, 3 };
int touch_currently_typed[6]      = { 0, 0, 0, 0, 0, 0 };
int touch_ports[5]                = { 33, 32, 27, 14, 12 };
unsigned long touch_last_seen[5]  = { 0, 0, 0, 0, 0 };
bool touch_rising_reported[5]     = { 0, 0, 0, 0, 0 };
unsigned long touch_first_seen[5] = { 0, 0, 0, 0, 0 };
int touch_current_pass_index = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("Museum Birdcage by kevinc...");
  pinMode(LED_PIN, OUTPUT);
  digitalWrite (LED_PIN, LOW);
}

int checkButtons() {
  int button_pressed = -1;
  
  // note: looks like 32 and 33 are switched physically.  32 will read from 33 pin, and vice versa
  for(int i=0; i<5; i++) {
    if (touchRead(touch_ports[i]) > BUTTON_THRESH) {

      // check on rising edge if we've been touching it already
      if (touch_first_seen[i] != 0 && (millis() - touch_first_seen[i] > BUTTON_DELAY)) {
        //Serial.printf("%d falling edge\n", i+1);
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
          //Serial.printf("%d rising edge\n", i+1);          
          button_pressed = i;
        }
      } else {
        touch_last_seen[i] = millis();
      }
    }
  }

  if (button_pressed >= 0) { 
    touch_rising_reported[button_pressed] = true;
  }
  
  return button_pressed + 1;
}

bool checkPassword() {
  for(int i=0; i<6; i++) {
    if (touch_currently_typed[i] != touch_password[i]) {
      return false;
    }
  }

  return true;
}

void loop() {
  int buttonPressed = checkButtons();
  if (buttonPressed != 0) {
    Serial.printf("%d pressed\n", buttonPressed);
    touch_currently_typed[touch_current_pass_index] = buttonPressed;
    
    if (touch_current_pass_index == 5) {
      Serial.printf("checking final password...\n");
      if (checkPassword()) {
        Serial.printf("SOLVED!!!\n");
      } else {
        // now reset the current password stuff
        touch_current_pass_index = 0;
        touch_currently_typed[0] = touch_currently_typed[1] = touch_currently_typed[2] = touch_currently_typed[3] = touch_currently_typed[4] = touch_currently_typed[5] = 0;
      }
    } else {
      touch_current_pass_index++;  
    }
  }
}
