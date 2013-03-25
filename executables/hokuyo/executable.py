import sys
sys.path.append('../../lib')

import zerobot
import optparse
import logging
import os.path
import subprocess

logging.basicConfig(level=logging.DEBUG)

class SubProcessSerial:
    def __init__(self, executable):
        self.p = subprocess.Popen(executable, stdout=subprocess.PIPE, stdin=subprocess.PIPE)

    def readline(self):
        self.p.stdout.flush()
        msg = self.p.stdout.readline()
        print('--', msg)
        return msg

    def write(self, msg):
        self.p.stdin.write(msg)
        self.p.stdin.flush()

def run(**args):
    default = {}
    default["server_ip"]              = "localhost"
    default["server_backend_port"]    = 5001
    default["server_events_port"]     = 5003
    default["identity"]               = "executable"
    default["executable_path"]        = ""
    default["protocol_file"]          = ""
    default["protocol_prefixe"]       = "Q_"
    default["protocol_prefix_erreur"] = "E_"
    default.update(args)

    usage = "usage: %prog [options]"
    parser = optparse.OptionParser(usage,version="%prog 0.0")
    parser.add_option("-S", "--server-ip",
                      action="store", dest="server_ip", default=default["server_ip"],
                      help="IP du serveur zerobot")
    parser.add_option("-B", "--server-backend-port",
                      action="store", dest="server_backend_port", type="int", default=default["server_backend_port"],
                      help="Port backend du serveur zerobot")
    parser.add_option("-E", "--server-events-port",
                      action="store", dest="server_events_port", type="int", default=default["server_events_port"],
                      help="Port de publication des évènements du serveur zerobot")
    parser.add_option("-i", "--identity",
                      action="store", dest="identity", default=default["identity"],
                      help="Identité du service")
    parser.add_option("-s", "--exec",
                      action="store", dest="executable_path", default=default["executable_path"],
                      help="Fichier executable à utiliser")
    parser.add_option("-f", "--protocol-file",
                      action="store", dest="protocol_file", default=default["protocol_file"],
                      help="Emplacement du fichier protocole à utiliser")
    parser.add_option("-x", "--protocol-prefixe",
                      action="store", dest="protocol_prefixe", default=default["protocol_prefixe"],
                      help="Préfixe utilisé par le protocole pour les fonctions")
    parser.add_option("-y", "--protocol-prefixe-erreur",
                      action="store", dest="protocol_prefixe_erreur", default=default["protocol_prefix_erreur"],
                      help="Préfixe utilisé par le protocole pour les erreurs")

    (options, _args) = parser.parse_args()

    fake_ser = SubProcessSerial(options.executable_path)
    adapter = zerobot.ArduinoAdapter(
        identity = options.identity,
        conn_addr = "tcp://%s:%s" % (options.server_ip, options.server_backend_port),
        ev_push_addr = "tcp://%s:%s" % (options.server_ip, options.server_events_port),
        serial = fake_ser,
        protocol_file = options.protocol_file,
        prefix = options.protocol_prefixe,
        prefix_erreur = options.protocol_prefixe_erreur)

    adapter.start()
