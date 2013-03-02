# -*- coding: utf-8 -*-


from ..define import *
from ..engine.engineobject import EngineObjectCircle


class Verre(EngineObjectCircle):
	def __init__(self,engine,posinit, color = "black"):
		EngineObjectCircle.__init__(self,
			engine			= engine,
			colltype		= COLLTYPE_VERRE,
			posinit			= posinit,
			color			= color,
			mass			= 60,
			radius			= mm_to_px(40),
		)

	def __repr__(self):
		return "Verre %s sur l'etage %d" % (self.posinit, self.etage)
