
import xml.dom.minidom

from .objects import *
from .define import *

def load_map(filename,engine):
	
	ofi = open(filename, 'r')
	document = ofi.read()
	ofi.close()
	
	dom = xml.dom.minidom.parseString(document)
	
	for cerise in dom.getElementsByTagName("cerise"):
		engine.add(Cerise(engine,mm_to_px(int(cerise.getAttribute("x")),int(cerise.getAttribute("y")))))
	for bougie in dom.getElementsByTagName("bougie"):
		engine.add(Bougie(engine,mm_to_px(int(bougie.getAttribute("x")),int(bougie.getAttribute("y"))),bougie.getAttribute("color")))
	for cadeau in dom.getElementsByTagName("cadeau"):
		engine.add(Cadeau(engine,mm_to_px(int(cadeau.getAttribute("x")),int(cadeau.getAttribute("y"))),cadeau.getAttribute("color")))
	for wall in dom.getElementsByTagName("wall"):
		engine.add(Wall(engine,mm_to_px(int(wall.getAttribute("x1")),int(wall.getAttribute("y1"))),mm_to_px(int(wall.getAttribute("x2")),int(wall.getAttribute("y2")))))
	for verre in dom.getElementsByTagName("verre"):
		engine.add(Verre(engine,mm_to_px(int(verre.getAttribute("x")),int(verre.getAttribute("y")))))
	for gateau in dom.getElementsByTagName("gateau"):
		engine.add(Gateau(engine,mm_to_px(int(gateau.getAttribute("x")),int(gateau.getAttribute("y")))))
