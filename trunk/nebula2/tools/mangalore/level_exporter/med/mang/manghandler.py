#-------------------------------------------------------------------------------
# manghandler.py
#-------------------------------------------------------------------------------

import xml.sax as sax
from level import Level
from opobject import OpObject
from envobject import EnvObject
from light import Light

class MangHandler(sax.ContentHandler):
    def __init__(self, mangLevel):
        self.mangLevel = mangLevel
        self.level = None
        sax.ContentHandler.__init__(self)

    def startElement(self, name, attrs):
        if name == 'Global':
            self.mangLevel.setAttributes(attrs)
        elif name == 'Level':
            self.level = Level()
            self.level.setAttributes(attrs)
        elif name == 'Objects':
            pass
        elif name == 'Object':
            opObject = OpObject(self.level.id)
            opObject.setAttributes(attrs)
            self.level.opObjects.append(opObject)
        elif name == 'Environment':
            pass
        elif name == 'EnvObject':
            envObject = EnvObject(self.level.id)
            envObject.setAttributes(attrs)
            self.level.envObjects.append(envObject)
        elif name == 'Lights':
            pass
        elif name == 'Light':
            light = Light(self.level.id)
            light.setAttributes(attrs)
            self.level.lights.append(light)

    def endElement(self, name):
        if name == 'Global':
            pass
        elif name == 'Level':
            self.mangLevel.levels.append(self.level)
        elif name == 'Objects':
            pass
        elif name == 'Environment':
            pass
        elif name == 'Lights':
            pass

#-------------------------------------------------------------------------------
# Eof
#-------------------------------------------------------------------------------