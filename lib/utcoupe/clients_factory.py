import sys
import zmq
import os
DIR_PATH = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.join(DIR_PATH, ".."))

import zerobot

import logging
logger = logging.getLogger(__name__)

# La machine à clients !
# L'idée est que dans un même script les adresses d'accès au serveur seront toujours
# les mêmes donc on les donne une fois en créant le ClientsFactory
# ensuite on appelle simplement la methode get_client("nom-du-service-distant")
class ClientsFactory:
    def __init__(self, factory_id, server, ctx=None):
        self.factory_id = factory_id
        self.server = server
        self.ctx = zmq.Context() if ctx is None else ctx
        self.clients = {}

    def get_client(self, service, *, async=False):
        client_type = zerobot.AsyncClient if async else zerobot.Client

        client = client_type("%s-%s" % (self.factory_id, service),
                             self.server["frontend"], service,
                             ev_sub_addr = self.server["ev_sub"],
                             ctx = self.ctx)
        client.start()
        return client

    def stop(self):
        raise Exception('Not implemented yet')