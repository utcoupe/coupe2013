/****************************************
 * Author : Quentin C			*
 * Mail : quentin.chateau@gmail.com	*
 * Date : 01/04/13			*
 ****************************************/
#ifndef PID_H
#define PID_H

#include "parameters.h"
#include "compaArduino.h"


class PID{
	public:
	void setPID(double n_P, double n_I, double n_D);
	void setBias(double n_bias);
	void reset();
	double getOutput();
	double compute(double error);	
	PID(double n_p = 0, double n_I = 0, double n_D = 0, double n_bias = 0);
	private:
	double P, I, D;
	unsigned long last_compute_time; //dernier compute pour la (D)érivée
	double last_error; //derniere erreur pour la (D)ériée
	double output;
	double error_I; //somme des erreurs * intervale = (I)ntégrale
	double bias; //somme constante ajoutée au résultat du PID
	bool initDone; //permet d'éviter les erreurs dues à l'absence de dérivée au premier compute
};

extern PID PID_Angle;
extern PID PID_Distance;
extern PID PID_SpdR;
extern PID PID_SpdL;
extern PID PID_AccR;
extern PID PID_AccL;
#endif
