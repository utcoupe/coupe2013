#include "Arduino.h"
#include "controle_ax12.h"
#include "message.h"
#include "protocole.h"

AX12 motor[NB_MOTEURS];
int reverse[18] = {-1, -1, -1, -1, -1, -1, -1, -1, -1,
                   -1, -1, -1, -1, -1, -1, -1, -1, -1};
int goal[NB_MOTEURS];
int ordre[NB_MOTEURS];

int tourner(int id, int id_moteur, int position, int vitesse) {
  if (position < -150 || position > 150 || vitesse < 0 || vitesse > 114 || id_moteur > NB_MOTEURS) {
    sendError(id, E_INVALID_PARAMETERS_VALUE);
  }
  else {
    int speed;
    int angle = map(position, -150, 150, 0, 1023);
    if (vitesse == 0)
        speed = 0;
    else
        speed = map(vitesse, 1, 114, 0, 1023);

    goal[id_moteur] = angle;
    ordre[id_moteur] = id;
    motor[id_moteur].writeInfo(MOVING_SPEED, speed);
    motor[id_moteur].writeInfo (GOAL_POSITION, angle);
    // TODO : tester les valeurs de retour des writeInfo
  }
}

int get_position(int id_moteur, char* erreur) {
  *erreur = 0;
  if (id_moteur > NB_MOTEURS) {
    *erreur = E_INVALID_PARAMETERS_VALUE;
    return 0;
  }
  else {
      int valeur = lire (PRESENT_POSITION, id_moteur, erreur);
      if (erreur != 0)
          return 0;
    else
      return (valeur*0.293255132)-150;
  }
}

int lire(int ordre, int id_moteur, char* erreur) {
  *erreur = 0;
  if (id_moteur > NB_MOTEURS) {
    *erreur = E_INVALID_PARAMETERS_VALUE;
    return 0;
  }
  else {
    motor[id_moteur].readInfo (ordre);
    return motor[id_moteur].status_data;
  }
}

void cherche_moteurs(void) {
  AX12::init(1000000);

  // on demande la déection de 4 AX12
  int detect[NB_MOTEURS];
  int c = 0;
  AX12::autoDetect (detect, NB_MOTEURS);

  for (byte i=0; i<NB_MOTEURS; i++) {
    //sendMessage(8, detect[i]);
    if (detect[i] >= 0) {
      c++;

      // detect[i] est l'id interne du moteur (modifiable si besoin mais à ne pas faire pendant que le robot tourne)
      // i est le numéro du moteur dans la chaine
      motor[i] = AX12(detect[i]);
      reverse[detect[i]] = i;
      goal[i] = 200;
      ordre[i] = -1;

      motor[i].writeInfo (TORQUE_ENABLE, 1); // on doit activer ça sinon le moteur ne développe pas de couple
      motor[i].setEndlessTurnMode(false); // ça semble pas utile: TODO tester sans pour pour voir

      // CW : ClockWise
      // et donc CCW : Counter-CLockWise

      // on veut que le moteur aille EXACTEMENT au bon endroit
      motor[i].writeInfo (CW_COMPLIANCE_MARGIN, 0);
      motor[i].writeInfo (CCW_COMPLIANCE_MARGIN, 0);

      // euh ça sert pour l'asservissement apparemment
      // ratio couple/erreur de position
      motor[i].writeInfo (CW_COMPLIANCE_SLOPE, 95);
      motor[i].writeInfo (CCW_COMPLIANCE_SLOPE, 95);

      motor[i].writeInfo (PUNCH, 150);

      // vitesse maximale (dépend du courant fourni évidemment)
      motor[i].writeInfo (MAX_TORQUE, 1023);

      motor[i].writeInfo (LIMIT_TEMPERATURE, 85); //valeur par défaut
      motor[i].writeInfo (DOWN_LIMIT_VOLTAGE, 60); //valeur par défaut
      motor[i].writeInfo (UP_LIMIT_VOLTAGE, 190); //valeur par défaut

      motor[i].writeInfo (RETURN_LEVEL, 2);
      motor[i].writeInfo (RETURN_DELAY_TIME, 150);

      // sécurités interne, on arrête le moteur quand trop de charge ou surchauffe ou problem d'alim
      // On allume la LED quand il y a un problème d'alim
      motor[i].writeInfo (ALARM_SHUTDOWN, 0b00100101);
      motor[i].writeInfo (ALARM_LED, 0b00000001);
    }
  }
  sendEvent(Q_AX12_DETECTED, 18, reverse); //on renvoie le nombre de moteurs détectés
}

/**
 * On regarde si les ax12 sont arrivés aux positions demandées
 * Si oui on envoie une réponse au PC
 */
void check_ax12_goals() {
    char error;
    for (char i=0; i<NB_MOTEURS; i++) {
        if (ordre[i] != -1) {
            int pos = lire(PRESENT_POSITION, i, &error);
            if (error == 0) {
                int err = pos - goal[i];
                if(err <= 4 && err >= -4) {
                    sendResponse(ordre[i], 0);
                    ordre[i] = -1;
                }
            }
        }
    }
}
