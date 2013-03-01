# -*- coding: utf-8 -*-


from geometry import ConvexPoly


from .define import *
from .objects import Verre

points = {
	T_VERRE		:4,
	T_CERISE	:2,
	T_BOUGIE	:4,
	T_CADEAU	:4,
	T_FUNNY		:12
}

class Match:
	def __init__(self):
		self.carte_arrache = [0,0]

	def init(self, engine):
		self.engine = engine

	def score(self, team):
		verres = self.get_verres(team)
		score  =  verres * points[T_VERRE]

		## TODO Complete score calculus
		return score  #, {'arrachercarte': self.carte_arrache[team], 'cds':cds, 'lingos':lingos}

	def get_verres(self, team):
		if team == BLUE:
			return self.get_verres_for_blue()
		elif team == RED:
			return self.get_verres_for_red()
		else:
			raise Exception("team inconnue")
	
	def get_verres_for_blue(self):
		poly = ConvexPoly(mm_to_px(
			(0,0),
			(400,0),
			(400,2000),
			(0,2000)
		))
		verres = self._get_verres_in_poly(poly)
		return verres
		
	def get_verres_for_red(self):
		poly = ConvexPoly(mm_to_px(
			(2600,0),
			(3000,0),
			(3000,2000),
			(2600,2000)
		))
		verres = self._get_verres_in_poly(poly)
		return verres

	def _get_verres_in_poly(self, poly):
		verres = 0
		for obj in self.engine.objects:
			if isinstance(obj, Verre):
				if obj.pos() in poly:
					verres += 1
		return verres




	
