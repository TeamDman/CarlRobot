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
*/
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
// NOTE: Motor max output 255

const char* esp_SSID = "Harambe";
const char* esp_PASS = "ripperoni";
const bool 	esp_HOTSPOT = false; // Enable to connect to an existing network, see below
// You can have the ESP connect to your wifi network by replacing the OCSB and wireless4all found in the setup


ESP8266WebServer server(80); // Start webserver

void setup() {
	// Serial.begin(115200); // Begin serial, higher baud rate is less stable but faster
	Serial.begin(57600);
	digitalWrite(7, HIGH); // Set HIGH output to ESP blue light
	pixels.begin();
	setLEDColour(0,100,0); // Set GREEN output to LED
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
	server.on("/index.html",server_HandleRoot);
	server.on("/move", robot_Move);
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

	// tone(13, 500, 250);
	// delay(250);
	// tone(13, 900, 250);
	// delay(250);
	// tone(13, 1200, 250);
	// delay(250);
	// tone(13,0,0);
	
	digitalWrite(7, LOW); // Disable the ESP blue light, finished setup
}

void loop() {
	server.handleClient();
	ArduinoOTA.handle();
}

void robot_Move() { 
	Serial.println("Robot: INPUT DATA");
	int apina=atoi(server.arg("ApinA").c_str());
  	int apinb=atoi(server.arg("ApinB").c_str());
	int bpina=atoi(server.arg("BpinA").c_str());
  	int bpinb=atoi(server.arg("BpinB").c_str());
  	Serial.println(apina);
  	Serial.println(apinb);
  	Serial.println(bpina);
  	Serial.println(bpinb);
  	Serial.println("\n");
	motor_SetOutputs(apina,apinb,bpina,bpinb);
	server_SendGood();
}

void motor_SetOutputs(int apina, int apinb, int bpina, int bpinb) { 
 	sx1509.pwm(motor_APinA, apina);
	sx1509.pwm(motor_APinB, apinb);
	sx1509.pwm(motor_BPinA, bpina);
	sx1509.pwm(motor_BPinB, bpinb);
}

void setLEDColour(int r, int g, int b) {
	for (int i = 0; i < 1; i++) { // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
    	pixels.setPixelColor(i, pixels.Color(r % 255, g % 255, b % 255));
    	pixels.show(); // sends updated pixel to hardware
  	}
}

void server_SendGood() {
	server.send (200, "text/html", "recieved");
}

void server_HandleRoot(){
	Serial.println("Handling Root");
	server.send(200,"text/html","<h1>Update 9-16</h1>");
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

