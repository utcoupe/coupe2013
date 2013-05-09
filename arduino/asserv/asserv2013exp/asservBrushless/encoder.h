/*
 * encoder.h
 *
 *  Created on: 13 janv. 2011
 *	  Author: HoHen
 */

#ifndef ENCODER_H_
#define ENCODER_H_

#include "parameters.h"

#define PIN_LEFT_A 19
#define PIN_LEFT_B 20
#define PIN_LEFT_0 21
#define PIN_RIGHT_A 3
#define PIN_RIGHT_B 2
#define PIN_RIGHT_0 18

#define INTERRUPT_LEFT_A 4
#define INTERRUPT_LEFT_B 3
#define INTERRUPT_LEFT_0 2
#define INTERRUPT_RIGHT_A 1
#define INTERRUPT_RIGHT_B 0
#define INTERRUPT_RIGHT_0 5

#define VALUE_PIN_LEFT_A 		bitRead(PIND,3)
#define VALUE_PIN_LEFT_B 		bitRead(PIND,2)
#define VALUE_PIN_RIGHT_B 		bitRead(PIND,1)
#define VALUE_PIN_RIGHT_A 		bitRead(PIND,0)


void initEncoders();

/*The most common type of incremental encoder uses two output channels (A and B) to sense position.
 *  the two output channels of the quadrature encoder indicate both position and direction of rotation.
 *  If A leads B, for example, the disk is rotating in a clockwise direction.
 *  If B leads A, then the disk is rotating in a counter-clockwise direction.
 *  */

void valueChangeOnEncoderLeftPinA();
void valueChangeOnEncoderLeftPinB();
void valueChangeOnEncoderRightPinA();
void valueChangeOnEncoderRightPinB();

extern long value_left_enc;
extern long value_right_enc;


#endif /* ENCODER_H_ */
