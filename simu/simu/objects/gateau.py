# -*- coding: utf-8 -*-

from ..define import *
from ..engine.engineobject import EngineObjectCircle


class Gateau(EngineObjectCircle):
	def __init__(self,engine,posinit):
		EngineObjectCircle.__init__(self,
			engine			= engine,
			colltype		= COLLTYPE_WALL,
			posinit			= posinit,
			color			=  "black",
			radius			= mm_to_px(500)
		)

	def __repr__(self):
		return "wall gateau"