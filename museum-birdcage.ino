#include "A4988.h"
#include "AudioPlayer.h"
#include "Stepper.h"
#include "Notes.h"
#include "LightSensor.h"

#define MOTOR_STEPS 200
#define MOTOR_TRAVEL 3200
#define RPM 400
#define MICROSTEPS 1
#define DIR 18
#define STEP 19
#define MS1 22
#define MS2 21
#define MS3 4
#define MOTOR_ENABLE_PIN 5

#define PR_PIN 0
#define PR_DARK_THRESHOLD 3100

#define BUTTON_THRESH 30
#define BUTTON_DELAY 50
#define RESET_TIME 180000 // 3 minutes
#define BUTTON_TIME_BETWEEN_SONG 4000 // time to wait before starting the song again

#define CMD_SEL_DEV 0X09
#define DEV_TF 0X02

int touch_password[6]             = { 5, 4, 2, 4, 1, 3 };
int touch_currently_typed[6]      = { 0, 0, 0, 0, 0, 0 };
int touch_ports[5]                = { 33, 32, 27, 14, 12 };
unsigned long touch_last_seen[5]  = { 0, 0, 0, 0, 0 };
bool touch_rising_reported[5]     = { 0, 0, 0, 0, 0 };
unsigned long touch_first_seen[5] = { 0, 0, 0, 0, 0 };
int touch_current_pass_index = 0;
unsigned long last_button_press = 0;

#define TRACK_FULL 6
#define TRACK_FAILED 7
#define TRACK_WINNING 8
int track_lengths_ms[8] = { 600,1500,1800,450,3850,24200,1100,4200 };

bool SOLVED = false;
unsigned long solved_at = 0;
bool TRAY_OUT = false;
bool ENABLED = true;
bool PLAYING_SONG = false;
unsigned long playing_song_at = 0;

A4988 stepper(MOTOR_STEPS, DIR, STEP, MS1, MS2, MS3);

AudioPlayer player;
LightSensor lightSensor;
Notes notes;
Stepper stepper2;

//HardwareSerial Serial1(1);
static int8_t Send_buf[8] = {0};

void setup() {
  Serial.begin(115200);
  Serial.println("Museum Birdcage by kevinc...");
  Serial1.begin(9600, SERIAL_8N1, 16, 17);

  player.setup();
  lightSensor.setup();
  notes.setup();
  stepper2.setup();
  
  pinMode(PR_PIN, INPUT);
  pinMode(MOTOR_ENABLE_PIN, OUTPUT);

  stepper.setSpeedProfile(BasicStepperDriver::LINEAR_SPEED);
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

void checkPassword(int buttonPressed) {
  Serial.printf("%d pressed\n", buttonPressed);
  touch_currently_typed[touch_current_pass_index] = buttonPressed;
    
  if (touch_current_pass_index == 5) {
    // wait here so previous clip can play
    delay(track_lengths_ms[buttonPressed-1] + 200);
    
    Serial.printf("checking final password...");
    if (isPasswordCorrect()) {
      Serial.printf("SOLVED!!!\n");
      SOLVED = true;
      solved_at = millis();
    } else {
      playTrack(TRACK_FAILED, false);
      Serial.printf("incorrect.\n");
      reset();
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

void reset() {
  solved_at = 0;
  SOLVED = false;
  touch_current_pass_index = 0;
  TRAY_OUT = false;
  ENABLED = true;

  last_button_press = 0;
  memset(touch_currently_typed, 0, sizeof(touch_currently_typed));
  memset(touch_last_seen, 0, sizeof(touch_last_seen));
  memset(touch_first_seen, 0, sizeof(touch_first_seen));
  memset(touch_rising_reported, 0, sizeof(touch_rising_reported));
}

void loop() {

  // keep motor off to reduce wine, only turn it on before we are going to use it  
  digitalWrite(MOTOR_ENABLE_PIN, HIGH);

  // check for light, only enable the device when its dark
  int light_value = analogRead(PR_PIN);  
  ENABLED = light_value <= PR_DARK_THRESHOLD;

  // if its not enabled, then NOOP
  if (!ENABLED) {
    if (PLAYING_SONG) {
      Serial.printf("Stopping song since its disabled\n");
      sendCommand(0x16,0x00);
      PLAYING_SONG = false;
      playing_song_at = 0;
    }
    return;
  }
  
  if (SOLVED) {
    if (!TRAY_OUT) {
      playTrack(TRACK_WINNING, false);
      digitalWrite(MOTOR_ENABLE_PIN, LOW);     
      stepper.rotate(MOTOR_TRAVEL);
      digitalWrite(MOTOR_ENABLE_PIN, HIGH);
      TRAY_OUT = true;
    } else if (millis() - solved_at > RESET_TIME) {
      Serial.printf("Resetting tray...\n");
      digitalWrite(MOTOR_ENABLE_PIN, LOW);
      stepper.rotate(-MOTOR_TRAVEL);
      digitalWrite(MOTOR_ENABLE_PIN, HIGH);
      ENABLED = false;
    }

    // NOOP the rest if we've solved it
    return;
  }

  if (!PLAYING_SONG) {
    if (last_button_press == 0 || millis() - last_button_press > BUTTON_TIME_BETWEEN_SONG) {
      Serial.printf("playing song...\n");
      PLAYING_SONG = true;
      playing_song_at = millis();
      playTrack(TRACK_FULL, true);
    }
  } else {

    // if totally timed out, restart it
    if (millis() - playing_song_at > 25000) {
      Serial.printf("done with song, starting again...\n");    
      PLAYING_SONG = false;
      playing_song_at = 0;
    }

    // if there was a button press between our start time, then restart it
    if (last_button_press > 0 && millis() - last_button_press > BUTTON_TIME_BETWEEN_SONG && last_button_press - playing_song_at < 25000) {
      Serial.printf("restarting song from button press...\n");    
      PLAYING_SONG = false;
      playing_song_at = 0;
    }
  }

  int buttonPressed = checkButtons();
  if (buttonPressed != 0) {    
    playTrack(buttonPressed, true);
    checkPassword(buttonPressed);
  }
}
