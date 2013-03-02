# -*- coding: utf-8 -*-


from ..define import *
from ..engine.engineobject import EngineObjectCircle


class Verre(EngineObjectCircle):
	def __init__(self,engine,posinit):
		EngineObjectCircle.__init__(self,
			engine			= engine,
			colltype		= COLLTYPE_VERRE,
			posinit			= posinit,
			color			=  "black",
			mass			= 60,
			radius			= mm_to_px(40)
		)

		self.etage = 0

	def monter_verre(self):
		self.etage += 1

	def decendre_verre(self):
		self.etage -= 1

	def __repr__(self):
		return "Verre %s sur l'etage %d" % (self.posinit, self.etage)
