
import sys
sys.path.append('../../lib')

from .action import *

import utcoupe

import time


#définition des constantes des rayons
R_ASSIETTE = 120 # 170/2*sqrt(2)
R_VERRE = 40

#point_acces = quand le robot est arrivé à ce point, il déclenche l'action

class ActionCadeau(Action):
	def __init__(self, ia, robot, enemies, point_acces):
		Action.__init__(self, ia, robot, enemies, point_acces)
		
	def run(self):
		print("\nACTION CADEAU\n")
		self.robot.asserv.turn(self.ia.a(90))	#block et block_level à virer dans l'asserv,utilisé pour irc
		#permet d'être sûr que le robot est en face du cadeau avant d'aller le taper
		time.sleep(0.5)
		
		#avance
		self.robot.asserv.pwm(100,100,500)			# !!!!!! valeur à tester, mise totalement arbitrairement
		#commande pwm : puissance roue droite, puissance roue gauche, temps
		#doit avancer de 100mm (car point_acces le position à 100+rayon_minirobot de l'action
		time.sleep(0.5)
		
		#recul
		self.robot.asserv.pwm(-100,-100,500)			# !!!!!! valeur à tester, mise totalement arbitrairement
		
		#fini
		self.clean()

		print("Cadeau headshot !\n")
	
	def __repr__(self):	#surcharge du print
		return "ActionCadeau(%s, %s)" % (self.point_acces, self.score)
		
		

class ActionGetCerise(Action):
		
	DIRECTION_RIGHT		= 0	# il faudra tourner de 90° à droite
	DIRECTION_LEFT		= 1 # il faudra tourner de 90° à gauche (-90°)
	DIRECTION_FRONT		= 2 # pas besoin de tourner
	
	def __init__(self, ia, robot, enemies, point_acces, direction):
		Action.__init__(self, ia, robot, enemies, point_acces)
		self.direction = direction
		
	def run(self):
		print("\nACTION RECUPERER CERISE\n")
		self.robot.asserv.turn(self.ia.a(90))
		#permet d'être sûr que le robot est en face de l'objet
		time.sleep(0.5)
		
		if self.direction != self.DIRECTION_FRONT:
			# le robot se tourne face à l'assiette
			angle = 90 if self.direction==self.DIRECTION_LEFT else -90
			asserv.turn(angle)
		
		#avance
		self.robot.asserv.pwm(100,100,100)			# !!!!!! valeur à tester, mise totalement arbitrairement		
		time.sleep(0.5)
		self.robot.actionneurs.cerise.get()
		
		#recul
		self.robot.asserv.pwm(-100,-100,100)			# !!!!!! valeur à tester, mise totalement arbitrairement
		
		#fini
		self.clean()

		print("Cerise done !\n")
	
	def __repr__(self):	#surcharge du print
		return "ActionGetCerise(%s, %s)" % (self.point_acces, self.score)
		
		
class ActionVerre(Action): #ouvrir les pinces au départ
	"""En mode buldo, on garde les pinces ouvertes, on se rend à la position et on avance toujours tout droit pour retrouner
	à la zone de départ (et normalement le robot devrait récup 2 verres"""
	def __init__(self, ia, robot, enemies, point_acces):
		Action.__init__(self, ia, robot, enemies, point_acces)
		
	def run(self):
		print("\nACTION VERRE\n")
		self.robot.asserv.turn(180,100)
		self.robot.asserv.goto(self.robot.pos[0]-310,self.robot.pos[1],100)	#cherche le verre suivant
		self.robot.asserv.goto(250,1400,100)	#retourne à l'air de jeu
		
		#fini
		self.clean()
		
	def __repr__(self):	#surcharge du print
		return "ActionVerre(%s, %s)" % (self.point_acces, self.score)
		
#implémenter l'essaye de récupération des verres ennemis si jamais on a le temps

class ActionShootCerise(Action):
	
	def __init__(self, ia, robot, enemies, point_acces):
		Action.__init__(self, ia, robot, enemies, point_acces)

	def run(self):
		
		#active le tir de cerises
		self.robot.actionneurs.cerise.shoot() 
		
		#fini
		self.clean()

		print("Shoot de cerise done !")
		
	def __repr__(self):
		return "ActionShootCerise(%s, %s)" % (self.point_acces, self.score)	
		
class ActionBougie(Action):
		
	def __init__(self, ia, robot, enemies, point_acces, color_bougie):
		Action.__init__(self, ia, robot, enemies, point_acces)
		if (color_bougie >= 100):
			bras_gauche = 1
		else:
			bras_gauche = 0
		if (color_bougie%100 >= 10):
			bras_haut = 1
		else:
			bras_haut = 0
		if (color_bougie%10):
			bras_droite = 1
		else:
			bras_droite = 0
			
# EXPLICATION
# color_bougie est composé comme suit : bras_gauche bras_haut bras_droite
# Pour chacun, on a 1 si on veut activer le bras et 0 sinon
# Donc pour activer le bras gauche et le bras droite, on envoit : color_bougie = 101
# Les if permettent d'initialiser les valeurs depuis color_bougie

	def run(self):
		
		#tape les bougies (en foncion de la couleur)
		self.robot.actionneurs.bougie(bras_gauche, bras_haut, bras_droite)
		
		#fini
		self.clean()

		print("Bougie soufflée !")
		
	def __repr__(self):
		return "ActionBougie(%s, %s)" % (self.point_acces, self.score)	


# Il y a encore les get_action à changer

#définition des positions des items suivant leurs couleurs (afin d'alléger les get_actions)

#def position_verre(team):
verre_nous = []	#tableau listant la positions de nos verres
verre_ennemis = []	#tableau listant la position des verres ennemis
"""On part du principe qu'en mode buldo, on va pour choper le premier verre (celui le plus éloigné)
et qu'on revient vers notre zone de départ et qu'on chope le deuxième au passage (donc ça fait seulement 3 positions"""
"""if team == red:
		verre_nous[0] = (900,950)
		verre_nous[1] = (1200,950)
		verre_nous[2] = (1050,1200)
		verre_nous[3] = (1350,1200)
		verre_nous[4] = (900,1450)
		verre_nous[5] = (1200,1450)"""
verre_nous.append((1200 + R_VERRE + 20,950)) #extremité verre 1
verre_nous.append((1350 + R_VERRE + 20,1200))#extremité verre 2
verre_nous.append((1200 + R_VERRE + 20,1450))#extremité verre 3

verre_ennemis.append((2100,950))
verre_ennemis.append((1800,950))
verre_ennemis.append((1950,1200))
verre_ennemis.append((1650,1200))
verre_ennemis.append((2100,1450))
verre_ennemis.append((1800,1450))
"""else: #team == bleu
		verre_ennemis[0] = (900,950)
		verre_ennemis[1] = (1200,950)
		verre_ennemis[2] = (1050,1200)
		verre_ennemis[3] = (1350,1200)
		verre_ennemis[4] = (900,1450)
		verre_ennemis[5] = (1200,1450)
		verre_nous[0] = (2100,950)
		verre_nous[1] = (1800,950)
		verre_nous[2] = (1950,1200)
		verre_nous[3] = (1650,1200)
		verre_nous[4] = (2100,1450)
		verre_nous[5] = (1800,1450)"""
	#return verre_nous, verre_ennemis
	
#def position_assiette(team):
	
assiette_nous = []	#tableau listant la positions de nos assiettes
	#assiette_ennemis[]	#tableau listant la position des assiettes ennemis
	#if team == red:
assiette_nous.append((200,250 + R_ASSIETTE + 20)) #20 de marge
assiette_nous.append((200,1000 - R_ASSIETTE - 20)) #orienté vers le bas
assiette_nous.append((200 + R_ASSIETTE + 20,1750)) #sur x car on chope l'assiette de côté
"""else:
		assiette_nous[0] = (2800,250)
		assiette_nous[1] = (2800,1000)
		assiette_nous[2] = (2800,1750)"""
	#return assiette_nous
		
#def position_cadeau(team):
cadeau_nous = []
	#if team == red:
cadeau_nous.append((525,2000 - 100)) #100 de marge
cadeau_nous.append((1125,2000 - 100))
cadeau_nous.append((1725,2000 - 100))
cadeau_nous.append((2325,2000 - 100))
"""else:
		cadeau_nous[0] = (675,2000)
		cadeau_nous[1] = (1275,2000)
		cadeau_nous[2] = (1875,2000)
		cadeau_nous[3] = (2475,2000)"""
	#return cadeau_nous

def get_actions_bigrobot(ia, robot, enemies):
	actions = []	
	
	#actions.append(ActionBouteille(ia, robot, enemies, ia.p((640, 2000 - R_BIGROBOT - 100))))
	#actions.append(ActionBouteille(ia, robot, enemies, (ia.x(1883), 2000 - R_BIGROBOT - 100)))
	#actions.append(ActionLingo(ia, robot, enemies, ia.p((400, 900))))
	#actions.append(ActionTotemHaut(ia, robot, enemies, ia.p((1400,1000-125-R_BIGROBOT-40))))
	#actions.append(ActionTotemBas(ia, robot, enemies, ia.p((1400,1000+125+R_BIGROBOT+40))))
	#actions.append(ActionFinalize(ia, robot, enemies, ia.p((400, 950))))
	#actions.append(ActionTotem3(ia, robot, enemies, ia.p((2200,1000-125-R_BIGROBOT-60)), ActionTotem.DIRECTION_HAUT))
	#actions.append(ActionTotem3(ia, robot, enemies, ia.p((2200,1000+125+R_BIGROBOT+60)), ActionTotem.DIRECTION_BAS))
	"""
	for i in range(0,3):
		print(assiette_nous[i])
		actions.append(ActionGetCerise(ia, robot, enemies, ia.p(assiette_nous[i]), ActionGetCerise.DIRECTION_FRONT))
	actions.append(ActionShootCerise(ia, robot, enemies, ia.p((1300,800))))#valeur mise au hasard
	"""
	#Florent, comment on implémente l'action bougie avec le gateau.py ?

	return actions

def get_actions_minirobot(ia, robot, enemies):
	actions = []
	# bouteilles
	"""actions.append(ActionBouteille(robot, enemies, (640, 2000 - R_MINIROBOT - 10)))
	actions.append(ActionBouteille(robot, enemies, (1883, 2000 - R_MINIROBOT - 10)))
	actions.append(ActionCarte(robot, enemies, (1500, R_MINIROBOT + 10)))"""
	#for i in range(0,4):
		#actions.append(ActionCadeau(ia, robot, enemies, ia.p(cadeau_nous[0])))
	for i in range(0,3):
		actions.append(ActionVerre(ia, robot, enemies, verre_nous[i]))
	return actions
