import sys
import os
FILE_DIR  = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.join(FILE_DIR,"..","lib"))

from .gateau import *
from .verre import *

import re
import time
import threading
from inspect import currentframe
import itertools
import math

# petite optimisation pour le calcul de k parmis 4, seul les valeurs de
# l'interval [0;4] sont calculées
C_K_PARMIS_4 = list(( tuple(itertools.permutations(range(4), i)) for i in range(5) ))
def permutation_k_parmis_4(k):
	return C_K_PARMIS_4[k]


class GameState:
	def __init__(self, bigrobot, minirobot, enemy1, enemy2, hokuyo):

		self.hokuyo				= hokuyo

		self.objects			= [enemy1, enemy2]

		self.event_bigrobot_pos_update = threading.Event()
		self.event_minirobot_pos_update = threading.Event()
		self.event_hokuyo_update = threading.Event()
		self.event_on_pong = threading.Event()
		self.event_bigrobot_visio_update = threading.Event()
		self.event_minirobot_visio_update = threading.Event()

		self.bigrobot = bigrobot
		self.minirobot = minirobot
		self.enemy1 = enemy1
		self.enemy2 = enemy2

		self.sums = {}
		self.sums['update_big_ng'] = {'t':0, 'n':0}
		self.sums['update_mini_ng'] = {'t':0, 'n':0}

		self.gateau = Gateau()
		self.verres = Verres()

		self.us_detect = False # pour l'homologation, si les ultra sons detectent quelque chose devant alors on s'arrête

		self.enemies_angle_mort = False # si les enemis sont dans un angle mort

	def reset(self):
		self.bigrobot.reset()
		#Mself.minirobot.reset()
		self.enemy1.reset()
		self.enemy2.reset()
		
		self.bigrobot.ng.dynamic_obstacles[0].move_to(self.enemy1.pos)
		self.bigrobot.ng.dynamic_obstacles[1].move_to(self.enemy2.pos)
		#Mself.bigrobot.ng.dynamic_obstacles[2].move_to(self.minirobot.pos)
		self.bigrobot.ng.update()
		#Mself.minirobot.ng.dynamic_obstacles[0].move_to(self.enemy1.pos)
		#Mself.minirobot.ng.dynamic_obstacles[1].move_to(self.enemy2.pos)
		#Mself.minirobot.ng.dynamic_obstacles[2].move_to(self.bigrobot.pos)
		#Mself.minirobot.ng.update()

		
		self.sums = {}
		self.sums['update_big_ng'] = {'t':0, 'n':0}
		self.sums['update_mini_ng'] = {'t':0, 'n':0}


	def update_robots(self):
		self.bigrobot.ng.dynamic_obstacles[0].move_to(self.enemy1.pos)
		self.bigrobot.ng.dynamic_obstacles[1].move_to(self.enemy2.pos)
		#Mself.bigrobot.ng.dynamic_obstacles[2].move_to(self.minirobot.pos)
		#Mself.minirobot.ng.dynamic_obstacles[0].move_to(self.enemy1.pos)
		#Mself.minirobot.ng.dynamic_obstacles[1].move_to(self.enemy2.pos)
		#Mself.minirobot.ng.dynamic_obstacles[2].move_to(self.bigrobot.pos)
		
		#if self.bigrobot.is_path_intersected():
		start_update_ng = time.time()
		self.bigrobot.ng.update()
		self.sums['update_big_ng']['t'] += time.time() - start_update_ng
		self.sums['update_big_ng']['n'] += 1
			
		#if self.minirobot.is_path_intersected():
		#Mstart_update_ng = time.time()
		#Mself.minirobot.ng.update()
		#Mself.sums['update_mini_ng']['t'] += time.time() - start_update_ng
		#Mself.sums['update_mini_ng']['n'] += 1
		
	
	def ask_update(self):
		self.event_bigrobot_pos_update.clear()
		#Mself.event_minirobot_pos_update.clear()
		self.event_hokuyo_update.clear()
		#self.event_minirobot_visio_update.clear()

		self.ask_asserv_for_pos(self.bigrobot)

		#Mself.ask_asserv_for_pos(self.minirobot)
		#self.ask_hokuyo_for_pos() # TO REMOVE !
		
	def wait_update(self):
		self.event_bigrobot_pos_update.wait()
		#self.event_bigrobot_visio_update.wait()
		#self.event_minirobot_visio_update.wait()
		#Mself.event_minirobot_pos_update.wait()
		#self.event_hokuyo_update.wait() # TO REMOVE !

	def ping(self, canal):
		n = 10
		start = time.time()
		for i in range(n):
			self.event_on_pong.clear()
			self.ircbot.send(canal, PREFIX_CMD+"ping # id=%s" % ID_MSG_PING)
			self.event_on_pong.wait()
		return (time.time() - start) / n

	def ask_hokuyo_for_pos(self):
		self.hokuyo.send_pos(cb_fct=self.on_msg_hokyo, block=False)

	def ask_asserv_for_pos(self, robot):
		cb = lambda y: self.on_msg_pos(y, robot)
		robot.asserv.pos(cb_fct=cb, block=False)

	def take_picture(self, block=False):
		self.event_bigrobot_visio_update.clear()
		self.bigrobot.visio.get_by_color(cb_fct=self.on_msg_visio, block=False)
		if block:
			self.event_bigrobot_visio_update.wait()

	def process_sharps(self):
		self.bigrobot.process_sharps()
		#Mself.minirobot.process_sharps()

	def on_msg(self, canal, auteur, msg):
		msg_split = msg.split(SEP)
		if len(msg_split) > 1:
			id_msg = int(msg_split[0])
			params = msg_split[1:]
			if ID_MSG_PING == id_msg:
				self.event_on_pong.set()
			elif ID_MSG_HOKUYO == id_msg:
				self.on_msg_hokyo(params)
			elif ID_MSG_POS == id_msg:
				self.on_msg_pos(canal,params)

	def on_msg_pos(self, resp, robot):
		args = resp.data
		#print(args, robot)
		if len(args) >= 3:
			# transformation des strings en int
			args = tuple(map(int, args))
			# choix du robot à update
			if robot == self.bigrobot:
				robot_to_update = self.bigrobot
				self.event_bigrobot_pos_update.set()
			elif robot == self.minirobot:
				robot_to_update = self.minirobot
				self.event_minirobot_pos_update.set()
			else:
				print("Error %s.on_msg_pos (%s:%d) : canal non connu : %s " % (self.__class__.__name__, currentframe().f_code.co_filename, currentframe().f_lineno, canal))
			# update
			robot_to_update.update_pos(args[0:2])
			robot_to_update.a = args[2]

	def on_msg_hokyo(self, resp):
		args = resp.data
		lpos = eval(args)
		#print(lpos)
		#lpos = list(filter(lambda p: (self.bigrobot.pos-p).norm2() > 300*300 and (self.minirobot.pos-p).norm2() > 300*300, lpos))
		lpos = list(filter(lambda p: (self.bigrobot.pos-p).norm2() > 300*300, lpos))
		if len(lpos) == 0:
			self.enemies_angle_mort = True
		else:
			self.enemies_angle_mort = False
			robots = self.enemyrobots()
			#print(lpos)
			if len(lpos)==0:
				pass
			elif len(lpos)==1:
				robot = min(robots, key=lambda r: (r.pos-lpos[0]).norm2())
				robot.update_pos(lpos[0])
			else:
				def test_permut(permut):
					l = ( (robots[i].pos - lpos[j]).norm2() for i,j in enumerate(permut) )
					return sum(l)
				permuts = tuple(itertools.permutations(range(len(lpos)), 2))
				print(permuts)
				if not permuts:
					print("quoi ?", lpos)
				else:
					best_permut = min(permuts, key=lambda permut: test_permut(permut))
					for i,j in enumerate(best_permut):
						robots[i].update_pos(lpos[j])
						self.verres.add_enemy_pos(lpos)
			#print(robots)
		self.event_hokuyo_update.set()

	def on_msg_visio(self, args):
		self.gateau.merge_data_str(args.data)
		self.event_bigrobot_visio_update.set()

	def on_msg_us(self, args):
		args = resp.data
		if len(args) == 1:
			dist = int(args[0])
			if dist < 200:
				if not self.us_detect:
					self.bigrobot.stop()
				self.us_detect = True
			else:
				if self.us_detect:
					self.bigrobot.resume()
				self.us_detect = False

	def robots(self):
		#return (self.bigrobot, self.minirobot, self.enemy1, self.enemy2)
		return (self.bigrobot, self.enemy1, self.enemy2)

	def enemyrobots(self):
		return (self.enemy1, self.enemy2)

	def __repr__(self):
		return "GameState(%s)" % (self.robots(),)

	def print_stats(self):
		for k, s in self.sums.items():
			if s['n'] != 0:
				print(k, s['t']/s['n'])

