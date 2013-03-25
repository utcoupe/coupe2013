#ifndef PROTOCOLE_H
#define PROTOCOLE_H


// cf. protocole
#define SEP			'+' // tout est séparé par ça


//
#define BLEU  	1
#define ROUGE 	2



// all
#define Q_DEBUG				99
/**
 * Fonction retournant la postion des robots en coordonnées cartésiennes
 * */
#define Q_GETDATA 				11

/**
 * Dit à l'hokuyo que notre robot est sur la couleur rouge
 * */
#define Q_SET_REDCOLOR 		2

/**
 * Dit à l'hokuyo que notre robot est sur la couleur violet
 * */
#define Q_SET_PURPLECOLOR 		3

/**
 * Ping
 */
#define Q_PING					4

/**
 * Ident
 */
#define Q_IDENT				5

/**
 * Signal pour stoper l'application hokuyo
 * */
#define Q_KILL 9




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
#define E_KILL								-69


#endif

