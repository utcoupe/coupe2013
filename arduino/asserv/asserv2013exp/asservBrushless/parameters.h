/*
 * parameters.h
 *
 *  Created on: 16 janv. 2011
 *      Author: HoHen
 */

#ifndef PARAMETERS_H_
#define PARAMETERS_H_

#include "math.h"

/* Parametres mecanique du robot */
#define DIST_MOTOR_AXIS_TO_BACK_MM 122.0 // Distance entre le l'axe des moteurs et l'arriere du robot


#define ASSERV_VITESSE 1
#define MAX_SPEED 18

const int ENC_RESOLUTION = 500; // Nombre de points par tour d'encodeur
const int ENC_RADIUS = 34; // Rayon de la roue codeuse

const double ENC_TICKS_TO_MM = (2.0*M_PI*((double)ENC_RADIUS))/(((double)ENC_RESOLUTION)*4.0); // en mm.tick^-1 (opti: a calculer apres) environ : 0.0565
const double ENC_MM_TO_TICKS = (((double)ENC_RESOLUTION)*4.0)/(2.0*M_PI*((double)ENC_RADIUS)); // environ : 17.6839

const double ENC_CENTER_DIST_MM = 262.0; // Distance entre chaque roue codeuse
const double ENC_CENTER_DIST_TICKS = ENC_CENTER_DIST_MM*ENC_MM_TO_TICKS; // Distance entre chaque roue codeuse du robot en ticks

// Duree d'un cycle (en millisecondes)

#define DUREE_CYCLE 2

/* Parametre de la table en mm*/
#define TABLE_WIDTH_MM 2100.0
#define TABLE_HEIGHT_MM 3000.0
#define TABLE_DISTANCE_MAX_MM 3661.96

/*Parametre de l'asservissement*/
#define KP_SPEED 2.0
#define KI_SPEED 5.0
#define KD_SPEED 1.0
#define KP_ANGLE 14.0
#define KI_ANGLE 1.5
#define KD_ANGLE 20000.0
#define KP_DELTA 14.0
#define KI_DELTA 1.5
#define KD_DELTA 5000.0
#define KP_ALPHA 5.0
#define KI_ALPHA 1.5
#define KD_ALPHA 5000.0

/*Parametre de la file des taches a effectuer*/
#define SIZE 15

#endif /* PARAMETERS_H_ */
