#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
Simulateur permettant de simuler un match d'eurobot.

Déplacer gros robot: clic gauche
Déplacer petit robot: clic droit

Déplacer adversaire: idem + ctrl

@author Thomas Recouvreux
@author Pierre-Henry Fricot
@author Cédric Bache

Pour la version 2013, nous avons utilisé le nouveau protocole de communication zérobot. Les objets et engines sont également réécrit pour adapter au nouveau reglement.
@author Siqi LIU <me@siqi.fr>
@author Florent Thévenet <florent@fthevenet.fr>
@author Thomas Fuhrmann <tomesman@gmail.com>

"""


import sys
import os
DIR_PATH = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.join(DIR_PATH, "..", "lib"))


import optparse
import threading
import time
import zmq

import zerobot
import utcoupe

from simu import *

if __name__ == "__main__":
        
        default = {}
        default["server_ip"]            = "localhost"
        default["port_frontend"]        = 5000
        default["port_backend"]         = 5001
        default["port_ev_push"]         = 5003
        default["port_ev_sub"]          = 5004

        usage = "usage: %prog [options]"
        parser = optparse.OptionParser(usage,version="%prog 0.0")
        parser.add_option("-S", "--server-ip",
                          action="store", dest="server_ip", default=default["server_ip"],
                          help="ip zerobot server")
        parser.add_option("-b", "--port-backend",
                          action="store", dest="port_backend", type="int", default=default["port_backend"],
                          help="port backend")
        parser.add_option("-f", "--port-frontend",
                          action="store", dest="port_frontend", type="int", default=default["port_frontend"],
                          help="port frontend")
        parser.add_option("-p", "--port-ev-push",
                          action="store", dest="port_ev_push", type="int", default=default["port_ev_push"],
                          help="port events publishing")
        parser.add_option("-s", "--port-ev-sub",
                          action="store", dest="port_ev_sub", type="int", default=default["port_ev_sub"],
                          help="port events subscribtion")
        (options, args) = parser.parse_args()


        engine = Engine()
        match = Match()

        # debug
        debug = Debug()

        # Store server info
        server = utcoupe.Server(server = options.server_ip,
                                frontend = options.port_frontend,
                                backend = options.port_backend,
                                ev_push = options.port_ev_push,
                                ev_sub = options.port_ev_sub)

        # Create services on demand
        ctx = zmq.Context(1)
        services = utcoupe.ServicesManager(server, ctx)

        # robots
        bigrobot = BigRobot(engine = engine,
                            posinit = mm_to_px(250, 2000 - 250),
                            team = BLUE,
                            asserv = utcoupe.ASSERV_BIG,
                            others = utcoupe.OTHERS_BIG,
                            visio = utcoupe.VISIO_BIG,
                            match = match,
                            services = services)
        minirobot = MiniRobot(engine = engine,
                              posinit = mm_to_px(400, 2000 - 250),
                              team = BLUE,
                              asserv = utcoupe.ASSERV_MINI,
                              others = utcoupe.OTHERS_MINI,
                              match = match,
                              services = services)
        bigrobot2 = BigRobot(engine = engine,
                             posinit = mm_to_px(3000-250,250),
                             team = RED,
                             asserv = utcoupe.ASSERV_BIG_ENEMY,
                             others = utcoupe.OTHERS_BIG_ENEMY,
                             visio = utcoupe.VISIO_BIG_ENEMY,
                             match = match,
                             services = services)
        minirobot2 = MiniRobot(engine = engine,
                               posinit = mm_to_px(3000-400,250),
                               team = RED,
                               asserv = utcoupe.ASSERV_MINI_ENEMY,
                               others = utcoupe.OTHERS_MINI_ENEMY,
                               match = match,
                               services = services)
        robots = (bigrobot, minirobot, bigrobot2, minirobot2)

        # hokuyo
        hokuyo = services.create(utcoupe.HOKUYO, Hokuyo(robots))

        engine.init(services.stop,match,debug)
        match.init(engine)
        bigrobot.init(engine)
        minirobot.init(engine)
        bigrobot2.init(engine)
        minirobot2.init(engine)
        
        load_map("map.xml",engine)
        
        engine.add(bigrobot)
        engine.add(minirobot)
        engine.add(bigrobot2)
        engine.add(minirobot2)

        t=threading.Thread(target=engine.start)
        t.setDaemon(True)
        t.start()

        while not engine.e_stop.is_set():
                try:
                        engine.e_stop.wait(3)
                        print(match.score(BLUE))
                        print(match.score(RED))
                except KeyboardInterrupt:
                        engine.stop()
                        break

