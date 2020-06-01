//Based on Handwash1 by quintbuilds. Just some minor cleanup.
#include <Adafruit_LEDBackpack.h>   //backback header
#include <Adafruit_GFX.h>           //Graphics header
#include <Wire.h>                   //not sure

int wash_time = 0;            //time washing
char countOnes = '0'; //counter for ones position
char countTens = '0'; //counter for tens position
int timeout_count = 0;            //timeout timer

//if you are putting the IR LED on the other side of the sink,
//then set this to true.
const bool break_beam = false;

//set this to the input pin you are using
const int ir_input = 5;

//set this to the seconds you want, please note that maximum is 99
int time_to_wash = 20;

//true = hold, false = single-swipe
bool hold_mode = false;

// added boolean for reset functionality
bool no_soap_reset = false;

//If the above is 1, then set this to the soaping delay you want. Otherwise this variable is useless.
int soap_hold_time = 5000; //Recommended

int text_hold_time = 750;

Adafruit_AlphaNum4 alpha4 = Adafruit_AlphaNum4();  //simplifies the class name

//setup four characters on the display
void showChars(char char1, char char2, char char3, char char4) {
  alpha4.writeDigitAscii(0, char1);
  alpha4.writeDigitAscii(1, char2);
  alpha4.writeDigitAscii(2, char3);
  alpha4.writeDigitAscii(3, char4);
  alpha4.writeDisplay();
}


void setup() {
  alpha4.begin(0x70);          //start communication between arduino and LED backpack
  showChars(' ',' ',' ',' ');  //clear display characters
  if (time_to_wash < 20) {
    time_to_wash = 20;
  }
}

//body of program
void loop() {
  if (hold_mode) {
    if (break_beam == digitalRead(ir_input)) {
      timeout_count = 0;
      while (break_beam == digitalRead(ir_input)) {
        if (wash_time == time_to_wash + 1) {
          break;
        }
        showChars(' ',' ',countTens,countOnes);
        countOnes += 1;
        if (countOnes > '9') {
          countTens += 1;
          countOnes = '0';
        }
        wash_time++;
        delay(1000);
      }
    }
      if (wash_time == time_to_wash + 1) {
        showChars('D','O','N','E');
        delay(1000);
        wash_time = 0;
        timeout_count = 0;
        countTens = '0';
        countOnes = '0';
        showChars(' ',' ',' ',' ');
        delay(3000);
      } else if (timeout_count >= 5) {      //Once timeout counter gets to five, show TIME OUT reset counters and clear display
        showChars('T','I','M','E');
        delay(500);
        showChars('0','U','T',' ');
        delay(500);
        wash_time = 0;
        timeout_count = 0;
        countTens = '0';
        countOnes = '0';
        showChars(' ',' ',' ',' ');
      } else if ((wash_time > 0) && (timeout_count < 5)) {   //If timeout is less than 5 then display WASH MORE
        showChars('W','A','S','H');
        delay(500);
        showChars('M','O','R','E');
        delay(500);
        timeout_count++;
    } else {
      timeout_count = 0;
    }
  } else {
    if (break_beam == digitalRead(ir_input) or no_soap_reset) {
      if (!no_soap_reset){
        showChars('S','O','A','P');
        delay(soap_hold_time);
        showChars('W','A','S','H');
        delay(text_hold_time);
      }
      no_soap_reset = false;
      wash_time = 0;
      countOnes = '0';
      countTens = '0';
      for (wash_time = 0; wash_time < time_to_wash + 2; wash_time++) {
        if (break_beam == digitalRead(ir_input) || wash_time == time_to_wash + 1) {
          break;
        }
        showChars(' ',' ',countTens,countOnes);
        delay(1000);
        countOnes++;
        if (countOnes > '9') {
          countTens++;
          countOnes = '0';
        }
      }
      if (wash_time >= time_to_wash) {
        showChars('D','O','N','E');
        delay(text_hold_time);
        showChars(' ',' ',' ',' ');
      }
      while (break_beam == digitalRead(ir_input)) {
        showChars('R','E',' ',' ');
        delay(text_hold_time);
        showChars('S','E','T',' ');
        delay(text_hold_time);
        showChars(' ',' ',' ',' ');
        wash_time = 0;
        no_soap_reset = true;
      }
    }
  }
}
