#include "Arduino.h"
#include "jack.h"
#include "protocole.h"
#include "message.h"

volatile char do_jack_check = 0;
volatile unsigned long jack_time = 0;

void initJackDetection() {
  pinMode(PIN_JACK, INPUT);
  digitalWrite(PIN_JACK, HIGH);
  attachInterrupt(INTERRUPT_JACK, jackInterrupt, RISING);
}

void jackInterrupt() {
  do_jack_check = 1;
  jack_time = millis();
}

void jackCheck() {
  if (do_jack_check and digitalRead(PIN_JACK) and (millis() - jack_time) > ACCEPTED_DELAY) {
    sendEvent(Q_JACK_REMOVED, 1);
    do_jack_check = 0;
  }
}
