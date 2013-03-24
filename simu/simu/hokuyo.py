
class Hokuyo:
	"""Émulateur pour l'hokuyo"""
	def __init__(self, robots):
		self.robots = robots

	def setRed(self, id_msg):
		pass

	def setBlue(self, id_msg):
		pass

	def send_pos(self, id_msg):
		lr = filter(lambda r: r.x() > 0 and r.y() > 0, self.robots)
		reponse = str(tuple(map(lambda r: (r.x(),r.y()), self.robots)))
		reponse = reponse.replace(" ","")
		return reponse

	def ping(self, id_msg):
		return "pong"
