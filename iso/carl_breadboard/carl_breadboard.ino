#define LINEPIN2 2
#define LINEPIN1 3
#define MOTORPIN1B A0 //motor 1 pin B
#define MOTORPIN1A A1 //motor 1 pin A
#define MOTORPIN2A A2 //motor 2 pin A
#define MOTORPIN2B A3 //motor 2 pin B

unsigned long int loopIter=0;

void setup() {
	Serial.begin(57600); 
	Serial.println();
	pinMode(LINEPIN1, INPUT);
	pinMode(LINEPIN2, INPUT);
}


int pleft=0;
int pright=1;
void loop() {
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
	motor_SetOutputs(nleft,aleft,nright,aright );
	
}

void motor_SetOutputs(int apina, int apinb, int bpina, int bpinb) { 
 	analogWrite(MOTORPIN1A, apina);
	analogWrite(MOTORPIN1B, apinb);
	analogWrite(MOTORPIN2A, bpina);
	analogWrite(MOTORPIN2B, bpinb);
}

