#--------------------------------------------------------------------------
# (c) 2005 Vadim Macagon
#
# Contents are licensed under the Nebula license.
#--------------------------------------------------------------------------

class Bundle:
    
    #--------------------------------------------------------------------------
    def __init__(self, bundleName, bldFilename):
        self.buildSys = None
        # these more or less directly correspond to stuff in bld files
        self.name = bundleName
        self.annotation = ''
        self.modules = []
        self.targets = []
        self.bldFile = bldFilename
    
    #--------------------------------------------------------------------------
    def Validate(self):
        dataValid = True
        # check bundle modules are defined
        for moduleName in self.modules:
            if moduleName not in self.buildSys.modules:
                self.buildSys.logger.error('Undefined module %s referenced' \
                                           ' in bundle %s from %s', moduleName, 
                                           self.name, self.bldFile)
                dataValid = False
        # check bundle targets are defined
        for targetName in self.targets:
            if targetName not in self.buildSys.targets:
                self.buildSys.logger.error('Undefined target %s referenced' \
                                           ' in bundle %s from %s', targetName, 
                                           self.name, self.bldFile)
                dataValid = False
        return dataValid
    
#--------------------------------------------------------------------------
# EOF
#--------------------------------------------------------------------------
