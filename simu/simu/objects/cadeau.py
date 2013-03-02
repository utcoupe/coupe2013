# -*- coding: utf-8 -*-

from ..define import *
from ..engine.engineobject import EngineObjectPoly


class Cadeau(EngineObjectPoly):
	def __init__(self,engine,posinit,color):
		EngineObjectPoly.__init__(self,
			engine			= engine,
			colltype		= COLLTYPE_CADEAU,
			posinit			= posinit,
			mass			= MASS_INF,
			color			= color,
			poly_points		= map(lambda p: mm_to_px(*p),[(0,0),(150,0),(150,22),(0,22)])
		)

	def __repr__(self):
		return "Cadeaux %s " % (self.posinit,)
