#-------------------------------------------------------------------------------
# transform.py
#-------------------------------------------------------------------------------

from matrix import Matrix

class Transform:
    def __init__(self):
        self.position = [0.0, 0.0, 0.0, 0.0]
        self.rotation = [0.0, 0.0, 0.0, 1.0]
        self.scale = [1.0, 1.0, 1.0]
        self.matrix = Matrix()

    def setMatrix(self):
        self.position = self.matrix.pos_component()

    def getMatrix(self):
        self.matrix.ident()
        self.matrix.scale(self.scale)
        quat = Matrix()
        quat.set_quat(self.rotation)
        self.matrix.mult_simple(quat)
        self.matrix.translate(self.position)

#-------------------------------------------------------------------------------
# Eof
#-------------------------------------------------------------------------------