#include <wiringPi.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <signal.h>

#define StartPos 0x7F
#define LSB 0x01

const int WR_pin = 12;
const int WE_pin = 13;
const int data[8] = {5, 6, 22, 27, 23, 24, 16, 26};

void setSpeed(int8_t speed){
    digitalWrite(WR_pin, HIGH);         //put BASYS in high impedance
    delayMicroseconds(1);                 //Allow Setting to Propagate

    for (int pinNum=0;pinNum<8;pinNum++){
        pinMode(data[pinNum], OUTPUT);                          //Place All Data pins in Output Mode
        digitalWrite(data[pinNum], (speed >> pinNum) & LSB);    //Write Bit to each pin
    }
    digitalWrite(WE_pin, HIGH);
    delayMicroseconds(1);
    digitalWrite(WE_pin, LOW);
    for (int i=0;i<8;i++)
        pinMode(data[i], INPUT);
    digitalWrite(WR_pin, LOW);
}

void safeStop(int a){
    setSpeed(0);
    exit(0);
}

int getPos(){
    int pos = 0;
    for(int i=0; i<8; i++){
        pos+=digitalRead(data[i])<<i;
    }
    return pos;
}

int enter_velocity(){
    int input_velocity; 
    
    printf("Enter a velocity: ");
    scanf("%d", &input_velocity);
//    printf("given velocity: %d", input_velocity);
    return input_velocity;
    
}


int main (void){
    int velocity;

    signal(SIGINT, safeStop); //Handle signals to turn off motors when interrupted

    if (wiringPiSetupGpio() == -1) exit (1) ; //exit in case of io error

    for (int i=0;i<8;i++) pinMode(data[i], INPUT); //begin with dataPins in input Mode

    pinMode (WR_pin, OUTPUT);   //Initialize Write/Read Pin as Low (Basys Writing, Pi Reading)
    digitalWrite(WR_pin, LOW);

    pinMode (WE_pin, OUTPUT);   //Initialize Write Enable Pin as Low
    digitalWrite(WE_pin, LOW);

    int8_t commandPos = StartPos;

    delay(100);
    velocity = enter_velocity();
    while(1){
    for(int j =0; j<4;j++){
        //pwm = (workingKP * positionError - workingKV * velocity) >> SHIFT_KPKV;
		
        int8_t delta = (5*commandPos- 10*getPos()) >> 8;
        //int speed = delta<<1;
        //delta *= 4;
        setSpeed(delta);
        delay(1);
    }
    commandPos += velocity;
    }
}
