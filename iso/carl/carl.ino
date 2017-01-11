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
*/



/*
	NOTES
	• Tone values above 13900 can cause unexpected behaviour.
	• The black wire from the battery pack should be closer to the tires than the red one.
	• Motor methods accept integers between 0-255
*/


#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <ArduinoOTA.h>
#include <Wire.h>
#include <sx1509_library.h>
#include <Adafruit_NeoPixel.h>

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


/*
	Robot can use either a pre-existing wifi network by setting the SSID and PASS variables to the corresponding values and setting HOTSPOT to false
	However, this is not recommended as the personal hotspot for the robot provides a more reliable connection.
	Using existing networks might provide an easier developing experience as it allows connection via ethernet to the robot given the correct IP address which can be obtained from the arduino serial monitor upon boot
*/
const char* esp_SSID = "CarlRobot Theta";
const char* esp_PASS = "CarlRobot";
const bool 	esp_HOTSPOT = true; 

int robot_Mode=0; 				// 1 = linefollowing, 0 = joystick
unsigned long int loopIter=0;	// Used for flashing the LED when in linefollowing mode, not very important
ESP8266WebServer server(80); 

void setup() {
	Serial.begin(57600); 	// Begin serial, higher baud rate is less stable but faster
	digitalWrite(7, HIGH); 	// Set HIGH output to ESP blue light
	pixels.begin();			// Setup the neopixel LEDs
	setLEDColour(255,0,0);
	Serial.println();

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

	// Setup for linefollowing (QRE113) sensors
	sx1509.pinDir(7, OUTPUT); 	// servo
	sx1509.pinDir(8, OUTPUT); 	// ultrasonic trigger
	sx1509.pinDir(9, INPUT); 	// ultrasonic echo
	sx1509.pinDir(10, OUTPUT);  // set SX1509 pin 10 as an output - enables all IR leds
	sx1509.pinDir(11, INPUT);	// set the rest of the sensors to input
	sx1509.pinDir(12, INPUT);
	sx1509.pinDir(13, INPUT);
	sx1509.pinDir(14, INPUT);
	sx1509.pinDir(15, INPUT);

	if (WiFi.status() == WL_NO_SHIELD)
		while(true) // Require the wifishield to boot
			Serial.println("WiFi shield not present");


	if (esp_HOTSPOT) {	// Setup for personal hotspot
		Serial.print("Wireless Access Point SSID: ");
		Serial.println(esp_SSID);
		WiFi.disconnect();
		WiFi.mode(WIFI_AP);
		WiFi.softAP(esp_SSID,esp_PASS);
	} else {			// Setup for existing network
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

	// Setup the network event handlers
	server.on("/",server_HandleRoot);
	server.onNotFound(server_HandleNotFound);	
	server.begin(); // Start the listeners

	// Print debug info
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

	// OTA update handling
	ArduinoOTA.begin();

	// Boot noises and colours
	tone(13, 500,250);
	delay(250);
	noTone(13);
	tone(13, 500,250);
	delay(250);
	noTone(13);
	setLEDColour(0,255,0);
	delay(2000);
	setLEDColour(0,0,255);
}


// Variables used in linefollowing logic
int pleft=0;
int pright=1;

void loop() {
	loopIter++;				// Increase logic counter for LED flashing, millis() might be a better method for future use
	server.handleClient();	// Handle network events
	ArduinoOTA.handle();	// Handle OTA updates
	if (robot_Mode==1) { // Linefollowing logic
		setLEDColour(loopIter%10==0?255:0,loopIter%10==5?255:0,0); // Flash LED
		sx1509.writePin(10,HIGH);
		int right = sx1509.readPin(12);  // read line sensor 1 (s4)
		int left = sx1509.readPin(14);   // read line sensor 3 (s3)

		int nleft=0;
		int nright=0;
		int aleft=0;
		int aright=0;
		/*
			nleft = left motor reverse
			aleft = left motor forward
			nright= right motor reverse
			aright= right motor forward
		*/		
		/*
			Linefollowing logic pseudo-code

			Default action : stop
			if (both sensors on white)
				resume previous actions (ex, turn left)
			elseif (both sensors on black)
				resume right_previous
				set right_previous=forward
			elseif (left sensor on white)
				set left_motor,left_previous=forward
				set previous_right=stop
			elseif (right sensor on white)
				set right_motor,right_previous=forward
				set previous_left=stop

		*/
		if (left==0&&right==0) {
			aleft=pleft;
			aright=pright;
		} else if (left==1&&right==1) {
			aright=pright;
			pright=255;
		} else if (left==0) {
			aleft=255;
			pleft=aleft;
			pright=0;
		} else if (right==0) {
			aright=255;
			pright=aright;
			pleft=0;
		}
		motor_SetOutputs(nleft,aleft,nright,aright ); //Set motor to determined values
		//Print debug info on one line, otherwise hard to read on scrolling serial monitor
		Serial.print(pleft);
		Serial.print(" ");
		Serial.print(pright);
		Serial.print(" ");
		Serial.print(left);
		Serial.print(" ");
		Serial.print(right);
		Serial.print(" ");
		Serial.print(aleft);
		Serial.print(" ");
		Serial.print(aright);
		Serial.println(" ");
	}
}

void motor_SetOutputs(int apina, int apinb, int bpina, int bpinb) { 
	// Each motor has 2 parts, reverse output and forward output
	// PinA=reverse pin
	// PinB=forward pin
	sx1509.pwm(motor_APinA, apina);
	sx1509.pwm(motor_APinB, apinb);
	sx1509.pwm(motor_BPinA, bpina);
	sx1509.pwm(motor_BPinB, bpinb);
}

void setLEDColour(int r, int g, int b) {
	for (int i = 0; i < 1; i++) { 
		// pixels.Color takes RGB values, from 0,0,0 up to 255,255,255	
		pixels.setPixelColor(i, pixels.Color(r, g, b));
		pixels.show();  // sends updated color to hardware
	}
}

void server_SendGood() {
	server.send (200, "text/html", "recieved"); // Tell the client that their request went through
}

int getArg(String arg) {
	return atoi(server.arg(arg).c_str()); // Cast the string arguments to integer and return
}

void server_HandleRoot(){
	Serial.println("Handling Root\n");
	// Get arguments from request, default=0
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

	robot_Mode=getArg("mode"); // Set robot mode
	if (freq==0) { // Stop tone generation
		noTone(tone_Pin);
	} else if (duration!=0) { // Set tone to play for set time in milliseconds
		tone(tone_Pin,freq,duration);
		delay(duration);
		noTone(tone_Pin);
	} else { // Play tone 'forever'
		tone(tone_Pin,freq,1000000);
	}
 
	// Set motor outputs to received values
	motor_SetOutputs(apina,apinb,bpina,bpinb);
	// Set LED to received values
	setLEDColour(r,g,b);

	// Build response displaying given variables
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

void server_HandleNotFound() { // Build error message when a request 404's
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

