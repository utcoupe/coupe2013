
import time
from geometry import Segment, Vec

class SmartAsserv:
        def __init__(self, client):
                self.client = client
                self.recule = False
                self.goal = None

        def configure(self, ia, robot):
                self.ia = ia
                self.gs = ia.gamestate
                #self.gs.hokuyo.add_callback('', self.zb_cb)
                self.robot = robot

        def zb_cb(self, msg):
                pass

        def s_goto(self, x, y, speed, **kwargs):
                return self.client.goto(x, y, speed, **kwargs)

        def s_gotor(self, x, y, speed, **kwargs):
                return self.client.goto(x, y, speed, **kwargs)

        def pwm(self, droite, gauche, temps, **kwargs):
                self.goal = {'type':'pwm', 'droite':droite, 'gauche':gauche, 'temps': temps, 'start':time.time(), 'k':kwargs}
                if droite < 0 or gauche < 0:
                        self.recule = True
                return self.client.pwm(gauche, droite, temps, **kwargs)

        def goto(self, a, b, c, **kwargs):
                self.recule = False
                if not('block' in kwargs.keys()):
                        kwargs['block'] = False
                self.goal = {'type': 'goto', 'x':a, 'y':b, 'speed':c, 'k':kwargs}
                return self.client.goto(a, b, c, **kwargs)

        def gotor(self, a, b, c, **kwargs):
                self.recule = False
                if not('block' in kwargs.keys()):
                        kwargs['block'] = False
                self.goal = {'type': 'gotor', 'pos':Vec(a,b), 'speed':c, 'start': self.pos, 'k':kwargs}
                return self.client.gotor(a, b, c, **kwargs)

        def turn(self, a, b, **kwargs):
                self.recule = False
                if not('block' in kwargs.keys()):
                        kwargs['block'] = False
                self.goal = {'type': 'turn', 'angle':a, 'speed':b, 'k':kwargs}
                return self.client.turn(a, b, **kwargs)

        def turnr(self, a, b, **kwargs):
                self.recule = False
                if not('block' in kwargs.keys()):
                        kwargs['block'] = False
                self.goal = {'type': 'turnr', 'angle':a, 'speed':b, 'start': self.a, 'k':kwargs}
                return self.client.turnr(a, b, **kwargs)

        #On recommence après avoir été interrompus
        def resume(self):
                if not(self.goal is None):
                        if self.goal['type'] == 'goto':
                                self.goto(self.goal['x'], self.goal['y'], self.goal['speed'], **self.goal['k'])
                        if self.goal['type'] == 'gotor':
                                # On corrige la position souhaitée
                                (x, y) = self.goal['pos'] - (self.goal['start'] - self.pos)
                                self.gotor(x, y, self.goal['speed'], **self.goal['k'])
                        if self.goal['type'] == 'turn':
                                self.turn(self.goal['angle'], self.goal['speed'], **self.goal['k'])
                        if self.goal['type'] == 'turnr':
                                # On corrige l'angle souhaité
                                angle = self.goal['a'] - (self.a - self.goal['start'])
                                self.turnr(angle, self.goal['speed'], **self.goal['k'])
                        if self.goal['type'] == 'pwm':
                                # On corrige le temps d'avance
                                temps = self.goal['temps'] - (time.time() - self.goal['start'])
                                self.pwm(self.goal['droite'], self.goal['gauche'], temps, **self.goal['k'])
                        else:
                                self.goal = None

        def __getattr__(self, key):
                return self.client.__getattr__(key)
