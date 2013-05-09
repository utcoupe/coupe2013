/****************************************
 * Author : Quentin C			*
 * Mail : quentin.chateau@gmail.com	*
 * Date : 31/03/13			*
 ****************************************/
#include "compaArduino.h"

void initPins(){
	//set mode des pins pour arduino
	pinMode(PIN_ENC_LEFT_A,INPUT);
	pinMode(PIN_ENC_LEFT_B,INPUT);
	pinMode(PIN_ENC_LEFT_0,INPUT);
	pinMode(PIN_ENC_RIGHT_0,INPUT);
	pinMode(PIN_ENC_RIGHT_A,INPUT);
	pinMode(PIN_ENC_RIGHT_B,INPUT);

	//Definition des interruptions arduino en fonction du type d'évaluation
#ifdef USE_4X
	attachInterrupt(INTERRUPT_ENC_LEFT_A,interruptLeftA,CHANGE);
	attachInterrupt(INTERRUPT_ENC_RIGHT_A,interruptRightA,CHANGE);
	attachInterrupt(INTERRUPT_ENC_LEFT_B,interruptLeftB,CHANGE);
	attachInterrupt(INTERRUPT_ENC_RIGHT_B,interruptRightB,CHANGE);
#endif

#ifdef USE_2X
	attachInterrupt(INTERRUPT_ENC_LEFT_A,interruptLeftA,CHANGE);
	attachInterrupt(INTERRUPT_ENC_RIGHT_A,interruptRightA,CHANGE);
#endif

#ifdef USE_1X
	attachInterrupt(INTERRUPT_ENC_LEFT_A,interruptLeftA,RISING);
	attachInterrupt(INTERRUPT_ENC_RIGHT_A,interruptRightA,RISING);
#endif

#if GESTION_3EME_FIL
	//Interruption du 3e fil des codeurs
	attachInterrupt(INTERRUPT_ENC_LEFT_0,interruptLeft0,RISING);
	attachInterrupt(INTERRUPT_ENC_RIGHT_0,interruptRight0,RISING);
#endif
}

unsigned long timeMillis(){
	return millis();
}
unsigned long timeMicros(){
	return micros();
}

//"hack" des interrupt arduino

void interruptLeftA(){
	encoderLeft.interruptA();
}

void interruptRightA(){
	encoderRight.interruptA();
}

#ifdef USE_4X
void interruptLeftB(){
	encoderLeft.interruptB();
}

void interruptRightB(){
	encoderRight.interruptB();
}
#endif

#if GESTION_3EME_FIL
void interruptLeft0{
	encoderLeft.interrupt0();
}

void interruptRight0{
	encoderRight.interrupt0();
}
#endif

