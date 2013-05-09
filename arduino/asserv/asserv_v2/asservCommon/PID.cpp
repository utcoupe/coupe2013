/****************************************
 * Author : Quentin C			*
 * Mail : quentin.chateau@gmail.com	*
 * Date : 27/04/13			*
 ****************************************/
#include "PID.h"

PID PID_Angle(ANG_P,ANG_I,ANG_D);
PID PID_Distance(DIS_P,DIS_I,DIS_D);
PID PID_SpdR(SPD_P,SPD_I,SPD_D);
PID PID_SpdL(SPD_P,SPD_I,SPD_D);
PID PID_AccR(ACC_P,ACC_I,ACC_D);
PID PID_AccL(ACC_P,ACC_I,ACC_D);

PID::PID(double n_P, double n_I, double n_D, double n_bias){
	PID::setPID(n_P,n_I,n_D);
	PID::setBias(n_bias);
	last_compute_time = timeMicros();
	PID::reset();
}

void PID::setPID(double n_P, double n_I, double n_D){
	if(n_P == 0 || n_I < 0 || n_D < 0)
		return; //Controle de PID correct
	P = n_P;
	I = n_I;
	D = n_D;
}

void PID::setBias(double n_bias){
	bias = n_bias;
}

void PID::reset(){
	last_error = 0;//ici, on va créer une premire dérivée trop grande, ce sera corrigé à l'initialisation du compute
	error_I = 0;
	last_compute_time = 0; //last compute sera set au premier compute
	output = 0;
	initDone = false;
}

double PID::compute(double error){
	unsigned long now = timeMicros();
	double error_D;
	double dt;

	if(!initDone){ //Lors du premier compute, on ne tient pas compte de I et D
		error_I = 0;
		error_D = 0;
		initDone = true;	
	}
	else{
		dt = (now - last_compute_time)/(1000000); //delta temps en s
		error_D = (error - last_error)/dt; //derivée = deltaErreur/dt
		error_I = error_I + error * dt; //integrale = somme(erreur*dt)
	}
	output = bias + (P*error) + (I*error_I) + (D*error_D); //calcul de la sortie avec le PID

	last_compute_time = now;
	last_error = error;

	return output;
}

double PID::getOutput(){
	return output;
}
