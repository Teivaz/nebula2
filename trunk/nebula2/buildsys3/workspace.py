#--------------------------------------------------------------------------
# (c) 2005 Vadim Macagon
#
# Contents are licensed under the Nebula license.
#--------------------------------------------------------------------------

import os, string

class Workspace:
    
    #--------------------------------------------------------------------------
    def __init__(self, workspaceName, bldFilename):
        self.buildSys = None
        # collated defs for targets + global defs
        self.targetDefsAll = {} 
        # these more or less directly correspond to stuff in bld files
        self.name = workspaceName
        self.annotation = ''
        self.dir = '' # directory where workspace will be generated
        self.binaryDir = '' # directory for binary output
        self.targets = []
        self.globalDefs = []
        self.targetDefs = {}
        self.bldFile = bldFilename
    
    #--------------------------------------------------------------------------
    def Clean(self):
        self.annotation = string.strip(self.annotation, '" ')
        self.dir = self.buildSys.CleanRelPath(self.dir)
        self.binaryDir = self.buildSys.CleanRelPath(self.binaryDir)
    
    #--------------------------------------------------------------------------
    def Validate(self):
        dataValid = True
        # check if workspace contains targets
        if not len(self.targets):
            self.buildSys.logger.error('Workspace contains no targets' \
                                       ' in workspace %s from %s',
                                       self.name, self.bldFile)
            dataValid = False
        # check workspace targets are defined
        for targetName in self.targets:
            if targetName not in self.buildSys.targets:
                self.buildSys.logger.error('Undefined target %s referenced' \
                                           ' in workspace %s from %s',
                                           targetName, self.name, self.bldFile)
                dataValid = False
        return dataValid
        
    #--------------------------------------------------------------------------
    # Collect all global (defined for every target in the workspace) and per
    # target defines and store them in an easily retrievable form.
    def CollectPreprocessorDefs(self):
        numTargetsPresent = 0
        for targetName in self.targets:
            # will contain all the preprocessor defines for a particular target, 
            # where (key: value) -> (define name: value given to the define)
            preprocDefs = {}
            for globalDef in self.globalDefs:
                if len(globalDef) == 1:
                    preprocDefs[globalDef[0]] = ''
                elif len(globalDef) == 2:
                    preprocDefs[globalDef[0]] = globalDef[1]
            if targetName in self.targetDefs:
                numTargetsPresent += 1
                for targetDef in self.targetDefs[targetName]:
                    if len(targetDef) == 1:
                        preprocDefs[targetDef[0]] = ''
                    elif len(targetDef) == 2:
                        preprocDefs[targetDef[0]] = targetDef[1]
            self.targetDefsAll[targetName] = preprocDefs
            
        # figure out if there are any orphaned per target defines
        # i.e. per target defines for targets that aren't in the workspace
        numTargetsMissing = len(self.targetDefs) - numTargetsPresent
        if numTargetsMissing > 0:
            self.buildSys.logger.warning('Found per target defines for ' \
                                         'targets that have not been ' \
                                         'explicitely added to the workspace ' \
                                         '%s in %s', self.name, self.bldFile)
    
    #--------------------------------------------------------------------------
    def GetWorkspacePath(self, defaultPath):
        if '' == self.dir:
            return defaultPath
        else:
            return self.dir
        
    #--------------------------------------------------------------------------
    def GetBinaryOutputPath(self, defaultPath):
        if '' == self.binaryDir:
            return defaultPath
        else:
            return self.binaryDir
    
    #--------------------------------------------------------------------------
    def GetTargetDefsStringForTarget(self, targetName):
        defStr = ''
        if targetName in self.targetDefsAll:
            for defName, defValue in self.targetDefsAll[targetName].items():
                if '' == defValue:
                    defStr += defName + ';'
                else:
                    defStr += defName + '=' + defValue + ';'
        return defStr
        
    #--------------------------------------------------------------------------
    #  Returns a list of all valid include dir paths relative to the
    #  workspace path. This list is generated from a list of all modules used 
    #  within the workspace. The list is returned as a single string where each
    #  entry is separated by a semicolon.
    def GetIncSearchDirsString(self, defaultWorkspacePath):
        searchStr = ''
        incPaths = []
        workspacePath = self.GetWorkspacePath(defaultWorkspacePath)
        
        for targetName in self.targets:
            target = self.buildSys.targets[targetName]
            for moduleName in target.modules:
                module = self.buildSys.modules[moduleName]
                if '' != module.codeDir:
                    incPath = self.buildSys.FindRelPath(workspacePath,
                                                        module.GetBaseIncDir())
                    if incPath not in incPaths:
                        incPaths.append(incPath)

        for incPath in incPaths:
            searchStr += incPath + ';'

        return searchStr
        
    #--------------------------------------------------------------------------
    def GetLibSearchDirsString(self, libSubdir, defaultWorkspacePath):
        searchStr = ''
        libPaths = []
        workspacePath = self.GetWorkspacePath(defaultWorkspacePath)
        
        for targetName in self.targets:
            target = self.buildSys.targets[targetName]
            for moduleName in target.modules:
                module = self.buildSys.modules[moduleName]
                if '' != module.codeDir:
                    libPath = self.buildSys.FindRelPath(workspacePath,
                                                        os.path.join('code', 
                                                        module.codeDir, 'lib', 
                                                        libSubdir))
                    if libPath not in libPaths:
                        libPaths.append(libPath)

        for libPath in libPaths:
            searchStr += libPath + ';'

        return searchStr
        
    #--------------------------------------------------------------------------
    # When a target depends on other targets those targets have to be included
    # in the workspace in order for things to build properly. In the past you
    # had to figure out which target dependencies to add to the workspace on 
    # your own, that was rather tedious so now this method does it for you.
    def CollectTransitiveTargetDependencies(self):
        transitiveDeps = []
        for targetName in self.targets:
            target = self.buildSys.targets[targetName]
            for targetDepName in target.depends:
                if targetDepName not in transitiveDeps:
                    if targetDepName not in self.targets:
                        transitiveDeps.append(targetDepName)
        self.targets.extend(transitiveDeps)
        #print 'Transitive Dependecies for %s: %s' % (self.name, str(transitiveDeps))
    
#--------------------------------------------------------------------------
# EOF
#--------------------------------------------------------------------------
