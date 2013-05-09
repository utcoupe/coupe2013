/****************************************
 * Author : Quentin C			*
 * Mail : quentin.chateau@gmail.com	*
 * Date : 26/04/13			*
 ****************************************/
#ifndef ROBOTSTATE_H
#define ROBOTSTATE_H

#include "parameters.h"
#include "encoder.h"
#include <math.h>

const double ENC_TICKS_TO_MM = (2.0*M_PI*(ENC_RADIUS))/(TICKS_PER_TURN);// = mm/ticks
const double ENC_MM_TO_TICKS = (TICKS_PER_TURN)/(2.0*M_PI*(ENC_RADIUS)); // = ticks/mm

double moduloTwoPI(double angle);

typedef struct pos pos;
struct pos{
	long x;
	long y;
	double angle;
};

typedef struct spd spd;
struct spd{
	double R;
	double L;
};

typedef struct acc acc;
struct acc{
	double R;
	double L;
};

class RobotState{
	public:
	void reset();
	pos getTicksPos();
	pos getMmPos();
	spd getTicksSpdEncoder();
	spd getTicksSpdMotor();
	spd getMmSpdEncoder();
	spd getMmSpdMotor();
	acc getMmAccMotor();
	void pushTicksPos(pos n_pos);
	void pushMmPos(pos n_pos);
	void update();
	RobotState();//Constructeur

	private:
	pos current_pos;
	spd spd_encoder, spd_motor, last_spd_motor; //conservées en ticks/ms
	acc acc_motor;
	unsigned long last_update;
	long last_ticksR, last_ticksL;
};

extern RobotState robot;

#endif
