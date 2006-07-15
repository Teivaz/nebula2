#-------------------------------------------------------------------------------
# mangalorelevel.py
#-------------------------------------------------------------------------------

import xml.sax as sax
from level import Level
from manghandler import MangHandler
from mangreader import MangReader
from manggenerator import MangGenerator

class MangaloreLevel:
    def __init__(self):
        self.levels = []
        self.currentLevel = ''

    def loadDb3(self, db3File):
        for i in db3File.queryLevelIds():
            level = Level()
            level.loadDb3(db3File, i)
            self.levels.append(level)
        self.currentLevel = db3File.queryGlobalValue('CurrentLevel')

    def saveDb3(self, db3File):
        for i in self.levels:
            i.saveDb3(db3File)
        if self.currentLevel:
            if db3File.hasGlobalValue('CurrentLevel'):
                db3File.updateGobalValue('CurrentLevel', self.currentLevel)
            else:
                db3File.addGlobalValue('CurrentLevel', 'x', self.currentLevel)

    def loadXml(self, pathName):
        parser = sax.make_parser()
        parser.setContentHandler(MangHandler(self))
        parser.parse(pathName)

    def saveXml(self, pathName):
        parser = MangReader()
        parser.setContentHandler(MangGenerator(file(pathName, 'w')))
        parser.parse(self)

    def setAttributes(self, attrs):
        if 'currentLevel' in attrs:
            self.currentLevel = attrs['currentLevel']

    def getAttributes(self):
        attrs = {}
        if self.currentLevel:
            attrs['currentLevel'] = self.currentLevel
        return attrs

#-------------------------------------------------------------------------------
# Eof
#-------------------------------------------------------------------------------