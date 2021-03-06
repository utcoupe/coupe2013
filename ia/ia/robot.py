
import copy
import time

import sys
sys.path.append('../../lib')
import utcoupe

from geometry import Segment, Vec

class Robot:
	def __init__(self, p, ng):
		self.init_actions = []
		self.init_pos = p
		self.ng = ng
		self.asserv = None
		self.reset()
		self.path_was_intersected = False
		self.trajectoire = []
		self.sharps = [None, False, False, False, False]
		self.sharps_time = [None, 0, 0, 0, 0]
		self.stopped_by_sharps = 0

	def set_sharp_alert(self, position):
		if (position > 1 and position <= 4):
			self.sharps_time = time.time()

	def process_sharps(self):
		time = time.time()
		for i in range(1, 5):
			if (time - self.sharps_time[i]) >= utcoupe.SHARP_TIME_LIMIT:
				#C'est safe, on peut repartir
				self.sharps[i] = False
			else:
				self.sharps[i] = True

	def set_actions(self, actions):
		self.init_actions = actions
		self.actions = copy.copy(actions)

	def set_asserv(self, asserv):
		"""
		Link le client asservissement
		"""
		self.asserv = asserv

	def set_visio(self, visio):
		self.visio = visio
	
	def set_extras(self, extras):
		"""
		Link le client extras (téléportation)
		"""
		self.extras = extras

	def set_actionneurs(self, actionneurs):
		self.actionneurs = actionneurs

	def reset(self):
		self.pos = Vec(self.init_pos)
		self.a = 0
		if self.asserv:
			self.asserv.reset()
		self.time_pos_updated = 0
		self.in_action = False
		self.path = []
		self.target_action = None

		self.actions = copy.copy(self.init_actions)
		

	def reset_target_action(self):
		self.path = []
		self.target_action = None
	
	def set_new_path(self, path):
		self.path = path
		self.current_goal = path[0]

	def set_target_action(self, action, path):
		self.reset_target_action()
		self.target_action = action
		self.set_new_path(path)

	def reach_next_checkpoint(self):
		return (self.pos - self.path[0]).norm2() <= 100

	def get_next_checkpoint(self):
		self.path.pop(0)
		if self.path:
			return self.path[0]
		else:
			return None

	def is_arrive(self):
		return len(self.path) == 0

	def is_new_action(self, action):
		if not self.target_action:
			return True
		if action != self.target_action:
			x1,y1 = action.point_acces
			x2,y2 = self.target_action.point_acces
			if abs(x1-x2) < 50 and abs(y1-y2) < 50:
				self.target_action = action
				return False
			else:
				return True
		else:
			return False

	def update_pos(self, p):
		self.trajectoire.append((self.pos, self.time_pos_updated))
		self.pos = Vec(p)
		self.time_pos_updated = time.time()

	def is_path_intersected(self):
		for poly in self.ng.dynamic_obstacles:
			if self.poly_intersect_path(poly):
				return True
		return False
	
	def poly_intersect_path(self, poly):
		last = self.pos
		for current in self.path:
			s = Segment(last, current)
			if poly.intersect(s):
				return True
		return False

	def __repr__(self):
		return "Robot(pos=%s, a=%s)" % (self.pos, self.a)



