
import sys
sys.path.append('../../lib')

from .action import *

import utcoupe

import time

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
		
		
class ActionVerre(Action):
	def __init__(self, ia, robot, enemies, point_acces):
		Action.__init__(self, ia, robot, enemies, point_acces)
		
	def run(self):
		print("\nACTION VERRE\n")
		
		#va chercher le premier verre
		self.recupFirst()
		
		#va chercher le deuxième verre sur la même ligne
		self.recupSecond()
		
		#retourne dans l'air de jeu pour y déposer les 2 verres
		self.recupRetour()
		
		#fini
		self.clean()

		print("Verre récup !\n")
		
		
	def recupFirst(self):	#action qui permet de récupérer le premier verre
		# on considère que le point d'entrée pour déclencher l'action se situe à 50 du verre
		self.robot.asserv.turn(self.ia.a(90))
		self.robot.actionneurs.pince.pos(PINCE_BAS)		#actionneur à coder qui fera descendre la pince pour l'avoir en bas
		
		#avance (de 52mm afin d'être sûr qu'on soit sur le verre)
		self.robot.asserv.pwm(100,100,100)	 	#!!!!!! valeur à tester, mise totalement arbitrairement
		
		#saisi le verre
		self.robot.actionneurs.pince.get()
		self.robot.actionneurs.pince.pos(PINCE_HAUT)	#on lève la pince
		
		#faire l'appel vers la fonction recupSecond pour pouvoir récupérer le second verre	
		
	def recupSecond(self):		#action qui permet de récupérer le second verre (avec le premier déjà récup)
		#on part de la position où le robot a récupéré le premier verre
		#avance vers le verre suivant (distance = 300)
#On suppose que la position PINCE_HAUT permet de placer le verre à une hauteur de 90, soit 10 au-dessus des verres
		self.robot.asserv.sgoto(pos.x+300, pos.y, 100)	 	#permet d'aller au verre suivant
		self.robot.actionneurs.pince.drop()	#lâche le verre
		self.robot.actionneurs.pince.pos(PINCE_MED)	#descend la pince pour pouvoir récupérer les 2 verres
		self.robot.actionneurs.pince.pos(PINCE_HAUT)
		
	def recupRetour(self):
		#permet de retourner dans notre zone de jeu (avec ou sans verre)
		
		#tourne de 180°
		self.robot.asserv.turn_r(180, 75)	#angle, vitesse
		#!!! régler la vitesse pour tourner doucement, histoire de ne pas perdre de verres
		self.robot.asserv.sgoto(50,pos.y, 100)	#retourne à l'air de jeu
		self.robot.actionneurs.pince.pos(PINCE_BAS)
		self.robot.actionneurs.pince.drop()
	
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


def get_actions_bigrobot(ia, robot, enemies):
	actions = []	

	#actions.append(ActionBouteille(ia, robot, enemies, ia.p((640, 2000 - R_BIGROBOT - 100))))
	#actions.append(ActionBouteille(ia, robot, enemies, (ia.x(1883), 2000 - R_BIGROBOT - 100)))
	#actions.append(ActionLingo(ia, robot, enemies, ia.p((400, 900))))
	#actions.append(ActionTotemHaut(ia, robot, enemies, ia.p((1400,1000-125-R_BIGROBOT-40))))
	actions.append(ActionTotemBas(ia, robot, enemies, ia.p((1400,1000+125+R_BIGROBOT+40))))
	#actions.append(ActionFinalize(ia, robot, enemies, ia.p((400, 950))))
	#actions.append(ActionTotem3(ia, robot, enemies, ia.p((2200,1000-125-R_BIGROBOT-60)), ActionTotem.DIRECTION_HAUT))
	#actions.append(ActionTotem3(ia, robot, enemies, ia.p((2200,1000+125+R_BIGROBOT+60)), ActionTotem.DIRECTION_BAS))

	return actions

def get_actions_minirobot(ia, robot, enemies):
	actions = []
	# bouteilles
	"""actions.append(ActionBouteille(robot, enemies, (640, 2000 - R_MINIROBOT - 10)))
	actions.append(ActionBouteille(robot, enemies, (1883, 2000 - R_MINIROBOT - 10)))
	actions.append(ActionCarte(robot, enemies, (1500, R_MINIROBOT + 10)))"""

	return actions
