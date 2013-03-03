# -*- coding: utf-8 -*-

from pymunk import RotaryLimitJoint, PinJoint
import pymunk
from math import *

from geometry import Vec


from ..define import *
from .robot import Robot, MINI, Others
from ..engine.engineobject import EngineObjectPoly,EngineObjectSegment
from .verre import Verre


class Palette(EngineObjectSegment):
	def __init__(self, engine, offset, angle_palette):
		angle = angle_palette
		pa = (0,0)
		pb = mm_to_px(SIZE_PALETTE * cos(angle), SIZE_PALETTE * sin(angle))
		EngineObjectSegment.__init__(self,
			engine			= engine,
			colltype		= COLLTYPE_PETIT_ROBOT,
			offset 			= offset,
			posA			= pa,
			posB			= pb,
			color			= "orange",
			mass			= 80,
			width			= WIDTH_PALETTE 
		)

# class Balais(EngineObjectSegment):
# 	def __init__(self, engine, angle_balais):
# 		angle = radians(angle_balais)
# 		pa = (0,0)
# 		pb = mm_to_px(SIZE_BALAIS*cos(angle), SIZE_BALAIS*sin(angle))
# 		d = mm_to_px(WIDTH_MINI/2+ECART_ROBOT_BALAIS)
# 		offset = d*cos(angle), d*sin(angle)
# 		"""points = [(0,0), (SIZE_BALAIS,0), (SIZE_BALAIS,WIDTH_BALAIS), (0,WIDTH_BALAIS)]
# 		points = map(lambda p: Vec(mm_to_px(p)), points)
# 		cosa = cos(angle)
# 		sina = sin(angle)
# 		points = map(lambda v: ((v[0]*cosa - sina*v[1]), (v[0]*sina + v[1]*cosa)), points)

# 		d = mm_to_px(WIDTH_MINI/2+ECART_ROBOT_BALAIS+SIZE_BALAIS/2)
# 		offset = (d*cosa, d*sina)"""
# 		EngineObjectSegment.__init__(self,
# 			engine			= engine,
# 			colltype		= COLLTYPE_ROBOT,
# 			offset			= offset,
# 			posA			= pa,
# 			posB			= pb,
# 			color			= "orange",
# 			width			= WIDTH_PALETTE
# 		)


class MiniRobot(Robot):
	def __init__(self, *, engine, asserv, others, posinit, team, match, services):
		Robot.__init__(self,
			engine 				= engine,
			asserv				= asserv,
			asserv_obj 			= None,
			others				= others,
            others_obj 			= MiniOthers(self),
            visio 				= None,
            visio_obj 			= None,
			team				= team,
			posinit				= posinit,
			mass				= 10,
			typerobot			= MINI,
			colltype 			= COLLTYPE_PETIT_ROBOT,
			poly_points			= mm_to_px((0,0),(HEIGHT_MINI,0),(HEIGHT_MINI,WIDTH_MINI),(0,WIDTH_MINI)),
            match 				= match,
            services 			= services
		)

		
		self.palette_left = Palette(engine, mm_to_px(HEIGHT_MINI/2, WIDTH_MINI/2), 45)
		self.palette_right = Palette(engine, mm_to_px(HEIGHT_MINI/2, -WIDTH_MINI/2), -45)

		# self.balais_left = Balais(engine, 90)
		# self.balais_right = Balais(engine, -90)

		self.nb_verres	 = 0
		self.nb_cadeau_pousse = 0
		self.state_buldo = False

	def remove_palette_left(self):
		self.remove_body_extension(self.palette_left)

	def add_palette_left(self):
		self.add_body_extension(self.palette_left)

	def remove_palette_right(self):
		self.remove_body_extension(self.palette_right)

	def add_palette_right(self):
		self.add_body_extension(self.palette_right)

	def prendre_verre(self, verre):
		if not self.state_buldo:
			if not self.is_full() and verre.color != "orange":
				# calcul de la position d'attérissage (un peu devant le robot
				if verre.color == "red":
					self.nb_verres += 2
				else: self.nb_verres += 1
				self.engine.objects_to_remove.append(verre)

						
	def pousser_cadeau(self, color):
		if(color == 'red' and self.team == RED) or (color == 'blue' and self.team == BLUE):
			self.nb_cadeau_pousse += 1
		else:
			print("-> Le petit a pousse le mauvais cadeau!")

	def monter_verre(self):
		for v in self.extension_objects:
			if isinstance(v, Verre.__class__):
				v.monter_verre()

	def is_full(self):
		return self.nb_verres * COEFF_ENGORGEMENT_VERRE >= 1

	# def remove_balais_left(self):
	# 	self.remove_body_extension(self.balais_left)

	# def add_balais_left(self):
	# 	self.add_body_extension(self.balais_left)

	# def remove_balais_right(self):
	# 	self.remove_body_extension(self.balais_right)

	# def add_balais_right(self):
	# 	self.add_body_extension(self.balais_right)
	

	def onEvent(self, event):
		if not Robot.onEvent(self, event):
			if self._event_concerns_me(event):
				if KEYDOWN == event.type:
					if KEY_BULDO == event.key:
						self.state_buldo = not self.state_buldo
						self._cmd_others_buldo(self.state_buldo)
					elif KEY_DROP == event.key:
						self._cmd_others_drop_verre()

	# def _cmd_others_arracher_carte(self, **kwargs):
	# 	self.match.arracher_carte(self.team)
	# 	self.send_canal_others(kwargs['id_msg'], 1)
	# 	print("Carte arrachée")

	# def _cmd_others_balais(self, right, on):
	# 	if right:
	# 		if on:
	# 			self.add_balais_right()
	# 		else:
	# 			self.remove_balais_right()
	# 	else:
	# 		if on:
	# 			self.add_balais_left()
	# 		else:
	# 			self.remove_balais_left()

	def _cmd_others_buldo(self, on):
		if on:
			self.add_palette_left()
			self.add_palette_right()
		else:
			self.remove_palette_left()
			self.remove_palette_right()

	def _cmd_others_is_full(self, **kwargs):
		coeff_engorgement = (self.nb_white_cds+self.nb_black_cds) * COEFF_ENGORGEMENT_CD
		coeff_engorgement += self.nb_lingos * COEFF_ENGORGEMENT_LINGO
		r = 0 if coeff_engorgement < 1 else 1
		self.send_canal_asserv(kwargs['id_msg'], r)

	def _cmd_others_drop_verre(self):

		# Le petit drop les verres en tour de 2, et puis eventuellement un verre seul!
		nb_tours3_to_drop = self.nb_verres // 3
		nb_tours2_to_drop = (self.nb_verres % 3) // 2
		nb_verre_to_drop = 0 if (self.nb_verres % 3) % 2 == 0 else 1 

		# On calcul le position de drop, donc un peu devant le robot.
		DIST = 600
		pos = Vec(self.pos())
		angle = self.angle()
		pos_drop = pos + mm_to_px(mm_to_px(DIST * cos(angle), DIST * sin(angle)))
		pos_drop = tuple(pos_drop)

		for _ in range(nb_tours3_to_drop):
			self.engine.add(Verre(self.engine, pos_drop, "orange")) # Orange pour les tours de 3, rouge pour les verres seule
		for _ in range(nb_tours2_to_drop):
			self.engine.add(Verre(self.engine, pos_drop, "red")) # Red pour les tours de 2, rouge pour les verres seule
		for _ in range(nb_verre_to_drop):
			self.engine.add(Verre(self.engine, pos_drop, "black"))

		self.nb_verres = 0

class MiniOthers(Others):
	pass
