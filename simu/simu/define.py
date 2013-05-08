# -*- coding: utf-8 -*-

from pymunk import inf as MASS_INF
from pygame.locals import *



FPS					= 30
PX_TO_MM			= 4



COLLTYPE_DEFAULT		= 0
COLLTYPE_WALL			= 1
COLLTYPE_GROS_ROBOT		= 2
COLLTYPE_PETIT_ROBOT	= 3
COLLTYPE_BRAS			= 4
COLLTYPE_VERRE			= 5
COLLTYPE_BOUGIE			= 6
COLLTYPE_CERISE			= 7
COLLTYPE_CADEAU			= 8
COLLTYPE_ROULEAU		= 9
COLLTYPE_ROBOT   		= 10


BLUE				= 0
RED					= 1

T_VERRE				= 0
T_CERISE			= 1
T_BOUGIE			= 2
T_CADEAU			= 3
T_FUNNY				= 4

#coeff non modifiés (mais il faut surement ajuster celui pour CD == VERRE dans la simu2013)
#idem pour le coeff LINGO == CERISE
COEFF_ENGORGEMENT_CERISE= 0.05 # eq : on peut mettre 25 Cerises avant d'être plein (0.05 * 20 = 1)
# COEFF_ENGORGEMENT_LINGO	= 0.2
COEFF_ENGORGEMENT_VERRE = 0.34  # Pour le petit robot, il en prend 2 a la fois.



KEY_CHANGE_TEAM		= K_LSHIFT		# changer d'equipe
KEY_CHANGE_ROBOT	= K_LCTRL		# changer de robot
KEY_STOP_RESUME		= K_SPACE		# apppui = stop, relache = resume
KEY_CANCEL			= K_ESCAPE		# cancel
KEY_DROP			= K_d			# vider le gros robot
KEY_BRAS			= K_b			# add or remove bras
KEY_BALAIS_LEFT		= K_q			# sortir le balais de gauche
KEY_BALAIS_RIGHT	= K_w			# sortir le balais de droite
KEY_BULDO			= K_x			# mode buldozer
KEY_TELEPORTATION	= K_t			# mode téléportation
KEY_RECUL			= K_r			# mode marche arrière
KEY_JACK			= K_j			# jack
KEY_SHARP1			= K_k			# sharp1
KEY_SHARP2			= K_l			# sharp2
KEY_SHARP3			= K_i			# sharp3
KEY_SHARP4			= K_o			# sharp4

BIG			= 0
MINI		= 1

LEFT		= 0
RIGHT		= 1

# dimensions du petit robot
WIDTH_MINI 		= 180
HEIGHT_MINI 	= 102
# dimensions du gros robot
WIDTH_GROS 		= 308
HEIGHT_GROS 	= 192
# dimension des palettes (buldo) du mini robot
SIZE_PALETTE	= 120
WIDTH_PALETTE	= 10
# dimensions balais qui râclent la zone adverse	
SIZE_BALAIS		= 70
WIDTH_BALAIS	= 10
ECART_ROBOT_BALAIS	= 0

#ECART_CENTRE = -65 # ecart par rapport au centre du robot

def mm_to_px(*args):
	"""
	@param args une liste ou un atom de valeurs ou de positions
	@param la veleur ou la liste convertie

	>>> mm_to_px(4) == int(4/PX_TO_MM)
	True

	>>> mm_to_px((4,8)) == (mm_to_px(4),mm_to_px(8))
	True

	>>> mm_to_px((4,8),(16,32)) == ((mm_to_px(4),mm_to_px(8)), (mm_to_px(16),mm_to_px(32)))
	True

	>>> mm_to_px([(4,8),(16,32)]) == mm_to_px((4,8),(16,32))
	True

	>>> mm_to_px(*[(4,8),(16,32)]) == mm_to_px((4,8),(16,32))
	True
	
	"""
	def f(a):
		if isinstance(a,tuple) or isinstance(a,list):
			return mm_to_px(*a)
		else:
			return int(a / PX_TO_MM)
	if len(args) == 1:
		return f(args[0])
	else:
		return tuple(map(lambda v: f(v), args))

def px_to_mm(*args):
	"""
	@param args une liste ou un atom de valeurs ou de positions
	@param la valeur ou la liste convertie
	"""
	def f(a):
		if isinstance(a,tuple) or isinstance(a,list):
			return px_to_mm(*a)
		else:
			return int(a * PX_TO_MM)
	if len(args) == 1:
		return f(args[0])
	else:
		return tuple(map(lambda v: f(v), args))




if __name__ == "__main__":
	import doctest
	doctest.testmod()



