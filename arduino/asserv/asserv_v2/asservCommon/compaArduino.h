/****************************************
 * Author : Quentin C			*
 * Mail : quentin.chateau@gmail.com	*
 * Date : 08/04/13			*
 ****************************************/
#ifndef COMPAARDUINO_H
#define COMPAARDUINO_H

#include "include_arduino.h"
#include "parameters.h"
#include "motor.h"
#include "encoder.h"

void interruptLeftA();
void interruptRightA();
#ifdef USE_4X
void interruptLeftB();
void interruptRightB();
#endif
#if GESTION_3EME_FIL
void interruptLeft0();
void interruptRight0();
#endif

void initPins();
unsigned long timeMillis();
unsigned long timeMicros();
#endif
