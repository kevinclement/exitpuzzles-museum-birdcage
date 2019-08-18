#include "Arduino.h"
#include "Notes.h"

#define BUTTON_DELAY 50
#define BUTTON_TIME_BETWEEN_SONG 4000 // time to wait before starting the song again

// Starting @ pin 12
//   Purple, Blue, Gray, Green, <BLANK>, Orange
int touch_ports[5]                = { 14, 15, 32, 27, 12 };
int touch_thresh[5]               = { 35, 25, 15, 30, 30 };
int touch_password[6]             = { 5, 4, 2, 4, 1, 3 };
int touch_currently_typed[6]      = { 0, 0, 0, 0, 0, 0 };
unsigned long touch_last_seen[5]  = { 0, 0, 0, 0, 0 };
bool touch_rising_reported[5]     = { 0, 0, 0, 0, 0 };
unsigned long touch_first_seen[5] = { 0, 0, 0, 0, 0 };
unsigned long last_button_press = 0;
int touch_current_pass_index = 0;

Notes::Notes(Logic &logic)
: _logic(logic)
{  
}

void Notes::setup() {
}

int Notes::checkButtons() {
  int button_pressed = -1;
  
  // note: looks like 32 and 33 are switched physically.  32 will read from 33 pin, and vice versa
  for(int i=0; i<5; i++) {
    int curVal = touchRead(touch_ports[i]);
    
    // ignore 0 stutters
    if (curVal == 0) {
      continue;
    }

    if (curVal > touch_thresh[i]) {

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
    last_button_press = millis();
  }
  
  return button_pressed + 1;
}

bool isPasswordCorrect() {
  for(int i=0; i<6; i++) {
    if (touch_currently_typed[i] != touch_password[i]) {
      return false;
    }
  }

  return true;
}

int Notes::checkPassword(int buttonPressed, int d) {
  int res = -1;
  Serial.printf("%d pressed\n", buttonPressed);
  touch_currently_typed[touch_current_pass_index] = buttonPressed;

  if (touch_current_pass_index == 5) {
    // wait here so previous clip can play
    delay(d + 200);

    Serial.printf("checking final password...");
    if (isPasswordCorrect()) {
      Serial.printf("SOLVED!!!\n");
      res = 1;
    } else {
      Serial.printf("incorrect.\n");
      res = 0;
    }

    touch_current_pass_index = 0;
    last_button_press = 0;
    memset(touch_currently_typed, 0, sizeof(touch_currently_typed));
    memset(touch_last_seen, 0, sizeof(touch_last_seen));
    memset(touch_first_seen, 0, sizeof(touch_first_seen));
    memset(touch_rising_reported, 0, sizeof(touch_rising_reported));
  } else {
    touch_current_pass_index++;
  }

  return res;
}

bool Notes::waitedLongEnough() {
    return last_button_press == 0 || millis() - last_button_press > BUTTON_TIME_BETWEEN_SONG;
}

bool Notes::buttonPressedDuringSong(unsigned long played_at) {
    return last_button_press > 0 && millis() - last_button_press > BUTTON_TIME_BETWEEN_SONG && last_button_press - played_at < 25000;
}

void debugButtons() {
  int one = touchRead(touch_ports[0]);
  int two = touchRead(touch_ports[1]);
  int three = touchRead(touch_ports[2]);
  int four = touchRead(touch_ports[3]);
  int five = touchRead(touch_ports[4]);

  Serial.printf("1:%d 2:%d 3:%d 4:%d 5:%d\n", one, two, three, four, five);
}

void Notes::handle() {
  //debugButtons();
}
