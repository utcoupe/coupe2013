#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys
import os
DIR_PATH = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.join(DIR_PATH, ".."))

import traceback

import zerobot

class ServicesManager:
    def __init__(self, server, ctx):
        self.ctx = ctx
        self.services = {}
        self.server = server

    def create(self, name, obj):
        if not(name in self.services.keys()):
            service = EmulService(name, self.server["backend"], obj, ev_push_addr = self.server["ev_push"])
            self.services[name] = service
            service.start(False)
            return service

    def stop(self):
        for i in self.services.values():
            try:
                i.stop()
            except Exception:
                print("Unable to stop ", i)

class ResponseLater(Exception) : pass
class EmulService(zerobot.Service):
    """Un créateur de service particulier qui donne l'identifiant du message à chaque appel de fonction et permet d'y répondre plus tard si on renvoie un utcoupe.ResponseLater"""

    def __init__(self, *args, **kwargs):
        super(EmulService, self).__init__(*args, **kwargs)
        self.waiting_calls = {}

    def _process(self, fd, _ev):
        """
        Le message reçu est en 2 parties :
        1. remote_id
        2. packed request
        
        Une Request est un dictionnaire:
        {@code
        {
        uid: 	{str} unique id qu'il faudra renvoyer
        fct:	{str} la fonction à appeller
        args:	{list} les arguments
        kwargs:	{dict} les arguments només
        }
        }
        La fonction va unpack le message et la request pour extraire la fonction à appeller.
        """
        msg = fd.recv_multipart()
        self.logger.debug("worker %s recv %s", self.identity, msg)
        remote_id, packed_request = msg
        request = zerobot.Request.unpack(packed_request)
        err = None
        r = None
        try:
            # Get the requested function
            if request.fct=='help':
                f = self.help
            elif request.fct=='stop':
                f = self.stop
            elif request.fct=='ping':
                f = self.ping
            else:
                f = getattr(self.exposed_obj, request.fct)

            # Now call it
            if request.fct.startswith('_'):
                raise Exception("Method %s is protected" % request.fct)
            args,kwargs = request.args, request.kwargs
            if args and kwargs:
                r = f(request.uid, *args, **kwargs)
            elif kwargs:
                r = f(request.uid, **kwargs)
            else:
                r = f(request.uid, *args)
            if isinstance(r, ResponseLater):
                self.waiting_calls[request.uid] = remote_id
                return
        except Exception as ex:
            err = {}
            err['tb'] = traceback.format_exc()
            err['error'] = str(ex)
        response = zerobot.Response(request.uid, r, err)
        self.send_multipart([remote_id, response.pack()])

    def ping(self, uid, entier):
        return int(entier)+42

    def send_response(self, id_msg, resp):
        if id_msg in self.waiting_calls:
            response = zerobot.Response(id_msg, resp, None)
            self.send_multipart([self.waiting_calls[id_msg], response.pack()])
            del self.waiting_calls[id_msg]
