#--------------------------------------------------------------------------
#  Mangalore 3dsmax MAXScript Build System
#
#  build.py
#
#  (c)2007 Kim Hyoun Woo
#--------------------------------------------------------------------------

import sys
from builder.mangalore import *

#--------------------------------------------------------------------------
def PrintHelp():
    print """\
Usage:
    build.py [-help] [-xml] [-entity]

Example:
    build.py -xml mangalore.xml -entity entity.ms -table table.ms

Details:
-help
    You're reading it.
-xml
    Input XML file which describes columns for each of tables in database.
-entity
    A generated MAXScript filename which is used for custom attributes of  
    oentity bject.
"""

#--------------------------------------------------------------------------
def Build(xml, entity, table):

    mangalore = Mangalore()

    mangalore.Parse(xml)
    mangalore.WriteEntityScript(entity)
    mangalore.WriteTableScript(table)
    mangalore.WriteParserScript('parse.ms')
   
#--------------------------------------------------------------------------
def main():
    numCmdLineArgs = len(sys.argv)

    xmlFile = 'mangalore.xml'
    entityScript = 'entity.ms'
    tableScript = 'newdb.ms' #'functions.ms'

    if numCmdLineArgs  > 1:
        i = 1
        while i < numCmdLineArgs:
            if '-help' == sys.argv[i]:
                PrintHelp()
                return
            elif '-xml' == sys.argv[i]:
                i += 1
                xmlFile = sys.argv[i]
            elif '-entity' == sys.argv[i]:
                i += 1
                entityScript = sys.argv[i]
            else:
                i += 1 #Build(xmlFile, entityScript, tableScript) 
            i += 1

    Build(xmlFile, entityScript, tableScript)
    print "'%s' and '%s' MaxScript files are generated.\n" % (entityScript, tableScript)


if __name__ == '__main__':
    main()

#--------------------------------------------------------------------------
# EOF
#--------------------------------------------------------------------------

