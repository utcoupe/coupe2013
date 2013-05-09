/****************************************
 * Author : Quentin C			*
 * Mail : quentin.chateau@gmail.com	*
 * Date : 26/04/13			*
 ****************************************/
#ifndef CONTROL_H
#define CONTROL_H

#include <math.h>
#include "robotstate.h"
#include "goals.h"
#include "motor.h"
#include "PID.h"

#define PWM_SPD_OVERFLOW 3
#define SPD_OVERFLOW 2
#define PWM_OVERFLOW 1
#define NO_OVERFLOW 0

class Control{
	public:
	//Constructeur sans argument, utilise les #define
	Control();
	//Constructeur surchargé
	Control(int n_max_spd, int n_pwm_mini, double a_P, double a_I, double a_D, double d_P, double d_I, double d_D, double s_P, double s_I, double s_D);

	//compute : update le robot_state puis compute l'asserv
	int compute();

	//update_robot_state : permet d'update la robot state sans compute l'asserv
	void update_robot_state();

	void reset();

	//set des differents PIDs
	void setPID_angle(double n_P, double n_I, double n_D); //PID de l'asservissement angulaire
	void setPID_distance(double n_P, double n_I, double n_D); //PID de l'asservissement en position
	void setPID_speed(double n_P, double n_I, double n_D); //PID de l'asserv vitesse
	
	//gestion de la pwm_mini. Attention, il faut modifier les PIDs !
	void setPwmMin(int n_pwm_min); //Pwm minimale
	void setMaxSpd(int n_max_spd);

	//Push un goal
	int pushGoal(int ID, int p_type, double p_data_1 = 0, double p_data_2 = 0, double p_data_3 = 0);
	void nextGoal(); //va au goal suivant
	void clearGoals();

	//Toutes les positions sont renvoyée en mm, toutes les vitess en mm/ms = m/s
	void pushPos(pos n_pos); 
	pos getPos();
	spd getMotorSpd();
	spd getEncoderSpd();
	int getEncRError();
	int getEncLError();

	//Renvoie les valeurs des codeur (utile pour debug)
	long getEncR();
	long getEncL();

	//Permet la gestion de la pause
	void pause();
	void resume();

	private:
	//interface avec les PIDs
	int setPwms(int pwm_right, int pwm_left); //controles puis modification (renvoie l'overflow)
	int controlAngle(double goal_angle); //goal en radians
	int controlPos(double goal_x, double goal_y); //goal en mm
	int controlSpd(double goal_spdL, double goal_spdR);

	void applyPwm();

	int pwm_min;
	int max_spd;

	//Les pwm à appliquer
	int value_pwm_right, value_pwm_left;
};
#endif
