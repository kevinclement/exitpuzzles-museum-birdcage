#include "A4988.h"

#define MOTOR_STEPS 200
#define RPM 200
#define MICROSTEPS 1
#define DIR 18
#define STEP 19
#define MS1 22
#define MS2 21
#define MS3 4

#define LED_PIN 2
#define BUTTON_THRESH 30
#define BUTTON_DELAY 50

#define CMD_SEL_DEV 0X09
#define DEV_TF 0X02
#define CMD_PLAY_W_VOL 0X22
#define CMD_PLAY 0X0D
#define CMD_PAUSE 0X0E
#define CMD_PREVIOUS 0X02
#define CMD_NEXT 0X01

int touch_password[6]             = { 5, 4, 2, 4, 1, 3 };
int touch_currently_typed[6]      = { 0, 0, 0, 0, 0, 0 };
int touch_ports[5]                = { 33, 32, 27, 14, 12 };
unsigned long touch_last_seen[5]  = { 0, 0, 0, 0, 0 };
bool touch_rising_reported[5]     = { 0, 0, 0, 0, 0 };
unsigned long touch_first_seen[5] = { 0, 0, 0, 0, 0 };
int touch_current_pass_index = 0;

#define TRACK_FULL 6
#define TRACK_FAILED 7
#define TRACK_WINNING 8
int track_lengths_ms[8] = { 600,1500,1800,450,3850,24200,1100,4200 };

bool SOLVED = false;
bool TRAY_OUT = false;
bool ENABLED = true;

A4988 stepper(MOTOR_STEPS, DIR, STEP, MS1, MS2, MS3);

HardwareSerial Serial1(1);
static int8_t Send_buf[8] = {0};



void setup() {
  Serial.begin(115200);
  Serial.println("Museum Birdcage by kevinc...");
  Serial1.begin(9600, SERIAL_8N1, 16, 17);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite (LED_PIN, LOW);
  stepper.begin(RPM, 16);
  
  delay(500);//Wait chip initialization is complete
  sendCommand(CMD_SEL_DEV, DEV_TF);//select the TF card  
  delay(200);
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

bool isPasswordCorrect() {
  for(int i=0; i<6; i++) {
    if (touch_currently_typed[i] != touch_password[i]) {
      return false;
    }
  }

  return true;
}

void checkPassword(int buttonPressed) {
  Serial.printf("%d pressed\n", buttonPressed);
  touch_currently_typed[touch_current_pass_index] = buttonPressed;
    
  if (touch_current_pass_index == 5) {
    // wait here so previous clip can play
    delay(track_lengths_ms[buttonPressed]);
    
    Serial.printf("checking final password...");
    if (isPasswordCorrect()) {
      Serial.printf("SOLVED!!!\n");
      SOLVED = true;
    } else {
      playTrack(TRACK_FAILED, false);
      Serial.printf("incorrect.\n");
      touch_current_pass_index = 0;
      memset(touch_currently_typed, 0, sizeof(touch_currently_typed));
    }
  } else {
    touch_current_pass_index++;  
  }
}

void sendCommand(int8_t command, int16_t dat)
{
  delay(20);
  Send_buf[0] = 0x7e; //starting byte
  Send_buf[1] = 0xff; //version
  Send_buf[2] = 0x06; //the number of bytes of the command without starting byte and ending byte
  Send_buf[3] = command; //
  Send_buf[4] = 0x00;//0x00 = no feedback, 0x01 = feedback
  Send_buf[5] = (int8_t)(dat >> 8);//datah
  Send_buf[6] = (int8_t)(dat); //datal
  Send_buf[7] = 0xef; //ending byte
  for(uint8_t i=0; i<8; i++)//
  {
    Serial1.write(Send_buf[i]) ;
  }
}

void playTrack(int8_t track, bool loud)
{
  // loud=60%, soft=22%
  sendCommand(06, loud ? 0x3C : 0x16);
  
  delay(20);
  Send_buf[0] = 0x7e; //starting byte
  Send_buf[1] = 0xff; //version
  Send_buf[2] = 0x06; //the number of bytes of the command without starting byte and ending byte
  Send_buf[3] = 0x0F; //
  Send_buf[4] = 0x00;//0x00 = no feedback, 0x01 = feedback
  Send_buf[5] = 0x01;
  Send_buf[6] = track; 
  Send_buf[7] = 0xef; //ending byte
  for(uint8_t i=0; i<8; i++)//
  {
    Serial1.write(Send_buf[i]);
  }
}


void loop() {
  
  // NOOP if we've solved it
  if (SOLVED) {

    if (!TRAY_OUT) {
      playTrack(TRACK_WINNING, false);
      stepper.rotate(400);
      delay(2000);
      stepper.rotate(-400);
      TRAY_OUT = true;
    }
    
    return;
  }
  
  int buttonPressed = checkButtons();
  if (buttonPressed != 0) {    
    playTrack(buttonPressed, true);
    checkPassword(buttonPressed);
  }
}
