#include "Arduino.h"
#include "AudioPlayer.h"

#define CMD_SEL_DEV 0X09
#define DEV_TF 0X02

static int8_t Send_buf[8] = {0};

void sendCommand(int8_t command, int16_t dat);

AudioPlayer::AudioPlayer(Logic &logic)
: _logic(logic)
{  
}

void AudioPlayer::setup() {
  Serial1.begin(9600, SERIAL_8N1, 16, 17);
  delay(500);//Wait chip initialization is complete
  sendCommand(CMD_SEL_DEV, DEV_TF);//select the TF card
  delay(200);

  // CMD_QUERY_FLDR_COUNT
  sendCommand(0x4f, 0);
  delay(500);

  // CMD_QUERY_TOT_TRACKS
  sendCommand(0x48, 0);
  delay(500);
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

void AudioPlayer::play(int8_t track, bool loud)
{
  playing = true;
  playing_song_at = millis();

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

void AudioPlayer::previous()
{
  playing = true;

  // loud=60%, soft=22%
  // sendCommand(06, loud ? 0x3C : 0x16);
  
  delay(20);
  Send_buf[0] = 0x7e; //starting byte
  Send_buf[1] = 0xff; //version
  Send_buf[2] = 0x06; //the number of bytes of the command without starting byte and ending byte
  Send_buf[3] = 0x02; //
  Send_buf[4] = 0x00;//0x00 = no feedback, 0x01 = feedback
  Send_buf[5] = 0x00;
  Send_buf[6] = 0x00; 
  Send_buf[7] = 0xef; //ending byte
  for(uint8_t i=0; i<8; i++)//
  {
    Serial1.write(Send_buf[i]);
  }
}

void AudioPlayer::next()
{
  playing = true;

  // loud=60%, soft=22%
  // sendCommand(06, loud ? 0x3C : 0x16);
  
  delay(20);
  Send_buf[0] = 0x7e; //starting byte
  Send_buf[1] = 0xff; //version
  Send_buf[2] = 0x06; //the number of bytes of the command without starting byte and ending byte
  Send_buf[3] = 0x01; //
  Send_buf[4] = 0x00;//0x00 = no feedback, 0x01 = feedback
  Send_buf[5] = 0x00;
  Send_buf[6] = 0x00; 
  Send_buf[7] = 0xef; //ending byte
  for(uint8_t i=0; i<8; i++)//
  {
    Serial1.write(Send_buf[i]);
  }
}

void AudioPlayer::stop() {
  sendCommand(0x16,0x00);
  playing = false;
  playing_song_at = 0;
}

bool AudioPlayer::finished() {
  return millis() - playing_song_at > 25000;
}

static uint8_t ansbuf[10] = {0}; // Buffer for the answers.    // BETTER LOCALLY

String sbyte2hex(uint8_t b)
{
  String shex;

  shex = "0X";

  if (b < 16) shex += "0";
  shex += String(b, HEX);
  shex += " ";
  return shex;
}

String sanswer(void)
{
  uint8_t i = 0;
  String answer = "";

  // Get only 10 Bytes
  while (Serial1.available() && (i < 10))
  {
    uint8_t b = Serial1.read();
    ansbuf[i] = b;
    i++;

    answer += sbyte2hex(b);
  }

  // if the answer format is correct.
  if ((ansbuf[0] == 0x7E) && (ansbuf[9] == 0xEF))
  {
    return answer;
  }

  return "???: " + answer;
}

String AudioPlayer::decodeOutput() {
  String decodedOutput = "";

  decodedOutput += sanswer();

  switch (ansbuf[3]) {
    case 0x3A:
      decodedOutput += " -> Memory card inserted.";
      break;

    case 0x3D:
      decodedOutput += " -> Completed play num " + String(ansbuf[6], DEC);
      break;

    case 0x40:
      decodedOutput += " -> Error";
      break;

    case 0x41:
      decodedOutput += " -> Data recived correctly. ";
      break;

    case 0x42:
      decodedOutput += " -> Status playing: " + String(ansbuf[6], DEC);
      break;

    case 0x48:
      decodedOutput += " -> File count: " + String(ansbuf[6], DEC);
      break;

    case 0x4C:
      decodedOutput += " -> Playing: " + String(ansbuf[6], DEC);
      break;

    case 0x4E:
      decodedOutput += " -> Folder file count: " + String(ansbuf[6], DEC);
      break;

    case 0x4F:
      decodedOutput += " -> Folder count: " + String(ansbuf[6], DEC);
      break;
  }

  return decodedOutput;
}

void AudioPlayer::handle() {
  // if (Serial1.available())
  // {
  //   Serial.println(decodeOutput());
  // }
}
