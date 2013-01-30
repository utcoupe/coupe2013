#ifndef __Emergency_H_
#define __Emergency_H_

#define PIN_EMERGENCY 2
#define INTERRUPT_EMERGENCY 0
#define ACCEPTED_DELAY 200

void initEmergencyStopDetection();
void emergencyInterrupt();
char emergencyCheck();

#endif
