# -*- coding: utf-8 -*-
# cela correspond en fait à 1 assiette de cerise pleine, et pas aux cerises de façon individuelle

from ..define import *
from ..engine.engineobject import EngineObjectPoly


class Cerise(EngineObjectPoly):
	def __init__(self,engine,posinit):
		EngineObjectPoly.__init__(self,
			engine			= engine,
			colltype		= COLLTYPE_CERISE,
			posinit			= posinit,
			color			= "purple",
			mass			= 380,
			poly_points		= map(lambda p: mm_to_px(*p),[(0,0),(170,0),(170,170),(0,170)])
		)

	def __repr__(self):
		return "Cerises %s " % (self.posinit,)
