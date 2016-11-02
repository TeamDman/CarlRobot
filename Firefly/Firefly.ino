
//  NEW COMMANDS

//  HT1632.setPixelFF(x,y);  - remapped pixel coordinates, origin (0,0) at bottom left, x is horizontal, y is vertical - requires library update
//  HT1632.drawTextFF(disp, OUT_SIZE - i, 2, FONT_8X4, FONT_8X4_END, FONT_8X4_HEIGHT); - works same as prev version but is rotated and remapped

#include "pins.h"

#include <HT1632.h>
#include <font_8x4.h>
#include <images.h>

#include <HardWire.h>
HardWire HWire(1, I2C_FAST_MODE); // I2c1

uint8_t accData[64];

int i = 13; // start in middle
uint8_t wdImage;
uint8_t wdText;
uint8_t wd;
char disp [] = "HARAMBE WAS JUST A GORILLA";

int16_t scollDelay = 0;

enum machineStates { idle, buttonPhysicalInput, buttonLeftInput, buttonRightInput, busy, sleeping } machineState;
enum scrollModes { scrollStop, scrollLeft, scrollRight } scrollMode;
enum displayModes { displayImage, displayText, displaySparkle } displayMode;

void buttonPhysical(void) {
  machineState = buttonPhysicalInput;
}
void buttonLeft(void) {
  machineState = buttonLeftInput;
}
void buttonRight(void) {
  machineState = buttonRightInput;
}


void setup () {
  machineState = idle;

  Serial.begin(115200);
  HT1632.begin(CS, SCK, MOSI);

  HWire.begin();

  pinMode (pushButton, INPUT_PULLUP);
  pinMode (capButtonRight, INPUT_PULLUP);
  pinMode (capButtonLeft, INPUT_PULLUP);

  attachInterrupt(pushButton, buttonPhysical, FALLING);
  attachInterrupt(capButtonLeft, buttonLeft, FALLING);
  attachInterrupt(capButtonRight, buttonRight, FALLING);

  delay (2000);
  Serial.println("\nFFv2\n");

  wdText = HT1632.getTextWidth(disp, FONT_8X4_END, FONT_8X4_HEIGHT);
  Serial.println(wd); // 46 for text
  wd = wdImage = 9; // for heart


 // HWire.requestFrom (0x1C, 5);
}



void readi2c (void) {

  HWire.requestFrom (0x1C, 32);
  for (int i = 0; i < 32; i++) {
    accData[i] = HWire.read();
  }
  //HWire.endTransmission ();

  HWire.requestFrom (0x1C, 32);
  for (int i = 33; i < 64; i++) {
    accData[i] = HWire.read();
  }
  //HWire.endTransmission ();

  for (int i = 0; i < 63; i++) {
    Serial.print ("reg ");
    Serial.print(i);
    Serial.print (" data ");
    Serial.println(accData[i]);

  }
}
  


void loop () {





 


  if (machineState == buttonLeftInput) {
    Serial.println("\nbutton left\n");
    scollDelay += 50;
    if (scollDelay > 200) {
      scollDelay = 200;
    }
    Serial.println(scollDelay);
    machineState = idle;
  }

  if (machineState == buttonRightInput) {
    Serial.println("\nbutton right\n");
    scollDelay -= 50;
    if (scollDelay < -200) {
      scollDelay = -200;
    }
    machineState = idle;
    Serial.println(scollDelay);
  }




  if (machineState == buttonPhysicalInput) {
    Serial.println("\nbutton physical\n");
    
    if ( displayMode == displayImage ) {
      displayMode = displaySparkle ;
      wd = wdText;
    }
    else if ( displayMode == displayText ) {
      displayMode = displayImage ;
      wd = wdImage;
    }
    else if ( displayMode == displaySparkle  ) {
      displayMode = displayText ;
    }


    i = 13; // reset pointer to manage out of bounds on text -> image
    //scrollMode = scrollStop;
    //scollDelay = 0;
    machineState = idle;
  }


  if ( displayMode == displaySparkle ) {
    HT1632.setPixelFF (random(16), random(12));
    HT1632.clearPixelFF (random(16), random(12));
    HT1632.clearPixelFF (random(16), random(12));
    delay(5);
  }
  else if ( displayMode == displayImage ) {
    HT1632.clear();
    HT1632.drawImageFF(IMG_HEART, IMG_HEART_WIDTH,  IMG_HEART_HEIGHT, 16 - i, 2);
  }
  else if ( displayMode == displayText ) {
    HT1632.clear();
    HT1632.drawTextFF(disp, 16 - i, 2, FONT_8X4, FONT_8X4_END, FONT_8X4_HEIGHT);
  }
  HT1632.render(); // This updates the display on the screen.


  if ( scollDelay < 0 ) {
    scrollMode = scrollLeft;
  }
  else if ( scollDelay > 0 ) {
    scrollMode = scrollRight;
  }
  else if ( scollDelay == 0 ) {
    scrollMode = scrollStop;
  }


  if ( displayMode != displaySparkle) {
    delay(215 - abs(scollDelay)); // top speed is delay 15
  }
  if (scrollMode == scrollLeft) {
    if (i == 0) {
      i = wd + 16;
    }
    i--;
  }
  else if  (scrollMode == scrollRight) {
    if ( i == 16 + wd ) {
      i = 0;
    }
    i++; // = (i+1)%(wd + OUT_SIZE);
  }
  else {
    ;  // stop
  }



}
