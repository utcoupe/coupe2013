//Adapted from Adafruit Motor Shield by Quentin C for UTCoupe
//Defined for brushless controler shield designed by UTCoupe

#ifndef _brushlessMotor_h_
#define _brushlessMotor_h_

#include <inttypes.h>
#include <avr/io.h>
#include "parameters.h"

#define MOTOR12_64KHZ _BV(CS20)  // no prescale
#define MOTOR12_8KHZ _BV(CS21)   // divide by 8
#define MOTOR12_2KHZ _BV(CS21) | _BV(CS20) // divide by 32
#define MOTOR12_1KHZ _BV(CS22)  // divide by 64

#define MOTOR1_DIR 50
#define MOTOR2_DIR 40

#define MOTOR1_EN 48
#define MOTOR2_EN 38

#define MOTOR1_SPD 3
#define MOTOR2_SPD 5

#define MOTOR1_RDY 42
#define MOTOR2_RDY 36

#define MOTOR1_DIG1 46
#define MOTOR1_DIG2 44
#define MOTOR2_DIG1 30
#define MOTOR2_DIG2 32

#define FORWARD 1
#define BACKWARD 2
#define BRAKE 3
#define RELEASE 4


class Motor
{
 public:
  Motor(uint8_t motornum, uint8_t freq =  MOTOR12_8KHZ);
  void run(uint8_t);
  void setSpeed(uint8_t);

 private:
  uint8_t motornum, pwmfreq;
};

#endif
