

#define IRPIN2 2 //TCRT pin
#define IRPIN1 3 //TCRT pin
#define MPIN1B A0 //motor 1 pin B
#define MPIN1A A1 //motor 1 pin A
#define MPIN2A A2 //motor 2 pin A
#define MPIN2B A3 //motor 2 pin B


//variables
int rSpeed = 255; //robot&#39;s max speed (used in Move() calculations)
int TS1 = 80; //turn speed 1: the speed at which the dominating motor turns at
int TS2 = -75; //turn speed 2: the speed at which the turning motor turns at
int FS = 90; //forward speed: the speed at which both motors turn at


void setup() {
	Serial.begin(115200); //serial for serial port
	pinMode(IRPIN1, INPUT);
	pinMode(IRPIN2, INPUT);
}


void checkLineSensors() {//this function reads TCRTs for 1 and 0 (0 = line, 1 = white) and turns
	motors accordingly
	// read pins
	int ir1 = digitalRead(IRPIN1);
	int ir2 = digitalRead(IRPIN2);
	//Serial.print(ir1);
	//Serial.print(&quot;left sensor & right sensor&quot;);
	//Serial.println(ir2); //test print
	//follow the line
	if (ir1 == 1 & ir2 == 0)
	{
		Move(TS1, TS2); //if you don&#39;t know what these mean, pls read whole code up there
	}
	else if (ir1 == 0 && ir2 == 1)
	{
		Move(TS2, TS1);
	}
	else if (ir1 == 0 && ir2 == 0)
	{
		Move(FS, FS);
	}
	else if (ir1 == 1 && ir2 == 1)
	{
		Move(0, 0);
	}
}

void loop() {
	checkLineSensors();
}	

void Move(int m1, int m2) { //m1 and m2 are ints from 0-100, which is % motors will run at.
	if (m1 < 0) { //only one pin of the motors needs to get an analogWrite
		analogWrite(MPIN1A, rSpeed * abs(m1) / 100);
		analogWrite(MPIN1B, 0);
	}
	else
	{
		analogWrite(MPIN1A, 0);
		analogWrite(MPIN1B, rSpeed * m1 / 100);
	}
	if (m2 < 0) {
		analogWrite(MPIN2A, rSpeed * abs(m2) / 100);
		analogWrite(MPIN2B, 0);
	}
	else
	{
		analogWrite(MPIN2A, 0);
		analogWrite(MPIN2B, rSpeed * m2 / 100);
	}
}