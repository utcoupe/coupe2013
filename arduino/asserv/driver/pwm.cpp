/*
 * pwm.cpp
 *
 *  Created on: 13 janv. 2011
 *      Author: HoHen
 */
#include "pwm.h"
#include "brushlessMotor.h"
#include "include_arduino.h"


Motor MoteurG(1, MOTOR12_64KHZ);
Motor MoteurD(2, MOTOR12_64KHZ);

void initPWM(){
	MoteurG = Motor(1, MOTOR12_64KHZ);
	MoteurD = Motor(2, MOTOR12_64KHZ);
	MoteurG.setSpeed(0);
	MoteurG.run(FORWARD);
	MoteurD.setSpeed(0);
	MoteurD.run(FORWARD);
}

void setLeftPWM(int value){
	MoteurG.setSpeed(abs(value));
	if(value<0)
		MoteurG.run(FORWARD);
	else
		MoteurG.run(BACKWARD);
}

void setRightPWM(int value){
	MoteurD.setSpeed(abs(value));
	if(value>0)
		MoteurD.run(FORWARD);
	else
		MoteurD.run(BACKWARD);
}


