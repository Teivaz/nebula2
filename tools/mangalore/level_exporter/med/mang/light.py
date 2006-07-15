#-------------------------------------------------------------------------------
# light.py
#-------------------------------------------------------------------------------

from entity import Entity

class Light(Entity):
    def __init__(self, level):
        Entity.__init__(self)
        self._category = 'Light'
        self._level = level
        self.type = ''
        self.color = []
        self.range = ''
        self.ambient = []
        self.castShadows = ''
        self.time = ''

    def loadDb3(self, db3File, guid):
        Entity.loadDb3(self, db3File, guid)
        self.type = db3File.queryEntityData(guid, 'LightType')
        self.color = db3File.queryEntityData(guid, 'LightColor').split(',')
        self.range = db3File.queryEntityData(guid, 'LightRange')
        self.ambient = db3File.queryEntityData(guid, 'LightAmbient').split(',')
        self.castShadows = db3File.queryEntityData(guid, 'LightCastShadows')
        self.time = db3File.queryEntityData(guid, 'Time')

    def saveDb3(self, db3File):
        Entity.saveDb3(self, db3File)
        db3File.updateCategoryName('Light')
        if self.type:
            db3File.updateEntityData(self.guid, 'LightType', self.type)
        if self.color:
            db3File.updateEntityData(self.guid, 'LightColor', ', '.join(self.color))
        if self.range:
            db3File.updateEntityData(self.guid, 'LightRange', self.range)
        if self.ambient:
            db3File.updateEntityData(self.guid, 'LightAmbient', ', '.join(self.ambient))
        if self.castShadows:
            db3File.updateEntityData(self.guid, 'LightCastShadows', self.castShadows)
        if self.time:
            db3File.updateEntityData(self.guid, 'Time', self.time)

    def setAttributes(self, attrs):
        Entity.setAttributes(self, attrs)
        if 'type' in attrs:
            self.type = attrs['type']
        if 'color' in attrs:
            self.color = attrs['color'].split()
        if 'range' in attrs:
            self.range = attrs['range']
        if 'ambient' in attrs:
            self.ambient = attrs['ambient'].split()
        if 'castShadows' in attrs:
            self.castShadows = attrs['castShadows']
        if 'time' in attrs:
            self.time = attrs['time']

    def getAttributes(self):
        attrs = Entity.getAttributes(self)
        if self.type:
            attrs['type'] = self.type
        if len(self.color):
            attrs['color'] = ' '.join(self.color)
        if self.range:
            attrs['range'] = self.range
        if self.ambient:
            attrs['ambient'] = ' '.join(self.ambient)
        if self.castShadows:
            attrs['castShadows'] = self.castShadows
        if self.time:
            attrs['time'] = self.time
        return attrs

#-------------------------------------------------------------------------------
# Eof
#-------------------------------------------------------------------------------