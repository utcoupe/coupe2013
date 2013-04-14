
class SmartAsserv:
        def __init__(self, client):
                self.client = client

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

        def __getattr__(self, key):
                return self.client.__getattr__(key)
