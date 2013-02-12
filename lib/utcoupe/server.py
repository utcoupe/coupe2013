
class Server:
    def __init__(self, *, server = "localhost",
                 frontend = 5000, backend = 5001,
                 ev_push = 5003, ev_sub = 5004):
        self.server = server
        self.frontend = frontend
        self.backend = backend
        self.ev_push = ev_push
        self.ev_sub = ev_sub

    def complete(self, port):
        return "tcp://%s:%s" % (self.server, port)

    def __getitem__(self, what):
        if what in ("frontend", "backend", "ev_push", "ev_sub"):
            return self.complete(self.__dict__[what])
        else:
            raise ValueError("Invalid zerobot port type")
