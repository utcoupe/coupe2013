# -*- coding: utf-8 -*-


import random
import math
import time


from ..define import *
from ..engine.engineobject import EngineObjectPoly
#from .cd import Cd
#from .lingo import Lingo

import sys
sys.path.append('../../../lib')
import utcoupe


def angle_diff(a, b):
	return min(2*math.pi - (b-a), b-a)

class GoalPWM:
	def __init__(self, id_msg, pwm, delay):
		self.pwm = pwm
		self.delay = delay
		self.start = -1
		self.id_msg = id_msg

class GoalPOS:
	def __init__(self, id_msg, x, y, v):
		"""
		@param {int:px} x
		@param {int:px} y
		@param {float:px/s} v
		"""
		self.pos = (x,y)
		self.v = v
		self.id_msg = id_msg


class GoalANGLE:
	def __init__(self, id_msg, a, v):
		"""
		@param {float:radians} a
		@param {float:px/s} v
		"""
		self.a = a
		self.v = v
		self.id_msg = id_msg


class GoalPOSR(GoalPOS): pass
class GoalANGLER(GoalANGLE): pass

class Robot(EngineObjectPoly):
	def __init__(self, *, engine, asserv, asserv_obj=None, others, others_obj, visio, visio_obj,
	team, posinit, mass, poly_points, typerobot, extension_objects=[], match, services):
		color = 'blue' if team == BLUE else 'red'
		EngineObjectPoly.__init__(self,
			engine		 	= engine,
			mass			= mass,
			posinit			= posinit,
			color			= color,
			colltype		= colltype, 
			poly_points		= poly_points,
			extension_objects	= extension_objects
		)

		self.typerobot = typerobot
		self.match = match

		# quand on clique ça téléporte au lieu d'envoyer un ordre à l'asservissement
		self.mod_teleport = False

		# marche arrière ou marche avant ?
		self.mod_recul = False

		# vitesse maximale (quand pwm=255)
		self.max_speed = 1000
		
		# team
		self.team = team

		# les goals
		self.goals = []
		
		self.stop = False

		# touche shift enfoncée ?
		self.current_robot	= BIG
		self.current_team	= BLUE
		self.stop			= False

		if asserv_obj != None:
			self.asserv = asserv_obj
		else:
			self.asserv = Asserv(self)
		self.asserv_service = services.create(asserv, self.asserv)
		self.others = others_obj
		print(self.asserv, self.others)
		self.others_service = services.create(others, self.others)
		if visio != None and visio_obj != None:
			self.visio = Visio(self)
			self.visio_service = services.create(visio, self.visio)

		self.body._set_velocity_func(self._my_velocity_func())
		
		self.state_jack = 0  # jack in

	def init(self, engine):
		self.engine = engine
	
	def x(self):
		return px_to_mm(self.body.position[0])
	
	def y(self):
		return px_to_mm(self.body.position[1])
	
	def a(self):
		return int(math.degrees(self.body.angle))
		

	def _my_velocity_func(self):
		def f(body, gravity, damping, dt):
		
			self.body._set_torque(0)
			self.body._set_angular_velocity(0)
			if not self.stop and self.goals:
				current_goal = self.goals[0]
				if isinstance(current_goal, GoalPOSR):
					x,y = self.body.position
					a = self.body.angle
					cx, cy = current_goal.pos
					dx = cx * math.cos(a) - cy * math.sin(a)
					dy = cx * math.sin(a) + cy * math.cos(a)
					self.goals[0] = GoalPOS(current_goal.id_msg, x+dx, y+dy, current_goal.v)
				elif isinstance(current_goal, GoalANGLER):
					a = current_goal.a
					ca = self.body.angle
					self.goals[0] = GoalANGLE( current_goal.id_msg, ca+a, current_goal.v )
				elif isinstance(current_goal, GoalPOS):
					gx,gy = current_goal.pos
					v = current_goal.v
					x,y = self.body.position
					dx = gx - x
					dy = gy - y
					d = math.sqrt(dx**2+dy**2)
					if d < abs(v * dt):
						self.body._set_position((gx,gy))
						self.goals.pop(0)
						self.body._set_velocity((0,0))
						self.asserv_service.send_response(current_goal.id_msg, 2)
					else:
						a = math.atan2(dy, dx)
						vx = abs(v) * math.cos(a)
						vy = abs(v) * math.sin(a)
						self.body._set_velocity((vx,vy))
						if v < 0:
							a += math.pi
						self.body._set_angle(a)
				elif isinstance(current_goal, GoalPWM):
					if current_goal.start == -1:
						current_goal.start = time.time()
					elif (time.time() - current_goal.start) > current_goal.delay:
						self.goals.pop(0)
						self.asserv_service.send_response(current_goal.id_msg, 0)
					else:
						a = self.body.angle
						v = self.max_speed * current_goal.pwm / 255
						vx = v * math.cos(a)
						vy = v * math.sin(a)
						self.body._set_velocity((vx,vy))
				elif isinstance(current_goal, GoalANGLE):
					self.body._set_angle(current_goal.a)
					self.goals.pop(0)
					self.asserv_service.send_response(current_goal.id_msg, 2)
				else:
					raise Exception("type_goal inconnu")
			else:
				self.body._set_velocity((0,0))
		return f

	def onEvent(self, event):
		# selection des teams et des robots
		if KEYDOWN == event.type:
			if KEY_CHANGE_TEAM == event.key:
				self.current_team = RED
				print("équipe rouge")
				return True
			elif KEY_CHANGE_ROBOT == event.key:
				self.current_robot = MINI
				print("control du mini robot")
				return True
			elif KEY_TELEPORTATION == event.key:
				self.mod_teleport = not self.mod_teleport
				return True
			elif KEY_RECUL == event.key:
				self.mod_recul = not self.mod_recul
			elif KEY_JACK == event.key:
				self.state_jack = 0 if self.state_jack else 1
				self.others.send_event('jack', self.state_jack)
		elif KEYUP == event.type:
			if KEY_CHANGE_TEAM == event.key:
				print("équipe bleu")
				self.current_team = BLUE
				return True
			elif KEY_CHANGE_ROBOT == event.key:
				self.current_robot = BIG
				print("control gros robot")
				return True

		# actions
		if self._event_concerns_me(event):
			# keydown
			if KEYDOWN == event.type:
				if KEY_STOP_RESUME == event.key:
					self.asserv.stop(0)
					return True
				elif KEY_CANCEL == event.key:
					self.asserv.cancel(0)
					return True
			# keyup
			elif KEYUP == event.type:
				if KEY_STOP_RESUME == event.key:
					self.asserv.resume(0)
			# mouse
			elif MOUSEBUTTONDOWN == event.type:
				p = event.pos
				p_mm = px_to_mm(p)
				print(p_mm)
				if self.mod_teleport:
					self.extras.teleport(p_mm[0], p_mm[1], 0)
				else:
					v = mm_to_px(1000) * (-1 if self.mod_recul else 1)
					self.asserv.goto(0, *px_to_mm(p[0],p[1],v))
				return True
		return False

	def _event_concerns_me(self, event):
		return self.current_team == self.team and self.typerobot == self.current_robot
	
	def __repr__(self):
		return "Robot"

	def inv_a(self, a):
		return -a	

class Asserv:
	""" Émule l'arduino dédiée à l'asservissement """

	def __init__(self, robot):
		self.robot = robot

	
	def id(self, id_msg):
		return 'asserv'

	def ping(self, id_msg):
		return 'pong'
	
	def goto(self, id_msg, x, y, v):
		"""
		Donner l'ordre d'aller à un point
		@param x mm
		@param y mm
		@param v mm/s
		"""
		self.robot.goals = []
		self.robot.goals.append( GoalPOS( id_msg, *mm_to_px(x,y,v*2) ) )
		return utcoupe.ResponseLater()
	
	def gotor(self, id_msg, x, y, v):
		"""
		Donner l'ordre d'aller à un point, relativement à la position actuelle
		@param x mm
		@param y mm
		@param v mm/s
		"""
		self.robot.goals = []
		self.robot.goals.append( GoalPOSR( id_msg, *mm_to_px(x,y,v*2) ) )
		return utcoupe.ResponseLater()

	def turn(self, id_msg, a, v):
		#print("HELLO", a, self.inv_a(a))
		#self.goals.append( GoalANGLE( id_msg, math.radians(self.inv_a(a)), mm_to_px(v) ) )
		self.robot.goals.append( GoalANGLE( id_msg, math.radians(a), mm_to_px(v) ) )
		return 1

	def turnr(self, id_msg, a, v):
		#self.goals.append( GoalANGLER( id_msg, math.radians(self.inv_a(a)), mm_to_px(v) ) )
		self.robot.goals.append( GoalANGLER( id_msg, math.radians(a), mm_to_px(v) ) )
		return utcoupe.ResponseLater()

	def cancel(self, id_msg):
		self.robot.goals = []
		return 0

	def stop(self, id_msg):
		self.robot.stop = True
		return 0

	def resume(self, id_msg):
		self.robot.stop = False
		return 0
	
	def pos(self, id_msg):
		return (self.robot.x(), self.robot.y(), self.robot.a())

	def pwm(self, id_msg, pwm_l, pwm_r, delay):
		self.robot.goals.append(GoalPWM(id_msg, pwm_l, delay/1000))
		return utcoupe.ResponseLater()
	
class Visio:
	"""Émule le programme de visio"""

	def __init__(self, robot):
		self.robot = robot

	def get_candles(self, id_msg):
		pass

	def ping(self, arg):
		return "pong"

class Others:
	def __init__(self, robot):
		self.robot = robot

	def goto_ax12(self, id_msg, id_moteur, pos, vitesse):
		return 1

	def ping(self, id_msg, entier):
		return entier + 42;

	def id(self, id_msg):
		return "others"
