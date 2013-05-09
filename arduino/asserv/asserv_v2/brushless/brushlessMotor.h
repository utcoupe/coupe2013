//Adapted from Adafruit Motor Shield by Quentin C for UTCoupe
//Defined for brushless controler shield designed by UTCoupe
//08/04/2013

#ifndef BRUSHLESSMOTOR_H
#define BRUSHLESSMOTOR_H

#include "parameters.h"

#define MOTOR1_DIR 50
#define MOTOR2_DIR 40

#define MOTOR1_EN 48
#define MOTOR2_EN 38

#define MOTOR1_SPD 5
#define MOTOR2_SPD 4

#define MOTOR1_RDY 42
#define MOTOR2_RDY 36

#define MOTOR1_DIG1 46
#define MOTOR1_DIG2 44
#define MOTOR2_DIG1 30
#define MOTOR2_DIG2 32

#define MOTOR_LEFT 1
#define MOTOR_RIGHT 2

#define FORWARD 1
#define BACKWARD 2
#define BRAKE 3
#define RELEASE 4

//Pins codeurs liés à la shield
#define PIN_ENC_RIGHT_A 19
#define PIN_ENC_RIGHT_B 20
#define PIN_ENC_RIGHT_0 21
#define PIN_ENC_LEFT_A 3
#define PIN_ENC_LEFT_B 2
#define PIN_ENC_LEFT_0 18

#define INTERRUPT_ENC_LEFT_A 1
#define INTERRUPT_ENC_LEFT_B 0
#define INTERRUPT_ENC_LEFT_0 5
#define INTERRUPT_ENC_RIGHT_A 4
#define INTERRUPT_ENC_RIGHT_B 3
#define INTERRUPT_ENC_RIGHT_0 2


class Motor
{
	public:
	Motor(int n_motor_side);
	void run(int cmd);
	void setPwm(int pwm);

	private:
	int motor_side;
};

#endif
