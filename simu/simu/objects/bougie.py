# -*- coding: utf-8 -*-

from ..define import *
from ..engine.engineobject import EngineObjectCircle


class Bougie(EngineObjectCircle):
	def __init__(self,engine,posinit,color):
		EngineObjectCircle.__init__(self,
			engine			= engine,
			colltype		= COLLTYPE_BOUGIE,
			posinit			= posinit,
			color			= color,
			mass 			= MASS_INF,
			radius			= mm_to_px(40)
		)

	def __repr__(self):
		return "Bougies %s " % (self.posinit,)

	def eteindre(self):
		self.color = "gray"
