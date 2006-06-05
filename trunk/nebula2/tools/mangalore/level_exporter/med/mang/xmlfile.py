#-------------------------------------------------------------------------------
# xmlfile.py
#-------------------------------------------------------------------------------

class XmlFile:
    def __init__(self):
        self.pathName = ''
        self.content = ''

    def open(self, pathName):
        self.pathName = pathName
        f = open(pathName, 'r')
        self.content = f.read()
        f.close()

    def save(self):
        f = open(self.pathName, 'w')
        f.write(self.content)
        f.close()

    def saveAs(self, pathName):
        self.pathName = pathName
        f = open(pathName, 'w')
        f.write(self.content)
        f.close()

#-------------------------------------------------------------------------------
# Eof
#-------------------------------------------------------------------------------