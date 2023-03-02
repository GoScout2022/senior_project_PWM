//gcc pwm.c -o pwm -lwiringPi

#include <wiringPi.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <signal.h>
#include <wiringPi.h>

#define StartPos 0x7F
#define LSB 0x01
#define KP 1
#define KV 1

#define DataPin1 5
#define DataPin2 6
#define DataPin3 22
#define DataPin4 27
#define DataPin5 23
#define DataPin6 24
#define DataPin7 16
#define DataPin8 26

const int WR_pin = 12;
const int WE_pin = 13;
const int data[8] = {5, 6, 22, 27, 23, 24, 16, 26};

void setSpeed(int8_t speed){
    digitalWrite(WR_pin, HIGH);         //put BASYS in high impedance
    delayMicroseconds(1);                 //Allow Setting to Propagate

    for (int pinNum=0;pinNum<8;pinNum++){
        pinMode(data[pinNum], OUTPUT);                          //Place All Data pins in Output Mode
        digitalWrite(data[pinNum], (speed >> pinNum) & LSB);    //Write Data to each pin
    }
    digitalWrite(WE_pin, HIGH);         //Enable Write
    delayMicroseconds(1);               //Allow Propagation
    digitalWrite(WE_pin, LOW);          //Disable Write

    pinMode(DataPin1, INPUT);
    pinMode(DataPin2, INPUT);
    pinMode(DataPin3, INPUT);
    pinMode(DataPin4, INPUT);

    pinMode(DataPin5, INPUT);
    pinMode(DataPin6, INPUT);
    pinMode(DataPin7, INPUT);
    pinMode(DataPin8, INPUT);


    //pinMode(data[pinNum], INPUT);

    for (int pinNum=0;pinNum<8;pinNum++)
        pinMode(data[pinNum], INPUT);   //Place all data pins in input mode
    digitalWrite(WR_pin, LOW);
}

void safeStop(int a){
    setSpeed(0);
    exit(0);
}

int8_t getPos(){
	int8_t pos = 0;
	for(int pinNum=0; pinNum<8; pinNum++){
		pos+=digitalRead(data[pinNum])<<pinNum;
	}
	return pos;
}


int main (void){

    signal(SIGINT, safeStop); //Handle signals to turn off motors when interrupted

    if (wiringPiSetupGpio() == -1) exit (1) ; //exit in case of io error

    for (int i=0;i<8;i++) pinMode(data[i], INPUT); //begin with dataPins in input Mode

    pinMode (WR_pin, OUTPUT);   //Initialize Write/Read Pin as Low (Basys Writing, Pi Reading)
    digitalWrite(WR_pin, LOW);

    pinMode (WE_pin, OUTPUT);   //Initialize Write Enable Pin as Low
    digitalWrite(WE_pin, LOW);

  int8_t commandPos = StartPos;
  int8_t actualPos = StartPos;
  delay(100);
  while(1){
    for(int j =0; j<4;j++){                     //For loop only used for temporary stability: remove after KV
        actualPos = getPos();
        int8_t delta = commandPos-actualPos;    //Compare current to target
        delta *= KP;                            //Multiply by KP TODO use shifting instead

        //TODO Velocity Compensation
        //delta-= KV * velocity
	setSpeed(0);
        //setSpeed(delta);                        //SetSpeed
        delay(1);                       //Delay 1 ms
    }
  commandPos += 10;                             //update Position
  }
}


