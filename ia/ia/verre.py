import sys
sys.path.append('../../lib')

import utcoupe
from geometry import Vec

# On détermine la position des verres
pos_verres = [
    (900, 950),
    (1050, 1200),
    (900, 1450)
    ]
for i in range(3):
    i = pos_verres[i]
    pos_verres.append((i[0]+300, i[1]))
for i in range(6):
    i = pos_verres[i]
    pos_verres.append((3000-i[0], i[1]))

class Verre:
    def __init__(self, pos):
        self.pos = Vec(pos)
        self.moved = False # <=> est-ce qu'on robot adverse est passé dessus et l'a déplacé ?
        self.taken = False # <=> est-ce qu'on l'a pris ?

class Verres:
    def __init__(self):
        self.verres = []
        for i in pos_verres:
            self.verres.append(Verre(i))

    def pp(self):
        for verre in self.verres:
            print(verre.pos.x, verre.pos.y, verre.moved)

    def add_enemy_pos(self, positions):
        """ Si l'ennemi passe dans le champs des verres on suppose qu'il les a pris ou changé de place donc on dégage
        """
        for pos in positions:
            for verre in self.verres:
                if (verre.pos-pos).norm() < utcoupe.RAYON_ENEMY:
                       verre.moved = True
