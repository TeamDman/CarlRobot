/*The MIT License (MIT)

	Copyright (C) 2015, Miguel Medeiros, Dominic Phillips

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in
	all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
	THE SOFTWARE.

	http://www.esp8266.com/viewtopic.php?f=32&t=3780
	https://github.com/sandeepmistry/esp8266-Arduino/blob/master/esp8266com/esp8266/libraries/ESP8266WebServer/src/ESP8266WebServer.cpp
*/


#include <Wire.h>
#include <sx1509_library.h>

#ifdef __AVR__
#include <avr/power.h>
#endif


sx1509Class sx1509(0x3E); // SX1509 I2C address
const int motor_APinA = 0;
const int motor_APinB = 1;
const int motor_BPinA = 2;
const int motor_BPinB = 3;

void setup() {
	Serial.begin(57600); // Begin serial, higher baud rate is less stable but faster


	// Legacy code, motor initializer
	sx1509.init();  // Initialize the SX1509, does Wire.begin()
    byte clockDivider = 1;
    sx1509.ledDriverInit(motor_APinA);
    sx1509.ledDriverInit(motor_APinA, clockDivider, LOGARITHMIC);
    sx1509.ledDriverInit(motor_APinB);
    sx1509.ledDriverInit(motor_APinB, clockDivider, LOGARITHMIC);
    sx1509.ledDriverInit(motor_BPinA);
    sx1509.ledDriverInit(motor_BPinA, clockDivider, LOGARITHMIC);
    sx1509.ledDriverInit(motor_BPinB);
    sx1509.ledDriverInit(motor_BPinB, clockDivider, LOGARITHMIC);

	// setup for QRE113 sensors
	sx1509.pinDir(7, OUTPUT); //servo
	sx1509.pinDir(8, OUTPUT); //ultrasonic trigger
	sx1509.pinDir(9, INPUT); //ultrasonic echo
	sx1509.pinDir(10, OUTPUT);  // Set SX1509 pin 10 as an output - enables all IR leds
	sx1509.pinDir(11, INPUT);
	sx1509.pinDir(12, INPUT);
	sx1509.pinDir(13, INPUT);
	sx1509.pinDir(14, INPUT);
	sx1509.pinDir(15, INPUT);

}


int pleft=0;
int pright=0;
void loop() {
	sx1509.writePin(10,HIGH);
	int right = sx1509.readPin(12);  // read line sensor 1 (s4)
	int left = sx1509.readPin(14);  // read line sensor 3 (s3)
	int nleft=0;
	int nright=0;
	int aleft=0;
	int aright=0;
	if (left==0&&right==0) {
		// aleft=255;
		// aright=255;
		aleft=pleft;
		aright=pright;
	} else if (left==1&&right==1) {
		aright=pright;
		pright=255;
	} else if (left==0) {
		aleft=255;
		pleft=aleft;
		pright=0;
		// nleft=180;
	} else if (right==0) {
		aright=255;
		pright=aright;
		pleft=0;
		// nright=180;
	}
	motor_SetOutputs(nleft,aleft,nright,aright );

}

void motor_SetOutputs(int apina, int apinb, int bpina, int bpinb) { 
 	sx1509.pwm(motor_APinA, apina);
	sx1509.pwm(motor_APinB, apinb);
	sx1509.pwm(motor_BPinA, bpina);
	sx1509.pwm(motor_BPinB, bpinb);
}