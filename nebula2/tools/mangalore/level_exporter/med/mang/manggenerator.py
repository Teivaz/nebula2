#-------------------------------------------------------------------------------
# manggenerator.py
#-------------------------------------------------------------------------------

from xml.sax.saxutils import XMLGenerator

class MangGenerator(XMLGenerator):
    def __init__(self, out = None, encoding = 'ascii', standalone = 'yes'):
        XMLGenerator.__init__(self, out, encoding)
        self._standalone = standalone

    def startDocument(self):
        self._write('<?xml version="1.0" encoding="%(encoding)s" standalone="%(standalone)s" ?>\n' %
                    {'encoding' : self._encoding, 'standalone' : self._standalone})

#-------------------------------------------------------------------------------
# Eof
#-------------------------------------------------------------------------------