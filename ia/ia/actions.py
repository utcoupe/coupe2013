""" Liste des classes d'action. Lorsque le robot arrive au point d'accès d'une action, celle-ci se déclenche et
effectue le traitement prévu dans la classe action correspondante"""
import sys
sys.path.append('../../lib')

from .action import *

import utcoupe

import time
import math


#définition des constantes des rayons
R_ASSIETTE = 120 # 170/2*sqrt(2)
R_VERRE = 40
RAYON_BIGROBOT = 230

#point_acces = quand le robot est arrivé à ce point, il déclenche l'action

class ActionCadeau(Action):
	def __init__(self, ia, robot, enemies, point_acces, priority):
		Action.__init__(self, ia, robot, enemies, point_acces, priority)
		
	def run(self):
		print("\nACTION CADEAU\n")
		self.robot.asserv.turn(90, 100)	#block et block_level à virer dans l'asserv,utilisé pour irc
		#permet d'être sûr que le robot est en face du cadeau avant d'aller le taper
		time.sleep(1)
		
		#avance
		self.robot.asserv.pwm(75,75,100)			# !!!!!! valeur à tester, mise totalement arbitrairement
		#commande pwm : puissance roue droite, puissance roue gauche, temps
		#doit avancer de 100mm (car point_acces le position à 100+rayon_minirobot de l'action
		time.sleep(1)
		
		#recul
		self.robot.asserv.pwm(-75,-75,100)			# !!!!!! valeur à tester, mise totalement arbitrairement
		
		#fini
		self.clean()

		print("Cadeau headshot !\n")
	
	def __repr__(self):	#surcharge du print
		return "ActionCadeau(%s, %s)" % (self.point_acces, self.score)
		
		

class ActionGetCerise(Action):
		
	DIRECTION_RIGHT		= 0	# il faudra tourner de 90° à droite
	DIRECTION_LEFT		= 1 # il faudra tourner de 90° à gauche (-90°)
	DIRECTION_FRONT		= 2 # pas besoin de tourner
	
	def __init__(self, ia, robot, enemies, point_acces, direction, priority):
		Action.__init__(self, ia, robot, enemies, point_acces, priority)
		self.direction = direction
		
	def run(self):
		print("\nACTION RECUPERER CERISE\n")
		#self.robot.asserv.turn(-90, 75)
		time.sleep(1)
		
		if self.direction != self.DIRECTION_FRONT:
			# le robot se tourne face à l'assiette
			angle = 90 if self.direction==self.DIRECTION_LEFT else -90
			self.robot.asserv.turn(angle, 75)
		
		#avance
		self.robot.asserv.pwm(50,50,175)			# !!!!!! valeur à tester, mise totalement arbitrairement		
		time.sleep(1)
		#self.robot.actionneurs.cerise.get()
		
		#recul
		self.robot.asserv.pwm(-50,-50,175)			# !!!!!! valeur à tester, mise totalement arbitrairement
		
		#fini
		self.clean()

		print("Cerise done !\n")
	
	def __repr__(self):	#surcharge du print
		return "ActionGetCerise(%s, %s)" % (self.point_acces, self.score)
		
		
class ActionVerre(Action): #ouvrir les pinces au départ
	"""En mode buldo, on garde les pinces ouvertes, on se rend à la position et on avance toujours tout droit pour retrouner
	à la zone de départ (et normalement le robot devrait récup 2 verres"""
	def __init__(self, ia, robot, enemies, point_acces, priority, pos_final):
		Action.__init__(self, ia, robot, enemies, point_acces, priority)
		self.pos_final = pos_final;
		
	def run(self):
		print("\nACTION VERRE\n")
		#self.robot.asserv.turn(180,100)
		if self.robot.pos[1] < 1000:
			self.robot.asserv.goto(self.robot.pos[0],self.robot.pos[1]+300,100)	#cherche le verre suivant
		else:
			self.robot.asserv.goto(self.robot.pos[0],self.robot.pos[1]-300,100)	#cherche le verre suivant
		#time.sleep(1)
		self.robot.asserv.goto(self.pos_final[0], self.pos_final[1], 150)	#retourne à l'air de jeu
		
		#fini
		self.clean()
		
	def __repr__(self):	#surcharge du print
		return "ActionVerre(%s, %s)" % (self.point_acces, self.score)
		
#implémenter l'essaye de récupération des verres ennemis si jamais on a le temps

class ActionShootCerise(Action):
	
	def __init__(self, ia, robot, enemies, point_acces, priority):
		Action.__init__(self, ia, robot, enemies, point_acces, priority)

	def run(self):
		
		#active le tir de cerises
		self.robot.actionneurs.cerise.shoot() 
		
		#fini
		self.clean()

		print("Shoot de cerise done !")
		
	def __repr__(self):
		return "ActionShootCerise(%s, %s)" % (self.point_acces, self.score)	
		
class ActionBougie(Action):
		
	def __init__(self, ia, robot, enemies, point_acces, priority):
		Action.__init__(self, ia, robot, enemies, point_acces, priority)
			
# EXPLICATION
# color_bougie est composé comme suit : bras_gauche bras_haut bras_droite
# Pour chacun, on a 1 si on veut activer le bras et 0 sinon
# Donc pour activer le bras gauche et le bras droite, on envoit : color_bougie = 101
# Les if permettent d'initialiser les valeurs depuis color_bougie

	def run(self):
		
		#tape les bougies (en foncion de la couleur)
		#self.robot.actionneurs.bougie(bras_gauche, bras_haut, bras_droite)	

		angle = math.atan(abs(1500-self.robot.pos[0])/self.robot.pos[1])*180/math.pi
		print(self.robot.pos)
		if (self.robot.pos[0]<1500):
			self.robot.asserv.turn(angle,100)
		elif (self.robot.pos[0]>1500):
			self.robot.asserv.turn(angle * -1,100)
		#pour les tests

		#test pour le coin du gâteau
		#self.robot.asserv.pwm(-15,-15,100)

		a_taper = self.ia.gamestate.gateau.bougie_a_taper(self.robot.pos, angle, self.ia.team)
		#test
		if (a_taper):
			print("il a une liste")
			print(a_taper)
		else:
			print("il a pas de liste")
		#lance l'action qui appel les actionneurs pour taper
		for i in range(len(a_taper)):
			if a_taper[i][0] == 'gauche':
				self.robot.actionneurs.goto_ax12(1,-150,110)
				self.robot.actionneurs.goto_ax12(1,-93,110)
			elif a_taper[i][0] == 'milieu':
				self.robot.actionneurs.goto_ax12(2,80,110)
				self.robot.actionneurs.goto_ax12(2,93,110)
			elif a_taper[i][0] == 'droit':
				self.robot.actionneurs.goto_ax12(0,100,110)
				self.robot.actionneurs.goto_ax12(0,150,110)
			print("on taper avec le bras")
			print(a_taper[i][0])

		#fini
		time.sleep(1)
		self.clean()

		print("Bougie soufflée !")
		
	def __repr__(self):
		return "ActionBougie(%s, %s)" % (self.point_acces, self.score)	

class ActionTakePicture(Action):
	"""Classe d'action basique qui déclenche la prise d'une photo pour la visio
	afin de déterminer la position des bougies de notre couleur (traitement fait dans gateau.py)"""
	
	def __init__(self, ia, robot, enemies, point_acces, priority):
		Action.__init__(self, ia, robot, enemies, point_acces, priority)

	def run(self):
		#active la prise de photo
		self.ia.gamestate.take_picture() 
		#mettre un ajout des actions bougies ici
		
		#fini
		self.clean()

		print("Shoot de cerise done !")
		
	def __repr__(self):
		return "ActionTakePicture(%s, %s)" % (self.point_acces, self.score)	

class ActionHomologation(Action):
	def __init__(self, ia, robot, enemies, point_acces, priority):
		Action.__init__(self, ia, robot, enemies, point_acces, priority)

	def run(self):
		#active la prise de photo
		"""self.robot.asserv.goto(1450,700,100)
		self.robot.asserv.goto(1450,990,100)
		self.robot.asserv.goto(900,950,200)
		self.robot.asserv.goto(350,750,200)"""
		self.robot.asserv.goto(self.robot.pos[0],self.robot.pos[1],100)

		
		#fini
		self.clean()

		print("Shoot de cerise done !")
		
	def __repr__(self):
		return "ActionTakePicture(%s, %s)" % (self.point_acces, self.score)


#définition des positions des items suivant leurs couleurs (afin d'alléger les get_actions)

#def position_verre(team):
verre_nous = []	#tableau listant la positions de nos verres
verre_ennemis = []	#tableau listant la position des verres ennemis
"""On part du principe qu'en mode buldo, on va pour choper le premier verre (celui le plus éloigné)
et qu'on revient vers notre zone de départ et qu'on chope le deuxième au passage (donc ça fait seulement 3 positions"""
verre_nous.append((1200 + R_VERRE + 20,950)) #extremité verre 1
verre_nous.append((1350 + R_VERRE + 20,1200))#extremité verre 2
verre_nous.append((1200 + R_VERRE + 20,1450))#extremité verre 3
verre_ennemis.append((2100,950))
verre_ennemis.append((1800,950))
verre_ennemis.append((1950,1200))
verre_ennemis.append((1650,1200))
verre_ennemis.append((2100,1450))
verre_ennemis.append((1800,1450))
	
assiette_nous = []	
#tableau listant la positions de nos assiettes
assiette_nous.append((225,600)) #10 de marge
assiette_nous.append((225,600)) #orienté vers le bas
assiette_nous.append((225 + R_ASSIETTE + 20,1750)) #sur x car on chope l'assiette de côté
		
#def position_cadeau(team):
cadeau_nous = []
cadeau_nous.append((490,2000 - 200)) #100 de marge
cadeau_nous.append((1090,2000 - 200))
cadeau_nous.append((1690,2000 - 200))
cadeau_nous.append((2290,2000 - 200))

def get_actions_bigrobot(ia, robot, enemies):
	actions = []	
	#for i in range(0,3):
	#actions.append(ActionGetCerise(ia, robot, enemies, assiette_nous[0], ActionGetCerise.DIRECTION_RIGHT))
	#actions.append(ActionGetCerise(ia, robot, enemies, assiette_nous[1], ActionGetCerise.DIRECTION_LEFT))
	#actions.append(ActionShootCerise(ia, robot, enemies, ia.p((1300,800))))#valeur mise au hasard
	#actions.append(ActionBougie(ia, robot, enemies, (1000,550), 1))

	actions.append(ActionHomologation(ia, robot, enemies, (400,600), 1))
	"""actions.append(ActionVerre(ia, robot, enemies, (1400,700), 1, (350,730)))
	actions.append(ActionVerre(ia, robot, enemies, (1550,850), 3, (350,1300)))
	actions.append(ActionVerre(ia, robot, enemies, (1410,1700), 2, (350,1450)))
	actions.append(ActionCadeau(ia, robot, enemies, (450,1750), 2))
	actions.append(ActionCadeau(ia, robot, enemies, (450,1750), 1))
	actions.append(ActionCadeau(ia, robot, enemies, (450,1750), 1))
	actions.append(ActionCadeau(ia, robot, enemies, (450,1750), 1))
	actions.append(ActionBougie(ia, robot, enemies, (1312,680), 5))
	actions.append(ActionBougie(ia, robot, enemies, (1660,676), 1))
	actions.append(ActionBougie(ia, robot, enemies, (1450,680), 2))
	actions.append(ActionBougie(ia, robot, enemies, (818,172), 1))
	actions.append(ActionBougie(ia, robot, enemies, (900,360), 4))
	actions.append(ActionBougie(ia, robot, enemies, (1105,550), 1))
	actions.append(ActionBougie(ia, robot, enemies, (1490,705), 3))
	actions.append(ActionBougie(ia, robot, enemies, (1850,615), 1))
	actions.append(ActionBougie(ia, robot, enemies, (1990,510), 2))
	actions.append(ActionBougie(ia, robot, enemies, (2105,355), 5))
	actions.append(ActionBougie(ia, robot, enemies, (2175,180), 1))"""

	return actions

def get_actions_minirobot(ia, robot, enemies):
	actions = []
	"""for i in range(0,4):
		actions.append(ActionCadeau(ia, robot, enemies, cadeau_nous[i], 2))
	#print("Après ajout de l'action")
	print(actions)
	for i in range(0,3):
		actions.append(ActionVerre(ia, robot, enemies, verre_nous[i], 1))"""
	"""actions.append(ActionBougie(ia, robot, enemies, (2050,370), 2))
	actions.append(ActionBougie(ia, robot, enemies, (1300,600), 2))
	actions.append(ActionBougie(ia, robot, enemies, (1550,680), 2))
	actions.append(ActionBougie(ia, robot, enemies, (1450,680), 2))
	actions.append(ActionBougie(ia, robot, enemies, (850,150), 2))
	actions.append(ActionBougie(ia, robot, enemies, (950,370), 2))
	actions.append(ActionBougie(ia, robot, enemies, (1300,600), 1))
	actions.append(ActionBougie(ia, robot, enemies, (1450,680), 1))
	actions.append(ActionBougie(ia, robot, enemies, (1550,680), 1))
	actions.append(ActionBougie(ia, robot, enemies, (1900,550), 1))
	actions.append(ActionBougie(ia, robot, enemies, (2050,370), 1))
	actions.append(ActionBougie(ia, robot, enemies, (2150,150), 1))
	actions.append(ActionBougie(ia, robot, enemies, (850,150), 1))"""
	return actions
