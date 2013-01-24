#include "ax12.h"
#include "Arduino.h"

#define ACTUEL 1
#define NOUVEAU 9

double speed(int val) {
  return 2000000 / (double)(val + 1);
}

void setup()
{ 
  // on détecte un ax12
  AX12::init(speed(ACTUEL));
  int detect[1];
  AX12::autoDetect (detect, 1);

  // on l'initialise
  if (detect[0] != -1) {
    AX12 motor = AX12(detect[0]);
    motor.writeInfo(BAUD_RATE, NOUVEAU);
  }
}

void loop()
{

}
