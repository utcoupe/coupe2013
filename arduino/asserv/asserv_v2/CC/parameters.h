/****************************************
 * Author : Quentin C			*
 * Mail : quentin.chateau@gmail.com	*
 * Date : 08/04/13			*
 ****************************************/
#ifndef PARAMETERS_H
#define PARAMETERS_H

/* Simple ou Double ou Quadruple evaluation ? 
 * La quadruple evaluation utilise 4 interruption par tick
 * (une pour chaque changement de valeur des fils A et B)
 *
 * La double evaluation utilise 2 interruptions par tick
 * (une pour chaque changement de valeur du fil A
 *
 * La simple evaluation utilise 1 interruption par tick
 * (front montant du fil A)
 *
 * Ces trois méthodes sont equivalentes
 * La quadruple evaluation est plus précise mais utilise
 * plus de puissance processeur
 * Il convient de tester la plus adapté selon le processeur
 * et le nombre de ticks du codeur 
 * 
 * ATTENTION : Utiliser un seul des defines suivants */

#define USE_4X //Quadruple evaluation
//#define USE_2X  //Double eval
//#define USE_1X //Simple eval

#define GESTION_3EME_FIL false

#define MAX_GOALS 15 //nombre max de goals dans la file (evite surcharge mémoire)
#define DUREE_CYCLE 2

/* Vitesse Maximale
 * Définit la vitesse maximale à laquelle peuvent aller les moteurs
 * Utile pour asservir la vitesse
 * Il suffit de commander les moteurs à pwm maximale et de faire un
 * control.getMotorSpd()
 * Cette vitesse est en mm/ms = m/s  */

#define SPD_MAX 1.0

/* PWM MINIMALE - NORMALEMENT PAS NECESSAIRE
 * "shift" de la pwm sur l'asservissement CC
 * cela sert à remédier au problème lié au fait 
 * qu'en dessous d'une certaine tension, les moteurs
 * CC ne tournent pas
 * 
 * Process de réglage :
 * envoyer des consignes en pwm au robot
 * partant de 0 et en augmentant progressivement
 * dès que le robot avance, la pwm min est trouvée */
#define PWM_MIN 0 

#define ENC_RESOLUTION 1024 //resolution du codeur

#ifdef USE_4X
	#define TICKS_PER_TURN (ENC_RESOLUTION * 4)
#endif
#ifdef USE_2X
	#define TICKS_PER_TURN (ENC_RESOLTION * 2)
#endif
#ifdef USE_1X
	#define TICKS_PER_TURN ENC_RESOLUTION
#endif

#define ENC_RADIUS 34 //rayon de la roue codeuse
#define ENTRAXE_ENC 262.0 // Distance entre chaque roue codeuse en mm
#define ENTRAXE_MOTOR 262.0 // Distance entre chaque roue motrice en mm

#define ERROR_ANGLE 0.05 //erreur en angle(radians) maximale pour considérer l'objectif comme atteint
#define ERROR_POS 10 // erreur en position (mm)  maximale pour considérer l'objectif comme atteint

#define MAX_ANGLE 0.2 // angle en dessous duquel on décrit une trajectoire curviligne (trop bas, le robot s'arretera constamment pour se recaler au lieu d'avancer, trop haut, il y aura overflow en vitesse et les trajectoires seront très courbes.

#define ENABLE_ACC_CONTROL true //active ou désactive l'asserv en accélération

//Intégrales et dérivée sont calculée avec un intervalle de temps en SECONDES

//P est le fateur qui transforme l'accélération voulue en pwm
//I est important car l'erreur statique est très élevée
//D évite le dépassement
#define ACC_P 150
#define ACC_I 20
#define ACC_D 0

//P permet de réguler l'accélération du robot : P augmente, le robot accèlère plus vite
//I devrait etre à 0 ou très bas :l'erreur statique ne sera pas grande
//D permet d'éviter le dépassement de la vitesse voulue mais peut créer de l'instabilité

#define SPD_P 0.5 //acc = P * E_spd(m/s)
#define SPD_I 0 //acc = I * I_spd(m/s * s)
#define SPD_D 0 

//Augmenter P revient à relentir plus tard, plus près de l'objectif
//I devrait être à 0 ou très bas : l'erreur statique ne sera pas grande
//D évite le dépassement mais peut créer de l'instabilité

#define ANG_P 0.75 //spd = P * E_ang(rad)
#define ANG_I 0 //spd = I * I_ang(rad * s)
#define ANG_D 0

#define DIS_P 0.002 //spd = P * E_dis(mm)
#define DIS_I 0 //spd = I * I_dis(mm * s)
#define DIS_D 0

//DEFINES ARDUINO
#define PIN_ENC_RIGHT_A 21
#define PIN_ENC_RIGHT_B 20
#define PIN_ENC_RIGHT_0 3
#define PIN_ENC_LEFT_A 18
#define PIN_ENC_LEFT_B 19
#define PIN_ENC_LEFT_0 2

#define INTERRUPT_ENC_LEFT_A 5
#define INTERRUPT_ENC_LEFT_B 4
#define INTERRUPT_ENC_LEFT_0 0
#define INTERRUPT_ENC_RIGHT_A 2
#define INTERRUPT_ENC_RIGHT_B 3
#define INTERRUPT_ENC_RIGHT_0 1

#endif
