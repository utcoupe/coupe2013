
DONE = 1
BLUE = 10
RED = 20

HAUT = 0
BAS = 1

class InvalidRangee(Exception):
    """1 ou 2"""
    pass

class InvalidColonne(Exception):
    """de 1 à 8 pour HAUT, de 1 à 12 pour BAS"""
    pass

class Gateau:
    def __init__(self):
        self.bougies = [-1,] * 20
        self.done = [False,] * 20

    def merge_data_str(self, string):
        """Ajoute les données obtenues par la visio"""
        data = [int(x) for x in string.split(', ')]

        for i in range(20):
            if self.bougies[i] == -1:
                if data[i] % 2 == 1:
                    self.done[i] = False
                    self.bougies[i] = data[i] - 1
                else:
                    self.done[i] = True
                    self.bougies[i] = data[i]

    def set_done(self, index):
        """Marque une bougie comme enfoncée"""
        self.done[index] = True

    def get_left_todo(self, color):
        """Renvoie la liste des index des bougies non encore tapées de la couleur color"""
        todos = []
        for index, done in enumerate(self.done):
            if not(done) and self.bougies[index] == color:
                todos.append(index)
        return todos

    def get_index(self, rangee, colonne):
        """Retourn l'index à partir de rangee (HAUT ou BAS) et colonne"""
        if (rangee > 1):
            raise InvalidRangee(rangee)
        if (rangee == HAUT and colonne > 8) or (rangee == BAS and colonne > 12):
            raise InvalidColonne(colonne)

        return (rangee * 8) + colonne - 1

    def get_info_from_index(self, index):
        """Retourne (rangee, colonne) à partir d'un index"""
        if index <= 7:
            return (HAUT, index + 1)
        else:
            return (BAS, index - 7)
