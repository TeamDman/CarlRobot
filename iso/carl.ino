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

/*
187
296
345


EEPROM
*/

//Tone values above 13900 sometimes generate low tones
//black closer to tires

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <ArduinoOTA.h>
#include <Wire.h>
#include <sx1509_library.h>
#include <Adafruit_NeoPixel.h>

//Not sure if these are needed, but the LED isn't working so might as well
#ifdef __AVR__
#include <avr/power.h>
#endif

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(1, 5, NEO_GRB + NEO_KHZ800);


sx1509Class sx1509(0x3E); // SX1509 I2C address
const int motor_APinA = 0;
const int motor_APinB = 1;
const int motor_BPinA = 2;
const int motor_BPinB = 3;
const int tone_Pin=13;
// NOTE: Motor max output 255

const char* esp_SSID = "CarlRobot";
const char* esp_PASS = "CarlRobot";
const bool 	esp_HOTSPOT = true; // Enable to connect to an existing network, see below
// You can have the ESP connect to your wifi network by replacing the OCSB and wireless4all found in the setup

int robot_Mode=0; //1 = linefollowing, 0 = joystick
unsigned long int loopIter=0;
ESP8266WebServer server(80); // Start webserver

void setup() {
	Serial.begin(57600); // Begin serial, higher baud rate is less stable but faster
	digitalWrite(7, HIGH); // Set HIGH output to ESP blue light
	pixels.begin();
	setLEDColour(255,0,0);
	Serial.println();

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

	if (WiFi.status() == WL_NO_SHIELD)
		while(true)
			Serial.println("WiFi shield not present");


	if (esp_HOTSPOT) {
		Serial.print("Wireless Access Point SSID: ");
		Serial.println(esp_SSID);
		WiFi.disconnect();
		WiFi.mode(WIFI_AP);
		WiFi.softAP(esp_SSID,esp_PASS);
	} else {
		esp_SSID = "OCSB";
		esp_PASS = "wireless4all";
		WiFi.mode(WIFI_STA);
		WiFi.begin(esp_SSID,esp_PASS);
		while (WiFi.status() != WL_CONNECTED) {
			Serial.print("Attempting connection to: ");
			Serial.println(esp_SSID);
			delay(1000);
		}
		while (WiFi.waitForConnectResult() != WL_CONNECTED) {
			Serial.println("Connection Failed! Rebooting...");
			delay(5000);
			ESP.restart();
		}
	}
	// if (true) { // I don't want these variables floating around .-.
	// 	IPAddress ip(192,168,0,124);
	// 	IPAddress gateway(192,168,0,1);
	// 	IPAddress subnet(255,255,255,0);
	// 	WiFi.config(ip,gateway,subnet);
	// }

	server.on("/",server_HandleRoot);
	server.onNotFound(server_HandleNotFound);	
	server.begin();

	Serial.print("Webserver active");
	Serial.print(" | ");
	Serial.println(WiFi.SSID());

	Serial.print("signal strength (RSSI):");
	Serial.println(WiFi.RSSI());
  
	Serial.println("IP Addresses: ");
	Serial.print("\tLOCAL\t");
	Serial.println(WiFi.localIP());
	Serial.print("\tSOFTAP\t");
	Serial.println(WiFi.softAPIP());

	ArduinoOTA.begin();

	tone(13, 500,250);
	delay(250);
	noTone(13);
	tone(13, 500,250);
	delay(250);
	noTone(13);
	setLEDColour(0,255,0);
	delay(2000);
	setLEDColour(0,0,255);
	// digitalWrite(7, LOW); // Disable the ESP blue light, finished setup
}


int pleft=0;
int pright=0;
void loop() {
	loopIter++;
	server.handleClient();
	ArduinoOTA.handle();
	if (robot_Mode==1) {
		setLEDColour(loopIter%10==0?255:0,loopIter%10==5?255:0,0);
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
}

void motor_SetOutputs(int apina, int apinb, int bpina, int bpinb) { 
 	sx1509.pwm(motor_APinA, apina);
	sx1509.pwm(motor_APinB, apinb);
	sx1509.pwm(motor_BPinA, bpina);
	sx1509.pwm(motor_BPinB, bpinb);
}

void setLEDColour(int r, int g, int b) {
	for (int i = 0; i < 1; i++) { // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
    	pixels.setPixelColor(i, pixels.Color(r, g, b));
    	pixels.show(); // sends updated pixel to hardware
  	}
}

void server_SendGood() {
	server.send (200, "text/html", "recieved");
}

int getArg(String arg) {
	return atoi(server.arg(arg).c_str());
}

void server_HandleRoot(){
	Serial.println("Handling Root\n");
	int apina=getArg("apina");
  	int apinb=getArg("apinb");
	int bpina=getArg("bpina");
  	int bpinb=getArg("bpinb");
  	int r=getArg("r");
  	int g=getArg("g");
  	int b=getArg("b");
  	int freq=getArg("freq");
  	int duration=getArg("dur");
  	int restart=getArg("restart");
  	robot_Mode=getArg("mode");
  	if (freq==0) {
  		noTone(tone_Pin);
  	} else if (duration!=0) {
  		tone(tone_Pin,freq,duration);
  		delay(duration);
  		noTone(tone_Pin);
  	} else {	
	  	tone(tone_Pin,freq,1000000);
  	}
 

	motor_SetOutputs(apina,apinb,bpina,bpinb);
	setLEDColour(r,g,b);
	
	String response = "<h1>Update 9-16</h1><h2>Arguments:</h2><p>";
	for (uint8_t i=0; i<server.args(); i++)
		response+="\t"+server.argName(i)+":\t"+server.arg(i)+"\n";
	Serial.println(response);
	response+="</p>";
	server.send(200,"text/html",response);
	if (restart==1) {
		tone(tone_Pin,0,500); // Crash esp for restart
	}
}

void server_HandleNotFound() {
	String message = "File Not Found\n\n";
	message += "URI: ";
	message += server.uri();
	message += "\nMethod: ";
	message += (server.method() == HTTP_GET)?"GET":"POST";
	message += "\nArguments: ";
	message += server.args();
	message += "\n";
	for (uint8_t i=0; i<server.args(); i++){
		message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
	}
	server.send(404, "text/plain", message);
}

