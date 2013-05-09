/****************************************
 * Author : Quentin C			*
 * Mail : quentin.chateau@gmail.com	*
 * Date : 31/03/13			*
 ****************************************/
#include "motor.h"

Motor motor_left(MOTOR_LEFT);
Motor motor_right(MOTOR_RIGHT);

void set_pwm_left(int pwm){
	if(pwm > 255)
		pwm = 255;
	else if(pwm < -255)
		pwm = -255;

	if(pwm >= 0)
		motor_left.run(FORWARD);
	else
		motor_left.run(BACKWARD);
	
	motor_left.setPwm(abs(pwm));
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
	
	motor_right.setPwm(abs(pwm));
}
