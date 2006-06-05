#-------------------------------------------------------------------------------
# level.py
#-------------------------------------------------------------------------------

from opobject import OpObject
from envobject import EnvObject
from light import Light

class Level:
    def __init__(self):
        self._category = '.Levels'
        self.center = []
        self.extents = []
        self.id = ''
        self.startLevel = ''
        self.opObjects = []
        self.envObjects = []
        self.lights = []

    def loadDb3(self, db3File, id):
        self.id = id
        self.center = db3File.queryLevelData(id, 'Center').split(',')
        self.extents = db3File.queryLevelData(id, 'Extents').split(',')
        self.startLevel = db3File.queryLevelData(id, 'StartLevel')
        for i in db3File.queryCategoryNames():
            if i != 'Light':
                for j in db3File.queryEntityGuid(i, id):
                    opObject = OpObject(id)
                    opObject.loadDb3(db3File, j)
                    self.opObjects.append(opObject)
        for i in db3File.queryEntityGuid('_Environment', id):
            envObject = EnvObject(id)
            envObject.loadDb3(db3File, i)
            self.envObjects.append(envObject)
        for i in db3File.queryEntityGuid('Light', id):
            light = Light(id)
            light.loadDb3(db3File, i)
            self.lights.append(light)

    def saveDb3(self, db3File):
        if self.center:
            db3File.updateLevelData(self.id, 'Center', ', '.join(self.center))
        if self.extents:
            db3File.updateLevelData(self.id, 'Extents', ', '.join(self.extents))
        if self.startLevel:
            db3File.updateLevelData(self.id, 'StartLevel', self.startLevel)
        for i in self.opObjects:
            i.saveDb3(db3File)
        for i in self.envObjects:
            i.saveDb3(db3File)
        for i in self.lights:
            i.saveDb3(db3File)

    def setAttributes(self, attrs):
        if 'center' in attrs:
            self.center = attrs['center'].split()
        if 'extents' in attrs:
            self.extents = attrs['extents'].split()
        if 'startLevel' in attrs:
            self.startLevel = attrs['startLevel']
        if 'id' in attrs:
            self.id = attrs['id']

    def getAttributes(self):
        attrs = {}
        if len(self.center):
            attrs['center'] = ' '.join(self.center)
        if len(self.extents):
            attrs['extents'] = ' '.join(self.extents)
        if self.startLevel:
            attrs['startLevel'] = self.startLevel
        if self.id:
            attrs['id'] = self.id
        return attrs

#-------------------------------------------------------------------------------
# Eof
#-------------------------------------------------------------------------------