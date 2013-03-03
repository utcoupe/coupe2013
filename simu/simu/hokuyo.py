
class Hokuyo:
	"""Émulateur pour l'hokuyo"""
	def __init__(self, robots):
		self.robots = robots

	def getdata(self, **kwargs):
		lr = filter(lambda r: r.x() > 0 and r.y() > 0, self.robots)
		reponse = str(tuple(map(lambda r: (r.x(),r.y()), self.robots)))
		reponse = reponse.replace(" ","")
		return reponse

	def ping(self):
		return "pong"
