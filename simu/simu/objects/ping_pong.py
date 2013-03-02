# -*- coding: utf-8 -*-

from ..define import *
from ..engine.engineobject import EngineObjectCircle


class Ping_pong(EngineObjectCircle):
	def __init__(self,engine,posinit, team):
		EngineObjectCircle.__init__(self,
			engine			= engine,
			colltype		= COLLTYPE_DEFAULT,
			posinit			= posinit,
			color 			= "orange" if team == RED else "green",
			mass 			= 800,
			radius			= mm_to_px(11)
		)

	def __repr__(self):
		return "pingpong %s" % (self.posinit)
