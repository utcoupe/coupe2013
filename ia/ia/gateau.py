""" ensemble de fonctions qui permettent la gestion du gâteau :
- matching des données depuis la visio (pour savoir où sont nos bougies)
- liste des bougies qu'il nous reste à taper
- une fonction qui permet de savoir quelles bougies le robot peut taper depuis sa position"""

DONE = 1
BLUE = 10
RED = 20

L_BRAS_PETIT = 100
L_BRAS_GRAND = 215
D_CENTRE_BRAS_GAUCHE = 21
D_CENTRE_BRAS_MILIEU = 14
D_CENTRE_BRAS_DROIT = 64
LARGEUR_BIGROBOT = 308

HAUT = 0
BAS = 1
#liste de la position des bougies
pos_bougie = []
#bougies du haut, de gauche à droite (de bleu vers rouge), 8 bougies
pos_bougie.append((1, 1157, 68))
pos_bougie.append((2, 1209, 194))
pos_bougie.append((3, 1306, 291))
pos_bougie.append((4, 1432, 343))
pos_bougie.append((5, 1568, 343))
pos_bougie.append((6, 1694, 291))
pos_bougie.append((7, 1791, 194))
pos_bougie.append((8, 1843, 68))
#bougies du bas, de gauche à droite (de bleu vers rouge), 12 bougies
pos_bougie.append((9, 1054, 59))
pos_bougie.append((10, 1084, 172))
pos_bougie.append((11, 1143, 274))	
pos_bougie.append((12, 1226, 357))
pos_bougie.append((13, 1328, 416))
pos_bougie.append((14, 1441, 446))
pos_bougie.append((15, 1559, 446))
pos_bougie.append((16, 1672, 416))
pos_bougie.append((17, 1774, 357))
pos_bougie.append((18, 1857, 274))
pos_bougie.append((19, 1916, 172))
pos_bougie.append((20, 1946, 59))

import utcoupe
import math

class InvalidRangee(Exception):
	"""1 ou 2"""
	pass

class InvalidColonne(Exception):
	"""de 1 à 8 pour HAUT, de 1 à 12 pour BAS"""
	pass

class Gateau:
	def __init__(self):
		self.bougies = [-1,] * 20
		self.done = [False,] * 20

	def merge_data_str(self, string):
		"""Ajoute les données obtenues par la visio"""
		data = [int(x) for x in string.split(',')]

		for i in range(20):
			if self.bougies[i] == -1:
				if data[i] % 2 == 1:
					self.done[i] = False
					self.bougies[i] = data[i] - 1
				else:
					self.done[i] = True
					self.bougies[i] = data[i]

	def merge_data_simu(self):
	#à utiliser pour le simu uniquement car la visio n'y est pas active
		"""self.bougies[0] = 10
		self.bougies[1] = 20
		self.bougies[2] = 10
		self.bougies[3] = 20
		self.bougies[4] = 20
		self.bougies[5] = 10
		self.bougies[6] = 20
		self.bougies[7] = 10
		self.bougies[8] = 10
		self.bougies[9] = 20
		self.bougies[10] = 20
		self.bougies[11] = 10
		self.bougies[12] = 10
		self.bougies[13] = 10
		self.bougies[14] = 10
		self.bougies[15] = 10
		self.bougies[16] = 20
		self.bougies[17] = 10
		self.bougies[18] = 10
		self.bougies[19] = 20"""

		self.bougies[0] = 10
		self.bougies[1] = 10
		self.bougies[2] = 10
		self.bougies[3] = 10
		self.bougies[4] = 10
		self.bougies[5] = 10
		self.bougies[6] = 10
		self.bougies[7] = 10
		self.bougies[8] = 10
		self.bougies[9] = 10
		self.bougies[10] = 10
		self.bougies[11] = 10
		self.bougies[12] = 10
		self.bougies[13] = 10
		self.bougies[14] = 10
		self.bougies[15] = 10
		self.bougies[16] = 10
		self.bougies[17] = 10
		self.bougies[18] = 10
		self.bougies[19] = 10

	def set_done(self, index):
		"""Marque une bougie comme enfoncée"""
		self.done[index] = True

	def get_left_todo(self, color):
		"""Renvoie la liste des index des bougies non encore tapées de la couleur color"""
		if color == utcoupe.BLUE:
			color = 10
		else:
			color = 20
		todos = []
		#a supprimer pour le cas réel, juste pour tester le simu
		self.merge_data_simu()
		for index, done in enumerate(self.done):
			if not(done) and self.bougies[index] == color:
				todos.append(index)
		return todos

	def get_todo_pos(self, color):
		"""retourne un tableau avec l'ensemble des bougies de la couleur avec leurs positions"""
		todo = []
		todo_pos = []
		todo = self.get_left_todo(color)
		for i in range(len(todo)):
			todo_pos.append((pos_bougie[todo[i]]))
		return todo_pos

	def get_index(self, rangee, colonne):
		"""Retourn l'index à partir de rangee (HAUT ou BAS) et colonne"""
		if (rangee > 1):
			raise InvalidRangee(rangee)
		if (rangee == HAUT and colonne > 8) or (rangee == BAS and colonne > 12):
			raise InvalidColonne(colonne)

		return (rangee * 8) + colonne - 1

	def get_info_from_index(self, index):
		"""Retourne (rangee, colonne) à partir d'un index"""
		if index <= 7:
			return (HAUT, index + 1)
		else:
			return (BAS, index - 7)
	
	def bougie_a_taper (self, pos, angle, color):
		"""Permet de retourner les bougies que le robot peut taper suivant sa position actuelle"""
		"""Cela passe par la création de 3 points qui correspondent au centre de l'extrémité de chacun des 3 bras
		du robot. On regarde si ce point est proche de l'une des 20 bougies (pour le moment on test toutes les bougies)
		si c'est le cas la méthode renvoit quel bras est concerné afin de notifier à l'IA d'activer le bras correspondant"""
		print("position du robot")
		print(pos[0])
		print(pos[1])
		print(angle)
		print("\nliste des bougies de la couleur :\n")
		bougie_color = []
		bougie_color = self.get_todo_pos(color)
		print(bougie_color)
		if (pos[0] < 1500): #si on est à gauche du gâteau, test effectué à cause des signes
			print("x inf à 1500")
			bras_gauche = (pos[0] - D_CENTRE_BRAS_GAUCHE*math.cos(math.radians(angle)) + (L_BRAS_PETIT + LARGEUR_BIGROBOT/2)*math.sin(math.radians(angle)), pos[1] - D_CENTRE_BRAS_GAUCHE*math.sin(math.radians(angle)) - (L_BRAS_PETIT + LARGEUR_BIGROBOT/2)*math.cos(math.radians(angle)))
			print("bras gauche")
			print(bras_gauche)
			bras_milieu = (pos[0] + D_CENTRE_BRAS_MILIEU*math.cos(math.radians(angle)) + (L_BRAS_GRAND + LARGEUR_BIGROBOT/2)*math.sin(math.radians(angle)), pos[1] + D_CENTRE_BRAS_MILIEU*math.sin(math.radians(angle)) - (L_BRAS_GRAND + LARGEUR_BIGROBOT/2)*math.cos(math.radians(angle)))
			print("bras milieu")
			print(bras_milieu)
			bras_droit = (pos[0] + D_CENTRE_BRAS_DROIT*math.cos(math.radians(angle)) + (L_BRAS_PETIT + LARGEUR_BIGROBOT/2)*math.sin(math.radians(angle)), pos[1] + D_CENTRE_BRAS_DROIT*math.sin(math.radians(angle)) - (L_BRAS_PETIT + LARGEUR_BIGROBOT/2)*math.cos(math.radians(angle)))
			print("bras droit")
			print(bras_droit)
		else:
			print("x sup à 1500")
			bras_gauche = (pos[0] - D_CENTRE_BRAS_GAUCHE*math.cos(math.radians(angle)) - (L_BRAS_PETIT + LARGEUR_BIGROBOT/2)*math.sin(math.radians(angle)), pos[1] + D_CENTRE_BRAS_GAUCHE*math.sin(math.radians(angle)) - (L_BRAS_PETIT + LARGEUR_BIGROBOT/2)*math.cos(math.radians(angle)))
			print("bras gauche")
			print(bras_gauche)
			bras_milieu = (pos[0] + D_CENTRE_BRAS_MILIEU*math.cos(math.radians(angle)) - (L_BRAS_GRAND + LARGEUR_BIGROBOT/2)*math.sin(math.radians(angle)), pos[1] - D_CENTRE_BRAS_MILIEU*math.sin(math.radians(angle)) - (L_BRAS_GRAND + LARGEUR_BIGROBOT/2)*math.cos(math.radians(angle)))
			print("bras milieu")
			print(bras_milieu)
			bras_droit = (pos[0] + D_CENTRE_BRAS_DROIT*math.cos(math.radians(angle)) - (L_BRAS_PETIT + LARGEUR_BIGROBOT/2)*math.sin(math.radians(angle)), pos[1] - D_CENTRE_BRAS_DROIT*math.sin(math.radians(angle)) - (L_BRAS_PETIT + LARGEUR_BIGROBOT/2)*math.cos(math.radians(angle)))
			print("bras droit")
			print(bras_droit)
		
		a_taper = []
		"""boucle qui parcourt toutes les bougies et ajoute dans une liste s'il y a une ou plusieurs bougies
		que le robot est en mesure de taper, en fonction de sa position actuelle et de son angle"""
		for i in range(len(bougie_color)):
			print("Bougie x : ")
			print(bougie_color[i][1])
			print("Bougie y : ")
			print(bougie_color[i][2])
			if ((bougie_color[i][1]-40) < bras_gauche[0] < (bougie_color[i][1])+40):
				print("ok gauche")
				if ((bougie_color[i][2]-40) < bras_gauche[1] < (bougie_color[i][2])+40):
					print("gauche")
					self.set_done(bougie_color[i][0]-1)
					print(bougie_color[i][0])
					a_taper.append(("gauche",(bougie_color[i][1], bougie_color[i][2])))
			if ((bougie_color[i][1]-40) < bras_milieu[0] < (bougie_color[i][1])+40):
				print("ok milieu")
				if ((bougie_color[i][2]-40) < bras_milieu[1] < (bougie_color[i][2])+40):
					print("milieu")
					print(bougie_color[i][0])
					self.set_done(bougie_color[i][0]-1)
					a_taper.append(("milieu",(bougie_color[i][1], bougie_color[i][2])))
			if ((bougie_color[i][1]-40) < bras_droit[0] < (bougie_color[i][1])+40):
				print("ok droit")
				if ((bougie_color[i][2]-40) < bras_droit[1] < (bougie_color[i][2])+40):
					print("droit")
					print(bougie_color[i][0])
					self.set_done(bougie_color[i][0]-1)
					a_taper.append(("droit",(bougie_color[i][1], bougie_color[i][2])))
		a_taper.append(("test", (500,500)))
		return a_taper
