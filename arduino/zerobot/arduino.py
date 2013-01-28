import sys
sys.path.append('../../../zerobot/python3')

import zerobot
import serial
import optparse
import logging
import os.path

logging.basicConfig(level=logging.DEBUG)

def run(file_, **args):
    default = {}
    default["server_ip"]           = "localhost"
    default["server_backend_port"] = 5001
    default["server_events_port"]  = 5003
    default["identity"]            = "arduino"
    default["serial_port"]         = "/dev/ttyACM0"
    default["serial_baudrate"]     = 115200
    default["protocol_file"]       = os.path.join(os.path.dirname(os.path.abspath(file_)), "driver", "protocole.h")
    default["protocol_prefixe"]    = "Q_"
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
    parser.add_option("-s", "--serial-port",
                      action="store", dest="serial_port", default=default["serial_port"],
                      help="Serial port")
    parser.add_option("-p", "--serial-baudrate",
                      action="store", dest="serial_baudrate", default=default["serial_baudrate"],
                      help="Serial baudrate")
    parser.add_option("-f", "--protocol-file",
                      action="store", dest="protocol_file", default=default["protocol_file"],
                      help="Emplacement du fichier protocole à utiliser")
    parser.add_option("-x", "--protocol-prefixe",
                      action="store", dest="protocol_prefixe", default=default["protocol_prefixe"],
                      help="Préfixe utilisé par le protocole")

    (options, _args) = parser.parse_args()

    print(options.protocol_file)

    ser = serial.Serial(options.serial_port, options.serial_baudrate)
    adapter = zerobot.ArduinoAdapter(
        identity = options.identity,
        conn_addr = "tcp://%s:%s" % (options.server_ip, options.server_backend_port),
        ev_push_addr = "tcp://%s:%s" % (options.server_ip, options.server_events_port),
        serial = ser,
        protocol_file = options.protocol_file,
        prefix = options.protocol_prefixe)

    adapter.start()
