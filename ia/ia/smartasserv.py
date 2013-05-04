
class SmartAsserv:
        def __init__(self, client):
                self.client = client
                self.recule = False

        def configure(self, ia):
                self.ia = ia
                self.gs = ia.gamestate
                self.gs.hokuyo.add_callback('', self.zb_cb)

        def zb_cb(self, msg):
                pass

        def s_goto(self, x, y, speed, **kwargs):
                return self.client.goto(x, y, speed, **kwargs)

        def s_gotor(self, x, y, speed, **kwargs):
                return self.client.goto(x, y, speed, **kwargs)

        def pwm(self, droite, gauche, temps, **kwargs):
                if droite < 0 or gauche < 0:
                        self.recule = True
                return self.client.pwm(gauche, droite, temps, **kwargs)

        def goto(self, a, b, c, **kwargs):
                self.recule = False
                if not('block' in kwargs.keys()):
                        kwargs['block'] = False
                return self.client.pwm(a, b, c, **kwargs)

        def __getattr__(self, key):
                return self.client.__getattr__(key)
