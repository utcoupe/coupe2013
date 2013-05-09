/****************************************
 * Author : Quentin C			*
 * Mail : quentin.chateau@gmail.com	*
 * Date : 31/03/13			*
 ****************************************/
#include "motor.h"

//Set up de la lib "AFMotor"
AF_DCMotor motor_left(1, MOTOR12_64KHZ);
AF_DCMotor motor_right(2, MOTOR12_64KHZ);

void set_pwm_left(int pwm){
	if(pwm > 255)
		pwm = 255;
	else if(pwm < -255)
		pwm = -255;

	if(pwm >= 0)
		motor_left.run(FORWARD);
	else
		motor_left.run(BACKWARD);
	
	motor_left.setSpeed(abs(pwm));
}

void set_pwm_right(int pwm){
	if(pwm > 255)
		pwm = 255;
	else if(pwm < -255)
		pwm = -255;

	if(pwm >= 0)
		motor_right.run(FORWARD);
	else
		motor_right.run(BACKWARD);
	
	motor_right.setSpeed(abs(pwm));
}
