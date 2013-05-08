/*
 * encoder.h
 *
 *  Created on: 13 janv. 2011
 *	  Author: HoHen
 */

#ifndef ENCODER_H_
#define ENCODER_H_

#include "parameters.h"

const int ENC_RESOLUTION = 500; // Nombre de points par tour d'encodeur
const int ENC_RADIUS = 34; // Rayon de la roue codeuse

const double ENC_TICKS_TO_MM = (2.0*M_PI*((double)ENC_RADIUS))/(((double)ENC_RESOLUTION)*4.0); // en mm.tick^-1 (opti: a calculer apres) environ : 0.0565
const double ENC_MM_TO_TICKS = (((double)ENC_RESOLUTION)*4.0)/(2.0*M_PI*((double)ENC_RADIUS)); // environ : 17.6839

const double ENC_CENTER_DIST_MM = 256.0; // Distance entre chaque roue codeuse
const double ENC_CENTER_DIST_TICKS = ENC_CENTER_DIST_MM*ENC_MM_TO_TICKS; // Distance entre chaque roue codeuse du robot en ticks


#define PIN_LEFT_A 20//19			// A encodeur 1 (gauche) (bleu)
#define PIN_LEFT_B 21//18			// B encodeur 1 (gauche) (blanc)
#define PIN_RIGHT_A 19//21			// A encodeur 2 (droite) (bleu)
#define PIN_RIGHT_B 18//20			// B encodeur 2 (droite) (blanc)
#define INTERRUPT_LEFT_A 3//4
#define INTERRUPT_LEFT_B 2//5
#define INTERRUPT_RIGHT_A 4//2
#define INTERRUPT_RIGHT_B 5//3


void initEncoders();

/*The most common type of incremental encoder uses two output channels (A and B) to sense position.
 *  the two output channels of the quadrature encoder indicate both position and direction of rotation.
 *  If A leads B, for example, the disk is rotating in a clockwise direction.
 *  If B leads A, then the disk is rotating in a counter-clockwise direction.
 *  */

extern long value_left_enc;
extern long value_right_enc;

#endif /* ENCODER_H_ */
