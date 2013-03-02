# -*- coding: utf-8 -*-

import math
import time
import random

from geometry import Vec,Segment

from .robot import Robot, BIG
from ..define import *
from ..engine.engineobject import EngineObjectPoly
from .bougie import Bougie
from .cerise import Cerise

class BigRobot(Robot):
	def __init__(self, *, engine, canal_asserv, canal_others, canal_visio, canal_extras, posinit, team):
		self.bras = EngineObjectPoly(
			engine 		= engine,
			colltype	= COLLTYPE_BRAS,
			offset		= mm_to_px(6,-137),
			color		= "red",
			poly_points = map(lambda p: mm_to_px(*p),[(0,0),(200,0),(200,20),(0,20)]), #taille du bras
			is_extension= True
		)

		self.rouleau = EngineObjectPoly(
			engine 		= engine,
			colltype	= COLLTYPE_ROULEAU,
			offset		= mm_to_px(6,0),
			color		= "orange",
			poly_points = map(lambda p: mm_to_px(*p),[(20,0),(118,0),(118,134),(20,134)]),
			is_extension= True
		)

		Robot.__init__(self,
			engine		 		= engine,
			canal_asserv		= canal_asserv,
			canal_others		= canal_others,
			canal_visio			= canal_visio, 
			canal_extras		= canal_extras,
			team				= team,
			posinit				= posinit,
			mass				= 10,
			typerobot			= BIG,
			poly_points			= mm_to_px((0,0),(288,0),(288,314),(0,314)),
			extension_objects	= [self.bras, self.rouleau]
		)

		self.nb_bougie 		= 0
		self.nb_cerise_pourri 	= 0
		self.nb_cerise_bonne 	= 0

	def onEvent(self, event):
		if not Robot.onEvent(self,event):
			if self._event_concerns_me(event):
				if KEYDOWN == event.type:
					if KEY_DROP == event.key:
						self._cmd_others_drop(id_msg=42)
						return True

	def eteindre_bougie(self, bougie):
		if bougie.color == 'gray':
			return
		if (bougie.color == 'red' and self.team == RED) or \
			(bougie.color == 'blue' and self.team == BLUE) or (bougie.color == 'white'):
			self.nb_bougie += 1
		else:
			self.nb_bougie -= 1
		bougie.color = 'gray'
		print("Bougie eteinte: %d" % self.nb_bougie)

	def prendre_cerise(self, cerise):
		self.nb_cerise_bonne 	+= 7	#on a 7 bonnes cerises par assiettes
		self.nb_cerise_pourri 	+= 1
		cerise.color = 'gray'
		print("bonnes cerises: %d mauvaises cerises: %d" % (self.nb_cerise_bonne, self.nb_cerise_pourri))
		
#à supprimer car inutile pour la version 2013

	def _cmd_others_drop(self, **kwargs):
		DIST = 200
		# calcul de la position d'attérissage (un peu derrière le robot
		pos = Vec(self.pos())
		angle = self.angle() + math.pi
		pos_drop = pos + mm_to_px(random.randint(-100,100),random.randint(-100,100)) + mm_to_px(DIST * math.cos(angle), DIST * math.sin(angle))
		pos_drop = tuple(pos_drop)
		# création des objets
		for _ in range(self.nb_cerise_bonne):
			cd = Cd(self.engine, pos_drop, "white")
			self.engine.add(cd)
		for _ in range(self.nb_cerise_pourri):
			cd = Cd(self.engine, pos, "black")
			self.engine.add(cd)
		
		self.nb_cerise_bonne = 0
		self.nb_cerise_pourri = 0

		self.send_canal_asserv(kwargs['id_msg'], 1)
		
	
	# def _cmd_others_vider_totem(self, **kwargs):
	# 	self.nb_white_cds += 4
	# 	self.nb_lingos += 1

	def _cmd_others_is_full(self, **kwargs):
		coeff_engorgement = (self.nb_cerise_bonne+self.nb_cerise_pourri) * COEFF_ENGORGEMENT_CERISE
		r = 0 if coeff_engorgement < 1 else 1
		self.send_canal_asserv(kwargs['id_msg'], r)

