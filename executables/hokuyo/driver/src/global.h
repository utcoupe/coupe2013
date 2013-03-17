/**
 * \file 	global.h
 * \author 	Xavier RODRIGUEZ
 * \date	29/02/2012
 * 
 * */
#ifndef GLOBAL_H
#define GLOBAL_H
 
#define DEBUG 0
 
// Paramétres
#define NB_MAX_ROBOT 4

// Tag des paramétres à passer 
#define TAG_PORTCOM	"-port"
#define TAG_COLOR	"-color"

// --- Définitions ---
	struct coord {
		int x;
		int y;
	};
	
	#define LX 3000.0			// Largeur et longueur de la table en mm
	#define LY 2000.0
	#define TETA_DIAG 0.59 	// en radian, sinon 33.69 degree
	#define RAD90 1.57
	#define TOLERANCE 80  	// Tolérence de distance entre deux points du même robot
	#define TETA_GAT 0.153  // Angle limite du gateau dans la champ de vision de l'hokuyo (modifier en cas d'erreur dues aux imprecisions)
	#define DIST_GAT 1300 // Distance entre bord du plateau et début du gateau
							
	#define TOL_BORDS 150	//

	#define ABS(a)	   (((a) < 0) ? -(a) : (a))

	#define ROUGE 0
	#define BLEU 1

#endif  //GLOBAL_H
