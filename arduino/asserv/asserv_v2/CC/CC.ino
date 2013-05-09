/****************************************
 * Author : Quentin C			*
 * Mail : quentin.chateau@gmail.com	*
 * Date : 31/03/13			*
 ****************************************/
  
#include "compaArduino.h"
#include "parameters.h"
#include <math.h>
#include "protocole.h"
#include "command.h"
#include "message.h"
#include "control.h"

unsigned long index = 0;
unsigned long timeStart = 0;
extern Control control;

void setup(){
	initPins();
	initSerialLink();
	// LED qui n'en est pas une
	pinMode(16,OUTPUT);
}

void loop(){
	int right_error, left_error;
	/* on note le temps de debut */
	timeStart = micros();

	/* La del est allumee pendant le traitement */
	digitalWrite(16, HIGH);

	/* zone programmation libre */
	
	/*lecture des ordres*/
	readIncomingData(cmd);

	//Action asserv
	control.compute();

	//---- DEBUG TICKS ----
	right_error = control.getEncRError();
	left_error = control.getEncLError();
	if(left_error != 0)
		Serial.println("-- left ticks error : " + left_error);
	if(right_error != 0)
		Serial.println("-- right ticks error : " + right_error);
	//---- FIN DEBUG TICKS ----

	/* fin zone de programmation libre */
	
	/* On eteint la del */
	digitalWrite(16, LOW);
	
	/* On attend le temps qu'il faut pour boucler */
	long udelay = DUREE_CYCLE*1000-(micros()-timeStart);
	if(udelay<0)
		Serial.println("ouch : mainloop trop longue");
	else
		 delayMicroseconds(udelay);
}


