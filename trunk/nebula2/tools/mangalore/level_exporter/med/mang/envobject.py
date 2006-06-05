#-------------------------------------------------------------------------------
# envobject.py
#-------------------------------------------------------------------------------

from entity import Entity

class EnvObject(Entity):
    def __init__(self, level):
        Entity.__init__(self)
        self._category = '_Environment'
        self._level = level
        self.animPath = ''
        self.graphics = ''
        self.physics = ''

    def loadDb3(self, db3File, guid):
        Entity.loadDb3(self, db3File, guid)
        self.animPath = db3File.queryEntityData(guid, 'AnimPath')
        self.graphics = db3File.queryEntityData(guid, 'Graphics')
        self.physics = db3File.queryEntityData(guid, 'Physics')

    def saveDb3(self, db3File):
        Entity.saveDb3(self, db3File)
        if self.animPath:
            db3File.updateEntityData(self.guid, 'AnimPath', self.animPath)
        if self.graphics:
            db3File.updateEntityData(self.guid, 'Graphics', self.graphics)
        if self.physics:
            db3File.updateEntityData(self.guid, 'Physics', self.physics)

    def setAttributes(self, attrs):
        Entity.setAttributes(self, attrs)
        if 'res' in attrs:
            self.graphics = attrs['res']
            self.physics = attrs['res']
        if 'AnimPath' in attrs:
            self.animPath = attrs['AnimPath']
        if 'Graphics' in attrs:
            self.graphics = attrs['Graphics']
        if 'Physics' in attrs:
            self.physics = attrs['Physics']

    def getAttributes(self):
        attrs = Entity.getAttributes(self)
        if self.graphics:
            attrs['res'] = self.graphics
        if self.animPath:
            attrs['AnimPath'] = self.animPath
        if self.graphics:
            attrs['Graphics'] = self.graphics
        if self.physics:
            attrs['Physics'] = self.physics
        return attrs

#-------------------------------------------------------------------------------
# Eof
#-------------------------------------------------------------------------------