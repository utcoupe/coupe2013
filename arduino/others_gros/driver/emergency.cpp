#include "Arduino.h"
#include "emergency.h"
#include "protocole.h"
#include "message.h"

volatile char do_emergency_check = 0;
volatile unsigned long emergency_time = 0;

void initEmergencyStopDetection() {
  attachInterrupt(INTERRUPT_EMERGENCY, emergencyInterrupt, RISING);
}

void emergencyInterrupt() {
  do_emergency_check = 1;
  emergency_time = millis();
}

char emergencyCheck() {
  if (do_emergency_check and digitalRead(PIN_EMERGENCY) and (millis() - emergency_time) > ACCEPTED_DELAY) {
    sendEvent(Q_EMERGENCY_STOP, 1);
    do_emergency_check = 0;
    return 1;
  } else {
    return 0;
  }
}
