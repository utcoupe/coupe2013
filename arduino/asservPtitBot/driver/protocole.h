#ifndef PROTOCOLE_H
#define PROTOCOLE_H

// cf. protocole
#define SEP			'+' // tout est séparé par ça

//
#define BLEU  	1
#define ROUGE 	2

////////////////////////////////////////////////////////////////////////
////						ASSERV
////////////////////////////////////////////////////////////////////////
/**
 * identification
 */
#define Q_ID 				0

/**
 * ping-pong
 */
#define Q_PING 			1

/**
 * @param x
 * @param y
 * @param vitesse
 */
#define Q_GOTO		 		2

/**
 * @param x
 * @param y
 * @param vitesse
 */
#define Q_GOTOR	 		3

/**
 * @param angle
 * @param vitesse
 */
#define Q_TURN		 		4

/**
 * Tourner en relatif
 * @param angle
 * @param vitesse
 */
#define Q_TURNR	 		5

/**
 * Position actuelle du robot
 */
#define Q_POS		 		6

/**
 * Définir la position actuelle
 * @param x mm
 * @param y mm
 * @param a degrès
 */
#define Q_SET_POS			7

/**
 * Mettre une pwm à droite et à gauche
 * @param pwm_left [-255,255]
 * @param pwm_right [-255,255]
 * @param period duree d'application de la pwm
 */
#define Q_PWM				8

/**
 * Stop
 */
#define Q_STOP				9

/**
 * stop et vide la fifo
 */
#define Q_CANCEL 			10


/**
 * Reprendre
 */
#define Q_RESUME			11

/**
 * Debug : sens du robot.
 */
#define Q_GETSENS			16

/**
 * Debug : valeur des encodeurs.
 */
#define Q_GETENC			17

/**
 * Debug : set les valeurs du PID pour l'asservissement des goto (delta)
 * @param P (1/1000)
 * @param I (1/1000)
 * @param D (1/1000)
 */
#define Q_SET_PID_POSITION_D	18

/**
 * Debug : set les valeurs du PID pour l'asservissement des goto (alpha)
 * @param P (1/1000)
 * @param I (1/1000)
 * @param D (1/1000)
 */
#define Q_SET_PID_POSITION_A	19

/**
 * Debug : set les valeurs du PID pour l'asservissement des rotations (delta)
 * @param P (1/1000)
 * @param I (1/1000)
 * @param D (1/1000)
 */
#define Q_SET_PID_ANGLE_D	20

/**
 * Debug : set les valeurs du PID pour l'asservissement des rotations (alpha)
 * @param P (1/1000)
 * @param I (1/1000)
 * @param D (1/1000)
 */
#define Q_SET_PID_ANGLE_A	21

/**
 * Debug on
 * @param b 0/1
 */
#define Q_DEBUG			22

/**
 * Reset
 */
#define Q_RESET			23

/**
 * @param speed
 */
#define Q_DSPEED			24

/**
 * @param speed
 */
#define Q_ASPEED			25

/**
 * @param p
 * @param i
 * @param d
 */
#define Q_SETPID			26

/**
 *
 */
#define Q_DUMP				27


// erreurs
#define	E_INVALID_CMD						-1
#define	E_INVALID_PARAMETERS_NUMBERS		-2
#define	E_INVALID_PARAMETERS_TYPE			-3
#define E_INVALID_PARAMETERS_VALUE			-4
#define E_TOURELLE_SETUP_FAIL				-5
#define W_SWITCH_COLOR						-21
#define W_JACK								-22
#define W_PING_AV							-30
#define W_PING_AR							-31
#define W_STOP								-42
#define E_TIMEOUT							-43
#define E_BLOCK								-44
#define E_SURCHAUFFE						-55
#define E_LOW_VOLTAGE						-56
#define E_MERDE_AX12						-57
#define E_CM5_NOT_CONN						-58
#define E_MERDE_AX18						-59
#define W_MS								-60
#define Q_KILL								-69

#define ID_ASSERV		"asserv_gros"

// paramètres
#define AVANT		0
#define ARRIERE		1
#define ARRET		2
#define GAUCHE		0
#define DROITE		1

#endif

