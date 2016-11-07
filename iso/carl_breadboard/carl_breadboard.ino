#define LINEPIN2 2
#define LINEPIN1 3
#define MOTORPIN1B A0 //motor 1 pin B
#define MOTORPIN1A A1 //motor 1 pin A
#define MOTORPIN2A A2 //motor 2 pin A
#define MOTORPIN2B A3 //motor 2 pin B
#define TRIGPIN 6 //ultrasonic trigger pin
#define ECHOPIN 7 //ultrasonic echo pin

unsigned long int loopIter=0;

void setup() {
	Serial.begin(57600); 
	Serial.println();
	pinMode(LINEPIN1, INPUT);
	pinMode(LINEPIN2, INPUT);
	pinMode(ECHOPIN, INPUT); 
	pinMode(TRIGPIN, OUTPUT);
}


int pleft=0;
int pright=1;
void loop() {
	
	//checkLines();
	checkUltrasonic();
}

int getUltrasonic() {
	long duration, cm;
	digitalWrite(TRIGPIN, LOW);
	delayMicroseconds(2);
	digitalWrite(TRIGPIN, HIGH);
	delayMicroseconds(10);
	digitalWrite(TRIGPIN, LOW);
	duration = pulseIn(ECHOPIN, HIGH); //duration is now a value in milliseconds of how long it took for a sound wave to exit the sensor, and bounce off of an object back into the sensor
	cm = duration / 34 / 2; //using the speed of sound, we find distance from object infront of us. look up "basic kinematics : finding distance using velocity and time"
	int distance = (int)cm;
	return distance;
}

void checkUltrasonic() { //check distance infront of ultrasonic sensor
	int distance = getUltrasonic();
	Serial.print("DIST ");
	Serial.println(distance);
	//Distances over 16 are irrelevant
	if (distance<10) {
		Serial.print("odear");
		motor_SetOutputs(500,0,0,500);
		delay(750);
	} else {
		motor_SetOutputs(0,500,0,500);
	}
}

void checkLines() {
	int right = digitalRead(LINEPIN1);
	int left = digitalRead(LINEPIN2); 
	int nleft=0;
	int nright=0;
	int aleft=0;
	int aright=0;
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
	motor_SetOutputs(nleft,aleft,nright,aright );
	// Serial.print(pleft);
	// Serial.print(" ");
	// Serial.print(pright);
	// Serial.print(" ");
	// Serial.print(left);
	// Serial.print(" ");
	// Serial.print(right);
	// Serial.print(" ");
	// Serial.print(aleft);
	// Serial.print(" ");
	// Serial.print(aright);
	// Serial.println(" ");
	motor_SetOutputs(nleft,aleft,nright,aright);
}

void motor_SetOutputs(int apina, int apinb, int bpina, int bpinb) { 
	analogWrite(MOTORPIN1A, apina);
	analogWrite(MOTORPIN1B, apinb);
	analogWrite(MOTORPIN2A, bpina);
	analogWrite(MOTORPIN2B, bpinb);
}

