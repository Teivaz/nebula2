#-------------------------------------------------------------------------------
# entity.py
#-------------------------------------------------------------------------------

from transform import Transform
from guid import *

class Entity:
    def __init__(self):
        self._type = ''
        self._id = ''
        self._category = ''
        self._level = ''
        self.guid = guid()
        self.transform = Transform()

    def loadDb3(self, db3File, guid):
        self.guid = guid
        self._category = db3File.queryEntityData(guid, '_Category')
        self._level = db3File.queryEntityData(guid, '_Level')
        self.transform.matrix.set([float(i) for i in db3File.queryEntityData(guid, 'Transform').split(',')])
        self.transform.setMatrix()

    def saveDb3(self, db3File):
        if self._id:
            db3File.updateEntityData(self.guid, '_ID', self._id + ' (' + self._level + ')')
        if self._category:
            db3File.updateEntityData(self.guid, '_Category', self._category)
        if self._level:
            db3File.updateEntityData(self.guid, '_Level', self._level)
        if self.transform:
            db3File.updateEntityData(self.guid, 'Transform', ', '.join([str(i) for i in self.transform.matrix.get()]))

    def setAttributes(self, attrs):
        if 'name' in attrs:
            self._id = attrs['name']
        if 'pos' in attrs:
            self.transform.position = [float(i) for i in attrs['pos'].split()]
        if 'rot' in attrs:
            self.transform.rotation = [float(i) for i in attrs['rot'].split()]
        if 'scale' in attrs:
            self.transform.scale = [float(i) for i in attrs['scale'].split()]
        self.transform.getMatrix()

    def getAttributes(self):
        attrs = {}
        if self._id:
            attrs['name'] = self._id
        if len(self.transform.position):
            attrs['pos'] = ' '.join([str(i) for i in self.transform.position])
        if len(self.transform.rotation):
            attrs['rot'] = ' '.join([str(i) for i in self.transform.rotation])
        if len(self.transform.scale):
            attrs['scale'] = ' '.join([str(i) for i in self.transform.scale])
        return attrs

#-------------------------------------------------------------------------------
# Eof
#-------------------------------------------------------------------------------