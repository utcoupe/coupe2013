//Par Quentin pour UTCoupe2013 17/12/2012
//Commande de shield arduino brushless by UTCoupe

#include <avr/io.h>
#include "include_arduino.h"

#include "brushlessMotor.h"


/******************************************
               MOTORS
******************************************/
inline void initPWM1(uint8_t freq) {
    pinMode(MOTOR1_SPD, OUTPUT);

    pinMode(MOTOR1_EN, OUTPUT);
    digitalWrite(MOTOR1_EN, HIGH); //Enable motor

    pinMode(MOTOR1_DIG1, OUTPUT);
    digitalWrite(MOTOR1_DIG1, LOW);
    pinMode(MOTOR1_DIG2, OUTPUT);
    digitalWrite(MOTOR1_DIG2, LOW); //config DIG1&2 to 0 

    pinMode(MOTOR1_RDY, INPUT);
}


inline void initPWM2(uint8_t freq) {
    pinMode(MOTOR2_SPD, OUTPUT);

    pinMode(MOTOR2_EN, OUTPUT);
    digitalWrite(MOTOR2_EN,OUTPUT); //enable motor
        
    pinMode(MOTOR1_DIG2, OUTPUT);
    digitalWrite(MOTOR2_DIG1, LOW);
    pinMode(MOTOR1_DIG2, OUTPUT);
    digitalWrite(MOTOR2_DIG2, LOW); //config DIG1&2 to 0

    pinMode(MOTOR2_RDY, INPUT);
}

/*********************************
DIG1 and DIG2 defined to 0 :
linear adjustment of the speed

might be configured to smth else in order use speed slavering
see datasheet of DEC-MODULE-24/2
***********************************/

Motor::Motor(uint8_t num, uint8_t freq) {
  motornum = num;
  pwmfreq = freq;

  switch (num) {
  case 1:
	pinMode(MOTOR1_DIR, OUTPUT);
	digitalWrite(MOTOR1_DIR, LOW);
    initPWM1(freq);
    break;
  case 2:
	pinMode(MOTOR2_DIR, OUTPUT);
	digitalWrite(MOTOR2_DIR, LOW);
    initPWM2(freq);
    break;
  }
}

void Motor::run(uint8_t cmd) {
  uint8_t dirPin;
  uint8_t enPin;
  switch (motornum) {
  case 1:
    dirPin = MOTOR1_DIR;
    enPin = MOTOR1_EN; break;
  case 2:
    dirPin = MOTOR2_DIR;
    enPin = MOTOR2_EN; break;
  default:
    return;
  }
  
  switch (cmd) {
  case FORWARD:
	digitalWrite(dirPin, LOW);
   	 break;
  case BACKWARD:
	digitalWrite(dirPin, HIGH);
   	 break;
  case RELEASE:
   	digitalWrite(enPin, LOW);//free wheel 
    	break;
  case BRAKE:
  	digitalWrite(enPin, HIGH);//disable free wheel
	break;
  }
}

void Motor::setSpeed(uint8_t speed) {
  switch (motornum) {
  case 1:
    analogWrite(MOTOR1_SPD, speed); break;
  case 2:
    analogWrite(MOTOR2_SPD, speed); break;
  }
}


