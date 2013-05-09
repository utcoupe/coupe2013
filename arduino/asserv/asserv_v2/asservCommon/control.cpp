/****************************************
 * Author : Quentin C			*
 * Mail : quentin.chateau@gmail.com	*
 * Date : 27/04/13			*
 ****************************************/
#include "control.h"


Control control;


/********************************************************
 * 							*
 * 		      CLASSE CONTROLE			*
 *							*
 ********************************************************/

/********** PUBLIC **********/
Control::Control(){
	setPID_angle(ANG_P, ANG_I, ANG_D);
	setPID_distance(DIS_P, DIS_I, DIS_D);
	setPID_speed(SPD_P , SPD_I, SPD_D);
	setPwmMin(PWM_MIN);
	setMaxSpd(max_spd);

	value_pwm_right = 0;
	value_pwm_left = 0;
}

Control::Control(int n_max_spd, int n_pwm_mini, double a_P, double a_I, double a_D, double d_P, double d_I, double d_D, double s_P, double s_I, double s_D){
	setPID_angle(a_P, a_I, a_D);
	setPID_distance(d_P, d_I, d_D);
	setPID_speed(s_P , s_I, s_D);
	setPwmMin(n_pwm_mini);
	setMaxSpd(n_max_spd);

	value_pwm_right = 0;
	value_pwm_left = 0;
}

int Control::compute(){
	static bool reset = true;
	static unsigned long start_time;
	int overflow = NO_OVERFLOW;
	struct goal current_goal = fifo.getCurrentGoal();
	pos current_pos = robot.getMmPos();
	robot.update();

	if(current_goal.isPaused){
		value_pwm_right = 0;
		value_pwm_left = 0;
	}
	else{
		if(current_goal.isReached && current_goal.next != 0){//Si le but est atteint et que ce n'est pas le dernier, on passe au suivant
			current_goal = fifo.gotoNext();
			reset = true;
		}

		if(reset){//permet de reset des variables entre les goals
			PID_Angle.reset();
			PID_Distance.reset();
			PID_SpdR.reset();
			PID_SpdL.reset();
			start_time = timeMicros();
			reset = false;
		}
	
	/* Choix de l'action en fonction du type d'objectif */
		switch(current_goal.type){
			case TYPE_ANG :
			{
				overflow = controlAngle(current_goal.data_1);
				if(abs(current_goal.data_1 - current_pos.angle) < ERROR_ANGLE)
					fifo.pushIsReached();
				break;
			}

			case TYPE_POS :
			{
				double goal_a = atan2(current_goal.data_2 - current_pos.y, current_goal.data_1 - current_pos.x);
				double dd = sqrt((current_goal.data_2-current_pos.y)*(current_goal.data_2-current_pos.y)+(current_goal.data_1-current_pos.x)*(current_goal.data_1-current_pos.x));//erreur en distance
				if((goal_a - current_pos.angle) > MAX_ANGLE)//On tourne sur place avant de se déplacer
					overflow = controlAngle(goal_a);
				else
					overflow = controlPos(current_goal.data_1, current_goal.data_2);
				if(dd < ERROR_POS)
					fifo.pushIsReached();
				break;
			}

			case TYPE_PWM :
			{
				if(current_goal.data_1 > 255){
					overflow = PWM_OVERFLOW;
					current_goal.data_1 = 255;
				}
				else if(current_goal.data_1 < -255){
					overflow = PWM_OVERFLOW;
					current_goal.data_1 = -255;
				}
				if(current_goal.data_2 > 255){
					overflow = PWM_OVERFLOW;
					current_goal.data_2 = 255;
				}
				else if(current_goal.data_2 < -255){
					overflow = PWM_OVERFLOW;
					current_goal.data_2 = -255;
				}
				if((timeMicros() - start_time)/1000 <= current_goal.data_3){
					value_pwm_right = current_goal.data_2;
					value_pwm_left = current_goal.data_1;
				}
				else{
					value_pwm_right = 0;
					value_pwm_left = 0;
					fifo.pushIsReached();
				}
				break;
			}

			case TYPE_SPD :
			{
				if((timeMicros() - start_time)/1000 <= current_goal.data_3){
					overflow = controlSpd(current_goal.data_1, current_goal.data_2);
				}
				else{
					value_pwm_right = 0;
					value_pwm_left = 0;
					fifo.pushIsReached();
				}
				break;
			}
		}
	}

	applyPwm();
	return overflow;
}

void Control::update_robot_state(){
	robot.update();
}

void Control::reset(){
	value_pwm_left = 0;
	value_pwm_right = 0;
	encoderRight.reset();
	encoderLeft.reset();
	fifo.clearGoals();
	robot.reset();
	applyPwm();
}


void Control::setPID_angle(double n_P, double n_I, double n_D){
	PID_Angle.setPID(n_P, n_I, n_D);
}

void Control::setPID_distance(double n_P, double n_I, double n_D){
	PID_Distance.setPID(n_P, n_I, n_D);
}

void Control::setPID_speed(double n_P, double n_I, double n_D){
	PID_SpdR.setPID(n_P, n_I, n_D);
	PID_SpdL.setPID(n_P, n_I, n_D);
}

void Control::setPwmMin(int n_pwm_min){
	pwm_min = n_pwm_min;
}

void Control::setMaxSpd(int n_max_spd){
	max_spd = n_max_spd;
}

void Control::pushPos(pos n_pos){
	robot.pushMmPos(n_pos);
}

int Control::pushGoal(int ID, int p_type, double p_data_1, double p_data_2, double p_data_3){
	return fifo.pushGoal(ID, p_type, p_data_1, p_data_2, p_data_3);
}

void Control::nextGoal(){
	fifo.gotoNext();
}

void Control::clearGoals(){
	fifo.clearGoals();
}

pos Control::getPos(){
	return robot.getMmPos();
}

spd Control::getMotorSpd(){
	return robot.getMmSpdMotor();
}

spd Control::getEncoderSpd(){
	return robot.getMmSpdEncoder();
}

int Control::getEncRError(){
	return encoderRight.getError();
}

int Control::getEncLError(){
	return encoderLeft.getError();
}

long Control::getEncR(){
	return encoderRight.getTicks();
}

long Control::getEncL(){
	return encoderLeft.getTicks();
}

void Control::pause(){
	fifo.pause();
}

void Control::resume(){
	fifo.resume();
}

/********** PRIVATE **********/

int Control::setPwms(int pwm_right, int pwm_left){
	bool overflowPwm = false;
	//Ajout des pwm minimale : "shift" des pwm
	if(pwm_min != 0){
		if(pwm_right > 0)
			pwm_right += pwm_min;
		else if(pwm_right < 0)
			pwm_right -= pwm_min;
		if(pwm_left > 0)
			pwm_left += pwm_min;
		else if(pwm_left < 0)
			pwm_left -= pwm_min;
	}

	//Tests d'overflow
	if(pwm_right > 255){
		overflowPwm = true;
		pwm_right = 255;
	}
	else if(pwm_right < -255){
		overflowPwm = true;
		pwm_right = -255;
	}
	if(pwm_left > 255){
		overflowPwm = true;
		pwm_left = 255;
	}
	else if(pwm_left < -255){
		overflowPwm = true;
		pwm_left = -255;
	}
	value_pwm_right = pwm_right;
	value_pwm_left = pwm_left;

	return overflowPwm;
}

int Control::controlAngle(double goal_angle)
{
	pos current_pos = robot.getMmPos();
	spd current_spd = robot.getMmSpdMotor();
	acc current_acc = robot.getMmAccMotor();
	double consigneSpd, consignePwmR, consignePwmL, consigneAccR, consigneAccL;
	bool overflowSpd = false;
	bool overflowPwm = false;

	//Asservissement en position, renvoie une consigne de vitesse
	consigneSpd = PID_Angle.compute(goal_angle - current_pos.angle);
	if(abs(consigneSpd) > max_spd){//test d'overflow
		overflowSpd = true;
		consigneSpd = (consigneSpd/abs(consigneSpd))*max_spd;
	}
	
	//Asservissement en vitesse
	consigneAccR = PID_SpdR.compute(consigneSpd - current_spd.R); 
	consigneAccL = PID_SpdL.compute((-consigneSpd) - current_spd.L); 

	if(ENABLE_ACC_CONTROL){
		//Asservissement en acceleration
		consignePwmR = PID_AccR.compute(consigneAccR - current_acc.R);
		consignePwmL = PID_AccL.compute(consigneAccL - current_acc.L);
	}
	else{ //Si on asservit pas l'accélération, la sortie de PID sur la vitesse est la pwm.
		consignePwmL = consigneAccL;
		consignePwmR = consigneAccR;
	}

	overflowPwm = setPwms(consignePwmR, consignePwmL);

	if(overflowPwm && overflowSpd)
		return PWM_SPD_OVERFLOW;
	else if(overflowPwm)
		return PWM_OVERFLOW;
	else if(overflowSpd)
		return SPD_OVERFLOW;
	else
		return NO_OVERFLOW;
}

int Control::controlPos(double goal_x, double goal_y)
{
	pos current_pos = robot.getMmPos();
	spd current_spd = robot.getMmSpdMotor();
	acc current_acc = robot.getMmAccMotor();
	double consigneSpdR, consigneSpdL, consigneSpdAngle, consigneSpdDistance, consignePwmR, consignePwmL, consigneAccR, consigneAccL;
	bool overflowSpd = false;
	bool overflowPwm = false;
	double dx = goal_x - current_pos.x;
	double dy = goal_y - current_pos.y;

	//Asservissement en position, renvoie une consigne de vitesse
	//Calcul des spd angulaire
	consigneSpdAngle = PID_Angle.compute(atan2(dy, dx) - current_pos.angle); //erreur = angle à corriger pour etre en direction du goal
	//Calcul des spd de distance
	consigneSpdDistance = PID_Distance.compute(sqrt(dx*dx+dy*dy)); //erreur = distance au goal

	consigneSpdR = consigneSpdDistance + consigneSpdAngle; //On additionne les deux speed pour avoir une trajectoire curviligne => n'utilise qu'avec les faibles angles
	consigneSpdL = consigneSpdDistance - consigneSpdAngle; //On additionne les deux speed pour avoir une trajectoire curviligne => n'utilise qu'avec les faibles angles

	if(abs(consigneSpdR) > max_spd){//test d'overflow
		overflowSpd = true;
		consigneSpdR = (consigneSpdR/abs(consigneSpdR))*max_spd;
	}
	if(abs(consigneSpdL) > max_spd){//test d'overflow
		overflowSpd = true;
		consigneSpdL = (consigneSpdL/abs(consigneSpdL))*max_spd;
	}
	
	//Asservissement en vitesse
	consigneAccR = PID_SpdR.compute(consigneSpdR - current_spd.R); 
	consigneAccL = PID_SpdL.compute(consigneSpdL - current_spd.L); 

	if(ENABLE_ACC_CONTROL){
		//Asservissement en acceleration
		consignePwmR = PID_AccR.compute(consigneAccR - current_acc.R);
		consignePwmL = PID_AccL.compute(consigneAccL - current_acc.L);
	}
	else{ //Si on asservit pas l'accélération, la sortie de PID sur la vitesse est la pwm.
		consignePwmL = consigneAccL;
		consignePwmR = consigneAccR;
	}

	overflowPwm = setPwms(consignePwmR, consignePwmL);

	if(overflowPwm && overflowSpd)
		return PWM_SPD_OVERFLOW;
	else if(overflowPwm)
		return PWM_OVERFLOW;
	else if(overflowSpd)
		return SPD_OVERFLOW;
	else
		return NO_OVERFLOW;
}

int Control::controlSpd(double goal_spdL, double goal_spdR)
{
	spd current_spd = robot.getMmSpdMotor();
	acc current_acc = robot.getMmAccMotor();
	double consignePwmR, consignePwmL, consigneAccR, consigneAccL;
	bool overflowSpd = false;
	bool overflowPwm = false;

	if(abs(goal_spdR) > max_spd || abs(goal_spdL) > max_spd){
		overflowSpd = true;
	}

	//Asservissement en vitesse
	consigneAccR = PID_SpdR.compute(goal_spdR - current_spd.R); 
	consigneAccL = PID_SpdL.compute(goal_spdL - current_spd.L); 

	if(ENABLE_ACC_CONTROL){
		//Asservissement en acceleration
		consignePwmR = PID_AccR.compute(consigneAccR - current_acc.R);
		consignePwmL = PID_AccL.compute(consigneAccL - current_acc.L);
	}
	else{ //Si on asservit pas l'accélération, la sortie de PID sur la vitesse est la pwm.
		consignePwmL = consigneAccL;
		consignePwmR = consigneAccR;
	}

	overflowPwm = setPwms(consignePwmR, consignePwmL);

	if(overflowPwm && overflowSpd)
		return PWM_SPD_OVERFLOW;
	else if(overflowPwm)
		return PWM_OVERFLOW;
	else if(overflowSpd)
		return SPD_OVERFLOW;
	else
		return NO_OVERFLOW;
}

void Control::applyPwm(){
	set_pwm_left(value_pwm_left);
	set_pwm_right(value_pwm_right);
}
