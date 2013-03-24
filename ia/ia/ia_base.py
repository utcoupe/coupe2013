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
        ng.add_dynamic_obstacle(ConvexPoly().initFromCircle(self.init_pos['enemy1'],R_ENEMY,8))
        ng.add_dynamic_obstacle(ConvexPoly().initFromCircle(self.init_pos['enemy2'],R_ENEMY,8))
        ng.add_dynamic_obstacle(ConvexPoly().initFromCircle(self.init_pos['mini'],R_MINIROBOT,8))
        ng.update()
        
        # robot
        bigrobot = Robot(self.init_pos['big'], ng)

        # Créateur de clients zerobot
        self.zfactory = utcoupe.ClientsFactory(server)

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
        ng.add_dynamic_obstacle(ConvexPoly().initFromCircle(self.init_pos['enemy1'],R_ENEMY,8))
        ng.add_dynamic_obstacle(ConvexPoly().initFromCircle(self.init_pos['enemy2'],R_ENEMY,8))
        ng.add_dynamic_obstacle(ConvexPoly().initFromCircle(self.init_pos['big'],R_BIGROBOT,8))
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
        hokuyo = self.zfactory.get_client(utcoupe.HUKUYO)

        #####
        ## Gamestate // Est-ce qu'on garde ce truc ? Son rôle est pas clair
        #####
        self.gamestate = GameState(bigrobot, minirobot, enemy1, enemy2, hokuyo)
        minirobot.asserv.configure(self)
        bigrobot.asserv.configure(self)


        #####
        ## Statistiques
        #####
        self.time_last_show_stats    = 0
        self.sums = {}
        self.sums['mainloop'] = {'t':0, 'n':0}

    def reset(self):
        self.gamestate.reset()

    def start(self):
        print("Attente de la connection au serveur Zerobot...")
        #self.ircbot.e_welcome.wait()
        try:
            self.gamestate.bigrobot.asserv.ping(0, timeout = 10)
        except Exception:
            print("Impossible de se connecter à l'asserv")
            quit()
        
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
        
        if self.team:
            self.gamestate.hokuyo.setRed()
        else:
            self.gamestate.hokuyo.setBlue()
        # premier rafraichissement
        self.gamestate.ask_update()
        self.gamestate.wait_update()
    
    def loop(self):
        # demande de rafraichissement
        self.gamestate.ask_update()
        
        # attente du rafraichissement
        self.gamestate.wait_update()

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


def get_latency(service):
    n = 10
    start = time.time()
    for i in range(n):
        service.ping()
        return (time.time() - start) / n
