#!/usr/bin/python3
# -*- coding: utf-8 -*-


import sys
import os
FILE_DIR  = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.join(FILE_DIR,"..","lib"))

import optparse

import zerobot
import utcoupe
from ia import *

default = {}
default["server_ip"]    = "localhost"
default["server_port_frontend"]  = 5000
default["server_port_backend"]  = 5001
default["server_port_ev_push"]  = 5003
default["server_port_ev_sub"]  = 5004
default["ia"]           = 'a'
default["color"]        = 'blue'
default["autostart"]    = 0
default["timeout"]      = 89
default["skip"]         = 0

usage = "usage: %prog [options]"
parser = optparse.OptionParser(usage,version="%prog 0.0")
parser.add_option("-s", "--server-ip",
                  action="store", dest="server_ip", default=default["server_ip"],
                  help="Zerobot server ip")
parser.add_option("-F", "--server-port-frontend",
                  action="store", dest="port_frontend", type="int", default=default["server_port_frontend"],
                  help="Zerobot server frontend port")
parser.add_option("-B", "--server-port-backend",
                  action="store", dest="port_backend", type="int", default=default["server_port_backend"],
                  help="Zerobot server backend port")
parser.add_option("-P", "--server-port-ev-push",
                  action="store", dest="port_ev_push", type="int", default=default["server_port_ev_push"],
                  help="Zerobot server events publishing port")
parser.add_option("-S", "--server-port-ev-sub",
                  action="store", dest="port_ev_sub", type="int", default=default["server_port_ev_sub"],
                  help="Zerobot server events subscribing port")
parser.add_option("-i", "--ia",
                  action="store", dest="ia", default=default["ia"],
                  help="IA à utiliser : t=Dev, p=Prod")
parser.add_option("-c", "--color",
                  action="store", dest="color", default=default["color"],
                  help="Couleur du bot, red ou blue")
parser.add_option("-a", "--autostart",
                  action="store", dest="autostart", type="int", default=default["autostart"],
                  help="Si autostart vaut 1 le robot démarre sans attendre le jack")
parser.add_option("-t", "--timeout",
                  action="store", dest="timeout", type="int", default=default["timeout"],
                  help="durée d'un latch en secondes, -1 pour infini")
parser.add_option("-k", "--skip",
                  action="store", dest="skip", type="int", default=default['skip'],
                  help="Si skip vaut 1 on ne fait pas le recalage du robot")
(options, args) = parser.parse_args()

if options.ia == 'a':
    IaChoosen = IaBase
elif options.ia == 'u':
    IaChoosen = IaUtcoupe
else:
    exit('IA invalide')

if options.color in ('red', 'blue'):
    c = (options.color != 'blue')
else:
    exit('Couleur invalide')

# Store server info
server = utcoupe.Server(server = options.server_ip,
                        frontend = options.port_frontend,
                        backend = options.port_backend,
                        ev_push = options.port_ev_push,
                        ev_sub = options.port_ev_sub)

myia = IaChoosen(server,
                 pos_grosbot = (225,600),
                 pos_minibot = (0,0),
                 pos_enemy1 = (3000,0),
                 pos_enemy2 = (3000,0),
                 team          = c,
                 autostart     = (options.autostart==1),
                 match_timeout = options.timeout if options.timeout > 0 else None,
                 skip_recalage = (options.skip == 1))

try:
    myia.start()
except KeyboardInterrupt as ex:
    myia.stop()
