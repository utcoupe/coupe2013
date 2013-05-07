# -*- coding: utf-8 -*-

import math
import time
import random

from geometry import Vec,Segment

from .robot import Robot, BIG, Others, Visio
from ..define import *
from ..engine.engineobject import EngineObjectPoly
from .bougie import Bougie
from .cerise import Cerise
from .ping_pong import Ping_pong

class BigRobot(Robot):
	def __init__(self, *, engine, asserv, others, visio, posinit, team, match, services):
		self.bras_milieu = EngineObjectPoly(
			engine 		= engine,
			colltype	= COLLTYPE_BRAS,
			offset		= mm_to_px(4, -154),
			color		= "purple",
			poly_points = map(lambda p: mm_to_px(*p),[(0,0),(20,0),(20,-215),(0,-215)]), #taille du bras
			is_extension= True
		)

		self.bras_gauche = EngineObjectPoly(
			engine 		= engine,
			colltype	= COLLTYPE_BRAS,
			offset		= mm_to_px(-41, -154),
			color		= "purple",
			poly_points = map(lambda p: mm_to_px(*p),[(0,0),(20,0),(20,-100),(0,-100)]), #taille du bras
			is_extension= True
		)

		self.bras_droit = EngineObjectPoly(
			engine 		= engine,
			colltype	= COLLTYPE_BRAS,
			offset		= mm_to_px(54, -154),
			color		= "purple",
			poly_points = map(lambda p: mm_to_px(*p),[(0,0),(20,0),(20,-100),(0,-100)]), #taille du bras
			is_extension= True
		)

		Robot.__init__(self,
			engine		 		= engine,
			asserv		= asserv,
                        asserv_obj = None,
			others		= others,
                        others_obj = BigOthers(self),
			visio			= visio,
                        visio_obj = BigVisio(self),
			team				= team,
			posinit				= posinit,
			mass				= 10,
			typerobot			= BIG,
			colltype 			= COLLTYPE_GROS_ROBOT,
			poly_points			= mm_to_px((0,0),(HEIGHT_GROS,0),(HEIGHT_GROS,WIDTH_GROS),(0,WIDTH_GROS)),
			extension_objects	= [],
                        match = match,
                        services = services
		)

		self.nb_bougie 		= 0
		self.nb_cerise_pourri 	= 0
		self.nb_cerise_bonne 	= 0
		self.state_bras			= False

	def remove_bras(self):
		self.remove_body_extension(self.bras_milieu)
		self.remove_body_extension(self.bras_gauche)
		self.remove_body_extension(self.bras_droit)

	def add_bras(self):
		self.add_body_extension(self.bras_milieu)
		self.add_body_extension(self.bras_gauche)
		self.add_body_extension(self.bras_droit)

	def onEvent(self, event):
		if not Robot.onEvent(self,event):
			if self._event_concerns_me(event):
				if KEYDOWN == event.type:
					if KEY_DROP == event.key:
						self._cmd_others_drop(id_msg=42)
						return True
					elif KEY_BRAS == event.key:
						self.state_bras = not self.state_bras
						self._cmd_others_bras(self.state_bras)

	def eteindre_bougie(self, bougie):
		# if bougie.color == 'gray':
		# 	return
		if (bougie.color == 'red' and self.team == RED) or \
			(bougie.color == 'blue' and self.team == BLUE) or (bougie.color == 'white'):
			self.nb_bougie += 1
		else:
			self.nb_bougie -= 1
		if bougie not in self.engine.objects_to_remove:
			self.engine.objects_to_remove.append(bougie)
		print("Bougie eteinte: %d" % self.nb_bougie)

	def prendre_cerise(self, cerise):
		if not self.is_full():
			self.nb_cerise_bonne 	+= 7	#on a 7 bonnes cerises par assiettes
			self.nb_cerise_pourri 	+= 1
			cerise.color = 'gray'
			print("bonnes cerises: %d mauvaises cerises: %d" % (self.nb_cerise_bonne, self.nb_cerise_pourri))

	def _cmd_others_drop(self, **kwargs):
		# calcul de la position d'attérissage (un peu derrière le robot
		# DIST = 200
		# pos = Vec(self.pos())
		# angle = self.angle() + math.pi
		# pos_drop = pos + mm_to_px(random.randint(-50,50),random.randint(-50,50)) + mm_to_px(DIST * math.cos(angle), DIST * math.sin(angle))
		# print("pos_drop= ", pos_drop)
		# création des objets
		for _ in range(self.nb_cerise_bonne):
			pos_drop = 	mm_to_px(1449 + random.randint(-25, 25), 51 + random.randint(-25, 25)) \
						if self.team == BLUE \
						else mm_to_px(1551 + random.randint(-25, 25), 51 + random.randint(-25, 25))
			pos_drop = tuple(pos_drop)
			pingpong = Ping_pong(self.engine, pos_drop, self.team)
			self.engine.add(pingpong)

		self.nb_cerise_bonne = 0
		self.nb_cerise_pourri = 0

		self.asserv_service.send_response(kwargs['id_msg'], 1)
		
	
	# def _cmd_others_vider_totem(self, **kwargs):
	# 	self.nb_white_cds += 4
	# 	self.nb_lingos += 1
	# 	
	def is_full(self):
		return COEFF_ENGORGEMENT_CERISE * (self.nb_cerise_bonne + self.nb_cerise_pourri) > 1

	def _cmd_others_is_full(self, **kwargs):
		coeff_engorgement = (self.nb_cerise_bonne+self.nb_cerise_pourri) * COEFF_ENGORGEMENT_CERISE
		r = 0 if coeff_engorgement < 1 else 1
		self.send_canal_asserv(kwargs['id_msg'], r)

	def _cmd_others_bras(self, on):
		if on:
			self.add_bras()
		else:
			self.remove_bras()


class BigOthers(Others):
	pass

class BigVisio(Others):
	def get_by_color(self, uid):
		return "21, 11, 21, 11, 21, 11, 21, 11, 21, 11, 21, 11, 21, 11, 21, 11, 21, 11, 21, 11"
