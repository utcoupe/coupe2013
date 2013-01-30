#ifndef __Jack_H_
#define __Jack_H_

#define PIN_JACK 3
#define INTERRUPT_JACK 1
#define ACCEPTED_DELAY 200

void initJackDetection();
void jackInterrupt();
void jackCheck();

#endif
