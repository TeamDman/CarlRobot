#define LINEPIN2 2
#define LINEPIN1 3
#define MOTORPIN1B A0 //motor 1 pin B
#define MOTORPIN1A A1 //motor 1 pin A
#define MOTORPIN2A A2 //motor 2 pin A
#define MOTORPIN2B A3 //motor 2 pin B
#define TRIGPIN 6 //ultrasonic trigger pin
#define ECHOPIN 7 //ultrasonic echo pin
#define RECHOPIN 10
#define RTRIGPIN 9

#define TURNLEFT motor_SetOutputs(255,0,0,255);
#define TURNRIGHT motor_SetOutputs(0,255,255,0);
#define FORWARD motor_SetOutputs(0,255,0,255);
#define GETULTRA getUltrasonic(&distance, &distside)
//1 second +/-255 both wheels = 180 spin

unsigned long int loopIter=0;

void setup() {
	Serial.begin(57600); 
	Serial.println();
	pinMode(LINEPIN1, INPUT);
	pinMode(LINEPIN2, INPUT);
	pinMode(ECHOPIN, INPUT); 
	pinMode(RECHOPIN, INPUT); 
	pinMode(TRIGPIN, OUTPUT);
	pinMode(RTRIGPIN, OUTPUT);
}


int pleft=0;
int pright=1;
void loop() {
	
	checkLines();
	checkUltrasonic();
}

bool getUltrasonic(int* dir, int* dirleft) {
	// assert(dir);
	// assert(dirleft);

	digitalWrite(TRIGPIN, LOW);
	delayMicroseconds(2);
	digitalWrite(TRIGPIN, HIGH);
	delayMicroseconds(10);
	digitalWrite(TRIGPIN, LOW);
	*dir = (int)pulseIn(ECHOPIN, HIGH) / 34 / 2; //using the speed of sound, we find distance from object infront of us. look up "basic kinematics : finding distance using velocity and time"
	digitalWrite(RTRIGPIN, LOW);
	delayMicroseconds(2);
	digitalWrite(RTRIGPIN, HIGH);
	delayMicroseconds(10);
	digitalWrite(RTRIGPIN, LOW);
	*dirleft = (int)pulseIn(RECHOPIN, HIGH) / 34 / 2; //duration is now a value in milliseconds of how long it took for a sound wave to exit the sensor, and bounce off of an object back into the sensor
	return true;
}

void checkUltrasonic() { //check distance infront of ultrasonic sensor
	int distance, distside;
	GETULTRA;
	Serial.print("DIST ");
	Serial.print(distance);
	Serial.print(" ");
	Serial.println(distside);
	if (distance < 10 && distance > 0) {
		do {
			TURNLEFT
			delay(100);
		} while (GETULTRA && (distside < 1 || distside > 19 ));
		do {
			FORWARD
			delay(100);
		} while (GETULTRA && distside < 20 && distside > 0);
		delay(300);
		do {
			// TURNRIGHT
			motor_SetOutputs(0,255,0,0);
			delay(100);
		} while (GETULTRA && (distside < 1 || distside > 19 ));
		do {
			FORWARD
			delay(100);
		} while (GETULTRA && distside < 20 && distside > 0);
		do {
			motor_SetOutputs(0,255,0,0);
			delay(100);
		} while (GETULTRA && (distside < 1 || distside > 19 ));
		do {
			FORWARD
			delay(50);
		} while (!(digitalRead(LINEPIN1) || digitalRead(LINEPIN2)));
		motor_SetOutputs(0,0,0,0);
		delay(2000);
		pright=255;
		pleft=0;
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

