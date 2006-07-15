#-------------------------------------------------------------------------------
# mangreader.py
#-------------------------------------------------------------------------------

from xml.sax.xmlreader import XMLReader

class MangReader(XMLReader):
    def __init__(self):
        XMLReader.__init__(self)

    def parse(self, mangLevel):
        self._cont_handler.startDocument()
        self.parseGlobal(mangLevel)
        self.parseLevels(mangLevel)
        self._cont_handler.endDocument()

# private
    def parseGlobal(self, mangLevel):
        self._cont_handler.ignorableWhitespace('\t')
        self._cont_handler.startElement('Global', mangLevel.getAttributes())
        self._cont_handler.endElement('Global')

    def parseLevels(self, mangLevel):
        self._cont_handler.startElement('MangaloreLevel', {'AppName' : 'Mangalore', 'Version' : '1.0'})
        for i in mangLevel.levels:
            self._cont_handler.ignorableWhitespace('\n\t')
            self._cont_handler.startElement('Level', i.getAttributes())
            self.parseOpObjects(i)
            self.parseEnvObjects(i)
            self.parseLights(i)
            self._cont_handler.ignorableWhitespace('\n\t')
            self._cont_handler.endElement('Level')
        self._cont_handler.ignorableWhitespace('\n')
        self._cont_handler.endElement('MangaloreLevel')

    def parseOpObjects(self, level):
        if len(level.opObjects):
            self._cont_handler.ignorableWhitespace('\n\t\t')
            self._cont_handler.startElement('Objects', {})
            for i in level.opObjects:
                self._cont_handler.ignorableWhitespace('\n\t\t\t')
                self._cont_handler.startElement('Object', i.getAttributes())
                self._cont_handler.endElement('Object')
            self._cont_handler.ignorableWhitespace('\n\t\t')
            self._cont_handler.endElement('Objects')

    def parseEnvObjects(self, level):
        if len(level.envObjects):
            self._cont_handler.ignorableWhitespace('\n\t\t')
            self._cont_handler.startElement('Environment', {})
            for i in level.envObjects:
                self._cont_handler.ignorableWhitespace('\n\t\t\t')
                self._cont_handler.startElement('EnvObject', i.getAttributes())
                self._cont_handler.endElement('EnvObject')
            self._cont_handler.ignorableWhitespace('\n\t\t')
            self._cont_handler.endElement('Environment')

    def parseLights(self, level):
        if len(level.lights):
            self._cont_handler.ignorableWhitespace('\n\t\t')
            self._cont_handler.startElement('Lights', {})
            for i in level.lights:
                self._cont_handler.ignorableWhitespace('\n\t\t\t')
                self._cont_handler.startElement('Light', i.getAttributes())
                self._cont_handler.endElement('Light')
            self._cont_handler.ignorableWhitespace('\n\t\t')
            self._cont_handler.endElement('Lights')

#-------------------------------------------------------------------------------
# Eof
#-------------------------------------------------------------------------------