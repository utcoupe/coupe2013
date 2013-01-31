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
 * @param nombre
 */
#define Q_PING 			1

/**
 * Allume la led 13
 */
#define Q_LED		 		2

/**
 * Fait tourner un ax12
 * @param id
 * @param angle
 * @param speed
 */
#define Q_GOTO_AX12          3

/**
 * Renvoie la position actuelle d'un ax12
 * @param id
 */
#define Q_POS_AX12          4

/**
 * @event
 */
#define Q_JACK_REMOVED       1

/**
 * Quand on appuie sur le bouton d'arrêt d'urgence ça coupe
 * l'alimentation des actionneurs et envoie ce signal
 * @event
 */
#define Q_EMERGENCY_STOP       2

/**
 * Envoyé quand les ax12 sont détectés
 * Renvoie le mapping ID moteur -> index dans la chaine
 * @event
 */
#define Q_AX12_DETECTED       3

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

// Maximum : -127

#define ID_OTHERS		"others_gros"

// paramètres
#define AVANT		0
#define ARRIERE		1
#define ARRET		2
#define GAUCHE		0
#define DROITE		1

#endif

