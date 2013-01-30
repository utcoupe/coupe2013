#ifndef C_AX12_H_
#define C_AX12_H_

#include "message.h"
#include "ax12.h"
#include "Arduino.h"



#define NB_MOTEURS 3

extern AX12 motor[NB_MOTEURS];
extern int goal[NB_MOTEURS];
extern int ordre[NB_MOTEURS];

void cherche_moteurs(void);
int tourner(int id, int id_moteur, int position, int vitesse);
int get_position(int id, int id_moteur);
int lire(int id, int ordre, int id_moteur);
void check_ax12_goals();

#endif /* C_AX12_H */
