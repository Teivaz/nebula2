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
        # check workspace targets are defined
        for targetName in self.targets:
            if targetName not in self.buildSys.targets:
                self.buildSys.logger.error('Undefined target %s referenced' \
                                           ' in workspace %s from %s',
                                           targetName, self.name, self.bldFile)
                dataValid = False
        return dataValid
        
    #--------------------------------------------------------------------------
    def CollectPreprocessorDefs(self):
        for targetName in self.targets:
            target = self.buildSys.targets[targetName]
            preprocDefs = {}
            for globalDef in self.globalDefs:
                if len(globalDef) == 1:
                    preprocDefs[globalDef[0]] = ''
                elif len(globalDef) == 2:
                    preprocDefs[globalDef[0]] = globalDef[1]
            if targetName in self.targetDefs:
                for targetDef in self.targetDefs[targetName]:
                    if len(targetDef) == 1:
                        preprocDefs[targetDef[0]] = ''
                    elif len(targetDef) == 2:
                        preprocDefs[targetDef[0]] = targetDef[1]
            self.targetDefsAll[targetName] = preprocDefs
    
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
    #  entry is separate by a semicolon.
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
                                                        os.path.join('code', 
                                                        module.codeDir, 'inc'))
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
# EOF
#--------------------------------------------------------------------------
