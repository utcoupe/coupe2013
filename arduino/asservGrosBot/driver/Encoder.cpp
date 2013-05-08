
#include "Encoder.h"

// Yes, all the code is in the header file, to provide the user
// configure options with #define (before they include it), and
// to facilitate some crafty optimizations!

Encoder_internal_state_t * Encoder::interruptArgs[];

/*
 * encoder.cpp
 *
 *  Created on: 13 janv. 2011
 *	  Author: HoHen
 */
#include "encoder.h"
#include "include_arduino.h"

long value_left_enc;
long value_right_enc;

Encoder knobLeft(PIN_LEFT_A, PIN_LEFT_B);
Encoder knobRight(PIN_RIGHT_A, PIN_RIGHT_B);

void initEncoders(){
	value_left_enc = 0;
	value_right_enc = 0;

	Serial.begin(9600);
}

void loopEncoders(){
	value_left_enc = knobLeft.read();
	value_right_enc = knobRight.read();
}


