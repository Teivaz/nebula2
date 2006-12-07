#-------------------------------------------------------------------------------
# opobject.py
#-------------------------------------------------------------------------------

from envobject import EnvObject

class OpObject(EnvObject):
    def __init__(self, level):
        EnvObject.__init__(self, level)
        self.id = ''
        self.placeholder = ''
        self.name = ''
        self.animSet = ''
        self.characterSet = ''
        self.inputFocus = ''
        self.cameraFocus = ''
        self.maxVelocity = ''
        self.fieldOfView = ''

    def loadDb3(self, db3File, guid):
        EnvObject.loadDb3(self, db3File, guid)
        self.id = db3File.queryEntityData(guid, 'Id')
        self.placeholder = db3File.queryEntityData(guid, 'Placeholder')
        self.Name = db3File.queryEntityData(guid, 'Name')
        self.animSet = db3File.queryEntityData(guid, 'AnimSet')
        self.characterSet = db3File.queryEntityData(guid, 'CharacterSet')
        self.inputFocus = db3File.queryEntityData(guid, 'InputFocus')
        self.cameraFocus = db3File.queryEntityData(guid, 'CameraFocus')
        self.maxVelocity = db3File.queryEntityData(guid, 'MaxVelocity')
        self.fieldOfView = db3File.queryEntityData(guid, 'FieldOfView')

    def saveDb3(self, db3File):
        EnvObject.saveDb3(self, db3File)
        db3File.updateCategoryName(self._category)
        if self.id:
            db3File.updateEntityData(self.guid, 'Id', self.id)
        if self.graphics:
            db3File.updateEntityCategory(self._category, self.id, 'Graphics', self.graphics)
            db3File.updateEntityData(self.guid, 'Graphics', self.graphics)
        if self.physics:
            db3File.updateEntityCategory(self._category, self.id, 'Physics', self.physics)
            db3File.updateEntityData(self.guid, 'Physics', self.physics)
        if self.placeholder:
            db3File.updateEntityCategory(self._category, self.id, 'Placeholder', self.placeholder)
            db3File.updateEntityData(self.guid, 'Placeholder', self.placeholder)
        if self.name:
            db3File.updateEntityCategory(self._category, self.id, 'Name', self.name)
            db3File.updateEntityData(self.guid, 'Name', self.name)
        if self.animSet:
            db3File.updateEntityCategory(self._category, self.id, 'AnimSet', self.animSet)
            db3File.updateEntityData(self.guid, 'AnimSet', self.animSet)
        if self.characterSet:
            db3File.updateEntityCategory(self._category, self.id, 'CharacterSet', self.characterSet)
            db3File.updateEntityData(self.guid, 'CharacterSet', self.characterSet)
        if self.inputFocus:
            db3File.updateEntityCategory(self._category, self.id, 'InputFocus', self.inputFocus)
            db3File.updateEntityData(self.guid, 'InputFocus', self.inputFocus)
        if self.cameraFocus:
            db3File.updateEntityCategory(self._category, self.id, 'CameraFocus', self.cameraFocus)
            db3File.updateEntityData(self.guid, 'CameraFocus', self.cameraFocus)
        if self.maxVelocity:
            db3File.updateEntityCategory(self._category, self.id, 'MaxVelocity', self.maxVelocity)
            db3File.updateEntityData(self.guid, 'MaxVelocity', self.maxVelocity)
        if self.fieldOfView:
            db3File.updateEntityCategory(self._category, self.id, 'FieldOfView', self.fieldOfView)
            db3File.updateEntityData(self.guid, 'FieldOfView', self.fieldOfView)

    def setAttributes(self, attrs):
        EnvObject.setAttributes(self, attrs)
        if 'loc' in attrs:
            str = attrs['loc'].split(':')
            self._category = str[1]
            self.id = str[2]
        if 'Placeholder' in attrs:
            self.placeholder = attrs['Placeholder']
        if 'Name' in attrs:
            self.name = attrs['Name']
        if 'AnimSet' in attrs:
            self.animSet = attrs['AnimSet']
        if 'CharacterSet' in attrs:
            self.characterSet = attrs['CharacterSet']
        if 'InputFocus' in attrs:
            self.inputFocus = attrs['InputFocus']
        if 'CameraFocus' in attrs:
            self.cameraFocus = attrs['CameraFocus']
        if 'MaxVelocity' in attrs:
            self.maxVelocity = attrs['MaxVelocity']
        if 'FieldOfView' in attrs:
            self.fieldOfView = attrs['FieldOfView']

    def getAttributes(self):
        attrs = EnvObject.getAttributes(self)
        if self._category or self.id:
            attrs['loc'] = 'db:' + self._category + ':' + self.id
        if self.placeholder:
            attrs['Placeholder'] = self.placeholder
        if self.name:
            attrs['Name'] = self.name
        if self.animSet:
            attrs['AnimSet'] = self.animSet
        if self.characterSet:
            attrs['CharacterSet'] = self.characterSet
        if self.inputFocus:
            attrs['InputFocus'] = self.inputFocus
        if self.cameraFocus:
            attrs['CameraFocus'] = self.cameraFocus
        if self.maxVelocity:
            attrs['MaxVelocity'] = self.maxVelocity
        if self.fieldOfView:
            attrs['FieldOfView'] = self.fieldOfView
        return attrs

#-------------------------------------------------------------------------------
# Eof
#-------------------------------------------------------------------------------