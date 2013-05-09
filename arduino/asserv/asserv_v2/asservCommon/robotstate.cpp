/****************************************
 * Author : Quentin C			*
 * Mail : quentin.chateau@gmail.com	*
 * Date : 26/04/13			*
 ****************************************/
#include "robotstate.h"

RobotState robot;

double moduloTwoPI(double angle){
	if(angle>0)
	        angle = fmod(angle+M_PI, 2.0*M_PI)-M_PI;
	else
           	angle = fmod(angle-M_PI, 2.0*M_PI)+M_PI;
	return angle;
}


/********************************************************
 * 							*
 * 		      CLASSE ROBOTSTATE			*
 *							*
 ********************************************************/
RobotState::RobotState()
{
	RobotState::reset();
}

void RobotState::reset(){
	current_pos.x = 0;
	current_pos.y = 0;
	current_pos.angle = 0;
	spd_encoder.R = 0;
	spd_encoder.L = 0;
	spd_motor.R = 0;
	spd_motor.L = 0;
	last_ticksR = encoderRight.getTicks();
	last_ticksL = encoderLeft.getTicks();
}

pos RobotState::getTicksPos(){
	return current_pos;
}

pos RobotState::getMmPos(){
	pos mm_pos = current_pos;
	mm_pos.x = mm_pos.x * ENC_TICKS_TO_MM;
	mm_pos.y = mm_pos.y * ENC_TICKS_TO_MM;
	return mm_pos;
}

spd RobotState::getTicksSpdEncoder(){
	return spd_encoder;
}

spd RobotState::getTicksSpdMotor(){
	return spd_motor;
}

spd RobotState::getMmSpdMotor(){
	spd mm_spd_motor = spd_motor;
	mm_spd_motor.R = mm_spd_motor.R * ENC_TICKS_TO_MM;
	mm_spd_motor.L = mm_spd_motor.L * ENC_TICKS_TO_MM;
	return mm_spd_motor;
}

spd RobotState::getMmSpdEncoder(){
	spd mm_spd_encoder = spd_encoder;
	mm_spd_encoder.R = mm_spd_encoder.R * ENC_TICKS_TO_MM;
	mm_spd_encoder.L = mm_spd_encoder.L * ENC_TICKS_TO_MM;
	return mm_spd_encoder;
}

acc RobotState::getMmAccMotor(){
	acc mm_acc_motor = acc_motor;
	mm_acc_motor.R *= ENC_TICKS_TO_MM;
	mm_acc_motor.L *= ENC_TICKS_TO_MM;
	return mm_acc_motor;
}

void RobotState::pushTicksPos(pos n_pos){
	current_pos = n_pos;
}

void RobotState::pushMmPos(pos n_pos){
	current_pos.x = n_pos.x * ENC_MM_TO_TICKS;
	current_pos.y = n_pos.y * ENC_MM_TO_TICKS;
	current_pos.angle = n_pos.angle;
}

void RobotState::update(){
	unsigned long now = timeMicros();
	long ticksR = encoderRight.getTicks();
	long ticksL = encoderLeft.getTicks();
	int dt = now - last_update;
	int dl = ticksL - last_ticksL;
	int dr = ticksR - last_ticksR;

	//calcul des vitesses en ticks/ms
	spd_encoder.R = (dr/dt)*1000;
	spd_encoder.L = (dl/dt)*1000;

	//On s'amuse un peu avec la géométrie
	spd_motor.L = (((ENTRAXE_ENC - ENTRAXE_MOTOR)/2)/ENTRAXE_ENC)*(spd_encoder.R - spd_encoder.L) + spd_encoder.L;
	spd_motor.L = ((((ENTRAXE_ENC - ENTRAXE_MOTOR)/2)+ENTRAXE_MOTOR)/ENTRAXE_ENC)*(spd_encoder.R - spd_encoder.L) + spd_encoder.L;
	
	current_pos.angle = moduloTwoPI(current_pos.angle + (dr - dl)/(ENTRAXE_ENC * ENC_MM_TO_TICKS)); //approximation tan(x) = x pour x petit

	current_pos.x = current_pos.x + ((dr + dl)/2)*cos(current_pos.angle);
	current_pos.y = current_pos.y + ((dr + dl)/2)*sin(current_pos.angle);

	acc_motor.R = (last_spd_motor.R - spd_motor.R)/dt;
	acc_motor.L = (last_spd_motor.L - spd_motor.L)/dt;

	//prepare la prochaine update
	last_update = now;
	last_ticksR = ticksR;
	last_ticksL = ticksL;
	last_spd_motor = spd_motor;
}
