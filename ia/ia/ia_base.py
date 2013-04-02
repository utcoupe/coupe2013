import threading
import time

import sys
sys.path.append('../../lib')

import zmq

import zerobot
import utcoupe
from geometry import ConvexPoly

from .gamestate import GameState
from .robot import Robot
from .graph import NavGraph
from .action import Action
from .smartasserv import SmartAsserv
from .actions import *

JACK_OUT = 1        # jack arraché
JACK_IN  = 0        # jack pluggé

STATE_PLAY = 1

class IaBase:
    def __init__(self, server, *, pos_grosbot, pos_minibot, pos_enemy1, pos_enemy2, team, autostart, match_timeout, skip_recalage):

        # équipe
        self.team = team

        self.init_pos = {}
        self.init_pos['big'] = pos_grosbot
        self.init_pos['mini'] = pos_minibot
        self.init_pos['enemy1'] = pos_enemy1
        self.init_pos['enemy2'] = pos_enemy2
        enemy1 = Robot(self.init_pos['enemy1'], None)
        enemy2 = Robot(self.init_pos['enemy1'], None)
        enemies = (enemy1, enemy2)

        #####
        ## Création gros robot
        #####

        # création du graph de déplacement
        ng = NavGraph(utcoupe.RAYON_BIGROBOT, utcoupe.FILENAME_MAP)
        ng.add_dynamic_obstacle(ConvexPoly().initFromCircle(self.init_pos['enemy1'],utcoupe.RAYON_ENEMY,8))
        ng.add_dynamic_obstacle(ConvexPoly().initFromCircle(self.init_pos['enemy2'],utcoupe.RAYON_ENEMY,8))
        ng.add_dynamic_obstacle(ConvexPoly().initFromCircle(self.init_pos['mini'],utcoupe.RAYON_MINIROBOT,8))
        ng.update()
        
        # robot
        bigrobot = Robot(self.init_pos['big'], ng)

        # Créateur de clients zerobot
        self.zfactory = utcoupe.ClientsFactory('IA', server)

        # création de l'asserv
        asserv = SmartAsserv(self.zfactory.get_client(utcoupe.ASSERV_BIG))
        bigrobot.set_asserv(asserv)

        # création de la visio
        visio = self.zfactory.get_client(utcoupe.VISIO_BIG)
        bigrobot.set_visio(visio)
        
        #creation des actionneurs
        actionneurs = self.zfactory.get_client(utcoupe.OTHERS_BIG)
        bigrobot.set_actionneurs(actionneurs)
        
        #####
        ## Création mini robot
        #####
        
        # création du graph de déplacement
        ng = NavGraph(utcoupe.RAYON_MINIROBOT, utcoupe.FILENAME_MAP)
        ng.add_dynamic_obstacle(ConvexPoly().initFromCircle(self.init_pos['enemy1'],utcoupe.RAYON_ENEMY,8))
        ng.add_dynamic_obstacle(ConvexPoly().initFromCircle(self.init_pos['enemy2'],utcoupe.RAYON_ENEMY,8))
        ng.add_dynamic_obstacle(ConvexPoly().initFromCircle(self.init_pos['big'],utcoupe.RAYON_BIGROBOT,8))
        ng.update()

        #robot
        minirobot = Robot(self.init_pos['mini'], ng)
        
        # création de l'asserv
        asserv = SmartAsserv(self.zfactory.get_client(utcoupe.ASSERV_MINI))
        minirobot.set_asserv(asserv)
        
        #creation des actionneurs
        petits_actionneurs = self.zfactory.get_client(utcoupe.OTHERS_MINI)
        minirobot.set_actionneurs(petits_actionneurs)

	#Hokuyo
        hokuyo = self.zfactory.get_client(utcoupe.HOKUYO)

        self.gamestate = GameState(bigrobot, minirobot, enemy1, enemy2, hokuyo)
        minirobot.asserv.configure(self)
        bigrobot.asserv.configure(self)

        self.skip_recalage = skip_recalage
        self.autostart = autostart
        self.match_timeout = match_timeout
        self.state_match = STATE_PLAY if autostart else 0

        #####
        ## Statistiques
        #####
        self.time_last_show_stats    = 0
        self.sums = {}
        self.sums['mainloop'] = {'t':0, 'n':0}

        self.reset()

    def reset(self):
        self.gamestate.reset()

        minirobot = self.gamestate.minirobot
        bigrobot = self.gamestate.bigrobot
        enemies = self.gamestate.enemyrobots()

        bigrobot.actionneurs.detect_ax12(cb_fct=self.detect_ax12_big)
        bigrobot.actionneurs.add_callback('jack_removed', self.cb_jack)
        bigrobot.actionneurs.add_callback('emergency_stop', self.cb_stop)
        actions = get_actions_bigrobot(self, bigrobot, enemies)
        bigrobot.set_actions(actions)

        actions = get_actions_minirobot(self, minirobot, enemies)
        minirobot.set_actions(actions)

        self.t_begin_match = None
        self.match_over = lambda : (self.t_begin_match and self.match_timeout and (time.time() - self.t_begin_match) > self.match_timeout)
        self.e_jack = threading.Event()

        time.sleep(0.5)

    def detect_ax12_big(self, resp):
        mapping = resp.data

    def cb_jack(self):
        print("Jack enlevé, let's start")
        self.e_jack.set()

    def cb_stop(self):
        self.stop()

    def start(self):
        print("Attente de la connection au serveur Zerobot...")
        #self.ircbot.e_welcome.wait()
        try:
            self.gamestate.bigrobot.asserv.ping(0, timeout = 10)
        except Exception as e:
            print("Timeout asserv Ctrl - Alt gr - \ pour tuer l'IA")

        #print("Get latency big asserv")
        #print(self.gamestate.bigrobot.asserv.get_latency())
        #print("Get latency mini asserv")
        #print(self.gamestate.bigrobot.asserv.get_latency())             # A DECOMMENTER
        #print("Get latency big visio")
        #print(self.gamestate.bigrobot.visio.get_latency())
        #print("Get latency mini visio")
        #print(self.gamestate.minirobot.visio.get_latency())            # A DECOMMENTER
        #print("Ping hokuyo")
        #print(self.gamestate.hokuyo.get_latency())                         # A DECOMMENTER HOKUYO
        #input("appuyez sur une touche pour démarrer")
        self.loopsetup()
        while 1:
            start_main_loop = time.time()

            # faire quelquechose
            self.loop()
            
            # calcul du temps écoulé
            self.sums['mainloop']['t'] += time.time() - start_main_loop
            self.sums['mainloop']['n'] += 1

            # calcul des stats
            self.stats()
                
            delay = max(0.05, 0.2 - time.time() - start_main_loop)
            time.sleep(delay)

    def stop(self):
        self.zfactory.stop()
        print("Exit")

    def loopsetup(self):
        self.gamestate.reset()
        
        # premier rafraichissement
        self.gamestate.ask_update()
        self.gamestate.wait_update()

    def loop(self):
        if self.match_over():
            print("Fin du Match")
            self.reset()

        if self.state_match is STATE_PLAY:
            # demande de rafraichissement
            self.gamestate.ask_update()
            # attente du rafraichissement
            self.gamestate.wait_update()

            if self.gamestate.us_detect:
                print("Les pings ont detectés un bolosse en face !!!")
            elif self.gamestate.enemies_angle_mort:
                print("Un bolosse est dans l'angle mort !!!")
            else:
                self.loopRobot(self.gamestate.bigrobot)
        else:
            self.e_jack.wait(2)

    def loopRobot(self, robot):
        actions = robot.actions
        ng = robot.ng
        asserv = robot.asserv
            
        if not self.gamestate.bigrobot.actions and not self.gamestate.minirobot.actions:
            print("PLUS D'ACTIONS A RÉALISER !")
            self.reset()
            return
            
        # si le robot n'est pas en action et qu'il reste des actions
        if not robot.in_action and actions:
            # recherche de la meilleur action à effectuer
            for action in actions:
                action.compute_score(robot.pos)
            reachable_actions = tuple(filter(lambda a: a.path, actions))
            
            if reachable_actions:
                best_action = min(reachable_actions, key=lambda a: a.score)
                
                #print(best_action)

                # si cette action n'est pas déjà celle que le robot veut atteindre
                # ou qu'un robot a coupé le chemin
                if robot.is_new_action(best_action) or \
                    robot.is_path_intersected():
                    print("CHANGEMENT D'ACTION", best_action)
                    asserv.cancel()
                    self.debug.draw_path(best_action.path, (255,0,0), id(robot))
                    print(robot.pos, best_action.point_acces, best_action.path)
                    robot.set_target_action(best_action, best_action.path)

                # si le robot est arrivé au point de lancement de l'action
                if robot.is_arrive():
                    print("YEEEES")
                    best_action.start()
                else:
                    # si le robot a atteind le prochain point de passage,
                    # on passe au suivant
                    if robot.reach_next_checkpoint():
                        goal = robot.get_next_checkpoint()
                        print("goto %s" % goal)
                        #self.bigrobot.cancel()
                        if goal:
                            asserv.goto(goal)
            else:
                print("No reachable actions")


    def stats(self):
        time_since_last_show_stats = time.time() - self.time_last_show_stats
        if time_since_last_show_stats >= 2:
            self.print_stats()
            self.time_last_show_stats = time.time()

    def print_stats(self):
        for k, s in self.sums.items():
            if s['n'] != 0:
                print(k, s['t']/s['n'])
        self.gamestate.print_stats()

    def p(self, p):
        """
        inverse la position si le robot est rouge, à utiliser dans les
        scripts fixes
        @param {position} p
        @return inverse p
        """
        return [self.x(p[0]), p[1]]

    def x(self, x):
        """
        inverse X si la couleur du robot est rouge, à utiliser dans les
        scripts fixes
        @param x
        @return (3000 - x) si rouge sinon x
        """
        if self.team == BLUE:
            return 3000 - x
        else:
            return x

    def a(self, a):
        """
        inverse l'angle si la couleur du robot est rouge, à utiliser dans
        les scripts fixes
        @param {degrès} a
        @return (a + 180) si rouge sinon a
        """
        if self.team == BLUE:
            if a > 0:
                return 180 - a
            else:
                return 180 + a
        else:
            return a

def get_latency(service):
    n = 10
    start = time.time()
    for i in range(n):
        service.ping()
        return (time.time() - start) / n
