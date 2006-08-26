#--------------------------------------------------------------------------
# (c) 2005 Vadim Macagon
#
# Contents are licensed under the Nebula license.
#--------------------------------------------------------------------------

from vstudio71 import vstudio71
from vstudio8 import vstudio8
from vstudio7 import vstudio7
from makefile import makefile
from doxygen import doxygen

class Factory:

    #--------------------------------------------------------------------------
    def __init__(self, buildSys):
        self.buildSys = buildSys
        self.generators = { 'vstudio71' : vstudio71(buildSys),
                            'vstudio7'  : vstudio7(buildSys),
                            'vstudio8'  : vstudio8(buildSys),
                            'makefile'  : makefile(buildSys),
                            'doxygen'   : doxygen(buildSys) }

    #--------------------------------------------------------------------------
    def GetGenerator(self, generatorName):
        return self.generators[generatorName]

    #--------------------------------------------------------------------------
    def GetGenerators(self):
        return self.generators

#--------------------------------------------------------------------------
# EOF
#--------------------------------------------------------------------------
