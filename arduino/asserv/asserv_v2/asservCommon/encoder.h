/****************************************
 * Author : Quentin C			*
 * Mail : quentin.chateau@gmail.com	*
 * Date : 01/04/13			*
 ****************************************/
#ifndef ENCODER_H
#define ENCODER_H

#include "parameters.h"
#include "include_arduino.h"
#include "compaArduino.h"

#define LEFT_SIDE 0
#define RIGHT_SIDE 1

class Encoder{
	public:
	long getTicks();
	void reset();
	void interruptA();
	#ifdef USE_4X
	void interruptB();
	#endif
	void interrupt0();//renvoie le nbr de ticks depuis le dernier interrupt 0 et 0 si c'est le premier tour
	int getError();
	Encoder(int p_side);

	private:
	long ticks;
	long last_ticks_on_0;
	bool last_value_A;
	bool last_value_B;
	bool signal_0_init;
	int side; //prend les valeur LEFT_SIDE ou RIGHT_SIDE
	int ticks_error;
};

extern Encoder encoderLeft, encoderRight;

#endif
