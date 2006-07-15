#-------------------------------------------------------------------------------
# transform.py
#-------------------------------------------------------------------------------

class Matrix:
    def __init__(self):
        self.mat = [[0.0 for i in range(4)] for j in range(4)]
        x = 100

    def ident(self):
        self.mat[0][0] = 1.0
        self.mat[0][1] = 0.0
        self.mat[0][2] = 0.0
        self.mat[0][3] = 0.0

        self.mat[1][0] = 0.0
        self.mat[1][1] = 1.0
        self.mat[1][2] = 0.0
        self.mat[1][3] = 0.0

        self.mat[2][0] = 0.0
        self.mat[2][1] = 0.0
        self.mat[2][2] = 1.0
        self.mat[2][3] = 0.0

        self.mat[3][0] = 0.0
        self.mat[3][1] = 0.0
        self.mat[3][2] = 0.0
        self.mat[3][3] = 1.0

    def set(self, mat):
        for i in range(4):
            self.mat[i][0] = mat[i*4]
            self.mat[i][1] = mat[i*4+1]
            self.mat[i][2] = mat[i*4+2]
            self.mat[i][3] = mat[i*4+3]

    def get(self):
        mat = []
        for i in range(4):
            mat.append(self.mat[i][0])
            mat.append(self.mat[i][1])
            mat.append(self.mat[i][2])
            mat.append(self.mat[i][3])
        return mat

    def set_quat(self, quat):
        x2 = quat[0] + quat[0]
        y2 = quat[1] + quat[1]
        z2 = quat[2] + quat[2]
        xx = quat[0]*x2
        xy = quat[0]*y2
        xz = quat[0]*z2
        yy = quat[1]*y2
        yz = quat[1]*z2
        zz = quat[2]*z2
        wx = quat[3]*x2
        wy = quat[3]*y2
        wz = quat[3]*z2
        self.mat[0][0] = 1.0 - (yy + zz)
        self.mat[1][0] = xy - wz
        self.mat[2][0] = xz + wy
        self.mat[0][1] = xy + wz
        self.mat[1][1] = 1.0 - (xx + zz)
        self.mat[2][1] = yz - wx
        self.mat[0][2] = xz - wy
        self.mat[1][2] = yz + wx
        self.mat[2][2] = 1.0 - (xx + yy)
        self.mat[3][0] = 0.0
        self.mat[3][1] = 0.0
        self.mat[3][2] = 0.0
        self.mat[0][3] = 0.0
        self.mat[1][3] = 0.0
        self.mat[2][3] = 0.0
        self.mat[3][3] = 1.0

    def scale(self, scale):
        for i in range(4):
            self.mat[i][0] = self.mat[i][0]*scale[0]
            self.mat[i][1] = self.mat[i][1]*scale[1]
            self.mat[i][2] = self.mat[i][2]*scale[2]

    def mult_simple(self, other):
        for i in range(4):
            mi0 = self.mat[i][0]
            mi1 = self.mat[i][1]
            mi2 = self.mat[i][2]
            self.mat[i][0] = mi0*other.mat[0][0] + mi1*other.mat[1][0] + mi2*other.mat[2][0]
            self.mat[i][1] = mi0*other.mat[0][1] + mi1*other.mat[1][1] + mi2*other.mat[2][1]
            self.mat[i][2] = mi0*other.mat[0][2] + mi1*other.mat[1][2] + mi2*other.mat[2][2]
        self.mat[3][0] = self.mat[3][0] + other.mat[3][0]
        self.mat[3][1] = self.mat[3][1] + other.mat[3][1]
        self.mat[3][2] = self.mat[3][2] + other.mat[3][2]
        self.mat[0][3] = 0.0
        self.mat[1][3] = 0.0
        self.mat[2][3] = 0.0
        self.mat[3][3] = 1.0

    def translate(self, trans):
        self.mat[3][0] = self.mat[3][0] + trans[0]
        self.mat[3][1] = self.mat[3][1] + trans[1]
        self.mat[3][2] = self.mat[3][2] + trans[2]

    def pos_component(self):
        return [self.mat[3][0], self.mat[3][1], self.mat[3][2]]

#-------------------------------------------------------------------------------
# Eof
#-------------------------------------------------------------------------------