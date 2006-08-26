#--------------------------------------------------------------------------
# 3rd Generation Nebula 2 Build System
#
# (c) 2005 Vadim Macagon
#
# Contents are licensed under the Nebula license.
#--------------------------------------------------------------------------

import os, glob, logging, sys
from bldscanner import *
from buildconfig import *
from workspace import Workspace
from target import Target
from bundle import Bundle
from module import Module
from generators.factory import Factory as GeneratorFactory

class BuildSysError(Exception):
    def __init__(self, value):
        self.value = value
    def __str__(self):
        return `self.value`


class BuildSys:
    platformTagDefMap = {'win32' : '__WIN32__',
                         'linux' : '__LINUX__',
                         'macosx': '__MACOSX__'}

    #--------------------------------------------------------------------------
    def __init__(self, homeDir, fileName, enableGUI = False):
        self.logHandler = None
        self.logger = None
        self.CreateDefaultLogger()
        self.showProgressDialog = False
        self.generators = []
        self.cfgFileName = fileName
        self.buildConfig = BuildConfig()
        self.useGUI = enableGUI
        self.mainFrame = None # this will be set later if running in GUI mode
        #self.errors = []
        self.homeDir = homeDir
        # these will be filled in Prepare()
        self.modules = {}
        self.targets = {}
        self.bundles = {}
        self.workspaces = {}
        self.projectDirs = []
        self.contribDirs = []
        self.generatorFactory = GeneratorFactory(self)
        # these will be set in AttachProgressDialog, if at all
        self.createProgressDlgFunc = None
        self.updateProgressDlgFunc = None
        self.progressDlgCancelledFunc = None
        self.destroyProgressDlgFunc = None
        # this will be set in AttachSummaryDialog
        self.displaySummaryDlgFunc = None

    #--------------------------------------------------------------------------
    # Return True if the build system is running in GUI mode and thus wxPython
    # can be used, or False if it's running in command line mode where
    # wxPython can't be used.
    def GUIEnabled(self):
        return self.useGUI

    #--------------------------------------------------------------------------
    def SetMainFrame(self, mainFrame):
        self.mainFrame = mainFrame

    #--------------------------------------------------------------------------
    def GetMainFrame(self):
        return self.mainFrame

    #--------------------------------------------------------------------------
    def GetBuildConfigSetting(self, settingName):
        return self.buildConfig.cfg.get(settingName, None)

    #--------------------------------------------------------------------------
    def SetBuildConfigSetting(self, settingName, settingValue):
        self.buildConfig.cfg[settingName] = settingValue

    #--------------------------------------------------------------------------
    def SaveBuildConfig(self):
        self.buildConfig.Write(os.path.join(self.homeDir, 'user.build.cfg.py'))

    #--------------------------------------------------------------------------
    def CreateDefaultLogger(self):
        self.logger = logging.getLogger('N2-BuildSystem')
        self.logHandler = logging.StreamHandler(sys.stdout)
        formatter = logging.Formatter('%(asctime)s %(levelname)s %(message)s',
                                      '%X')
        self.logHandler.setFormatter(formatter)
        self.logger.addHandler(self.logHandler)
        self.logger.setLevel(logging.DEBUG) # log everything

    #--------------------------------------------------------------------------
    def AttachProgressDialog(self, createFunc, updateFunc, cancelFunc,
                             destroyFunc):
        self.createProgressDlgFunc = createFunc
        self.updateProgressDlgFunc = updateFunc
        self.progressDlgCancelledFunc = cancelFunc
        self.destroyProgressDlgFunc = destroyFunc

    #--------------------------------------------------------------------------
    # Toggle whether a progress dialog will be displayed showing the progress
    # of the build system when run in GUI mode.
    def ShowProgressDialog(self, show):
        self.showProgressDialog = show

    #--------------------------------------------------------------------------
    def CreateProgressDialog(self, title, message, maxVal = 100,
                             canAbort = True):
        if self.showProgressDialog:
            if self.createProgressDlgFunc != None:
                self.createProgressDlgFunc(title, message, maxVal, canAbort)

    #--------------------------------------------------------------------------
    def UpdateProgressDialog(self, val, message):
        if self.showProgressDialog:
            if self.updateProgressDlgFunc != None:
                self.updateProgressDlgFunc(val, message)

    #--------------------------------------------------------------------------
    def ProgressDialogCancelled(self):
        if self.showProgressDialog:
            if self.progressDlgCancelledFunc != None:
                return self.progressDlgCancelledFunc()
        return False

    #--------------------------------------------------------------------------
    def DestroyProgressDialog(self):
        if self.showProgressDialog:
            if self.destroyProgressDlgFunc != None:
                self.destroyProgressDlgFunc()

    #--------------------------------------------------------------------------
    def AttachSummaryDialog(self, displayFunc):
        self.displaySummaryDlgFunc = displayFunc

    #--------------------------------------------------------------------------
    def DisplaySummaryDialog(self, details):
        if self.displaySummaryDlgFunc != None:
            self.displaySummaryDlgFunc(details)

    #--------------------------------------------------------------------------
    def GetGenerators(self):
        return self.generatorFactory.GetGenerators()

    #--------------------------------------------------------------------------
    def HasGenerator(self, generatorName):
        return (generatorName in self.generatorFactory.GetGenerators())

    #--------------------------------------------------------------------------
    # Pre-process bld files
    def Prepare(self):
        try:
            self.findCodeDirs()
            if self.buildConfig.Read(self.cfgFileName):
                #print self.buildConfig.bldFiles
                for bldFileName in self.buildConfig.bldFiles:
                    # parse bld file
                    scanner = None
                    try:
                        bldFile = file(bldFileName, 'r')
                    except IOError:
                        self.logger.exception("Couldn't read " + bldFileName)
                    else:
                        scanner = BldScanner(bldFile, bldFileName, self.logger)
                        value, text = scanner.read()
                        bldFile.close()
                    if scanner != None:
                        # collect modules
                        keepGoing = True
                        for module in scanner.modules:
                            if not self.addModule(module):
                                keepGoing = False
                        if not keepGoing:
                            return False
                        # collect targets
                        for target in scanner.targets:
                            if not self.addTarget(target):
                                keepGoing = False
                        if not keepGoing:
                            return False
                        # collect bundles
                        for bundle in scanner.bundles:
                            if not self.addBundle(bundle):
                                keepGoing = False
                        if not keepGoing:
                            return False
                        # collect workspaces
                        for workspace in scanner.workspaces:
                            if not self.addWorkspace(workspace):
                                keepGoing = False
                        if not keepGoing:
                            return False
                # now that we have everything do some more checks
                if self.validateInput():
                    return self.processInput()
                else:
                    return False
            else:
                self.logger.error('BuildSys.Prepare(): Failed to read ' \
                                  + self.cfgFileName)
                return False
        except:
            self.logger.exception('BuildSys.Prepare()')
            return False

    #--------------------------------------------------------------------------
    # Generate pkg files, resource files and project/make files
    def Run(self, generatorName, workspacesToBuild):
        try:
            # make sure the pkg directory exists
            pkgDirectory = os.path.join(self.homeDir, 'build', 'pkg')
            if not os.path.exists(pkgDirectory):
                os.makedirs(pkgDirectory)
            # first spit out the pkg and resource files
            for target in self.targets.values():
                target.GeneratePkgFile()
                target.GenerateResFile()
            # figure out which workspaces to generate
            if len(workspacesToBuild) == 0: # build all workspaces
                workspacesToBuild = self.workspaces.keys()
            else:
                availableWorkspaces = self.workspaces.keys()
                for workspaceName in workspacesToBuild:
                    if workspaceName not in availableWorkspaces:
                        self.logger.error('Workspace %s is undefined!',
                                          workspaceName)
                        return
            # create the generator and get it running
            generator = self.generatorFactory.GetGenerator(generatorName)
            generator.Generate(workspacesToBuild)
        except:
            self.logger.exception('Exception in BuildSys.Run()')

    #-------------------------------------------------------------------------
    # Private Stuff
    #-------------------------------------------------------------------------

    #--------------------------------------------------------------------------
    def findCodeDirs(self):
        #print 'Find Code Dirs:'
        searchPath = os.path.join(self.homeDir, 'code', '*') + os.sep
        #print 'Search Path: ' + searchPath
        self.projectDirs = glob.glob(searchPath)
        searchPath = os.path.join(self.homeDir, 'code', 'contrib', '*') + os.sep
        #print 'Search Path: ' + searchPath
        self.contribDirs = glob.glob(searchPath)
        # strip the trailing slash and remove CVS directories from the paths
        while True:
            matchIdx = -1
            for i in range(len(self.projectDirs)):
                self.projectDirs[i] = string.rstrip(self.projectDirs[i], os.sep)
                head, tail = os.path.split(self.projectDirs[i])
                if string.upper(tail) == 'CVS':
                    matchIdx = i
                    break;
            if matchIdx != -1:
                del self.projectDirs[matchIdx]
            else:
                break;
        # same with the contrib paths
        while True:
            matchIdx = -1
            for i in range(len(self.contribDirs)):
                self.contribDirs[i] = string.rstrip(self.contribDirs[i], os.sep)
                head, tail = os.path.split(self.contribDirs[i])
                if string.upper(tail) == 'CVS':
                    matchIdx = i
                    break;
            if matchIdx != -1:
                del self.contribDirs[matchIdx]
            else:
                break;

        #print self.projectDirs
        #print self.contribDirs

    #--------------------------------------------------------------------------
    def addModule(self, module):
        modName = module.name
        if modName in self.modules:
            self.logger.error('Module names must be unique!\n' \
                              'The module %s from %s conflicts with a module' \
                              ' of the same name from %s', modName,
                              module.bldFile, self.modules[modName].bldFile)
            return False
        else:
            module.buildSys = self
            module.Clean()
            self.modules[modName] = module
            return True

    #--------------------------------------------------------------------------
    def addTarget(self, target):
        tarName = target.name
        if tarName in self.targets:
            self.logger.error('Target names must be unique!\n' \
                              'The target %s from %s conflicts with a target' \
                              ' of the same name from %s', tarName,
                              target.bldFile, self.targets[tarName].bldFile)
            return False
        else:
            target.buildSys = self
            target.Clean()
            self.targets[tarName] = target
            return True

    #--------------------------------------------------------------------------
    def addBundle(self, bundle):
        bunName = bundle.name
        if bunName in self.bundles:
            self.logger.error('Bundle names must be unique!\n' \
                              'The bundle %s from %s conflicts with a bundle' \
                              ' of the same name from %s', bunName,
                              bundle.bldFile, self.bundles[bunName].bldFile)
            return False
        else:
            bundle.buildSys = self
            self.bundles[bunName] = bundle
            return True

    #--------------------------------------------------------------------------
    def addWorkspace(self, workspace):
        worName = workspace.name
        if worName in self.workspaces:
            self.logger.error('Workspace names must be unique!\n' \
                              'The workspace %s from %s conflicts with a' \
                              ' workspace of the same name from %s', worName,
                              workspace.bldFile,
                              self.workspaces[worName].bldFile)
            return False
        else:
            workspace.buildSys = self
            workspace.Clean()
            self.workspaces[worName] = workspace
            return True

    #--------------------------------------------------------------------------
    def validateInput(self):
        dataValid = True
        for workspace in self.workspaces.values():
            if not workspace.Validate():
                dataValid = False
        # TODO: module validation?
        for bundle in self.bundles.values():
            if not bundle.Validate():
                dataValid = False
        for target in self.targets.values():
            if not target.Validate():
                dataValid = False
        return dataValid

    #--------------------------------------------------------------------------
    # Process modules, bundles, targets etc.
    def processInput(self):
        dataValid = True
        # process modules
        for module in self.modules.itervalues():
            module.ResolvePaths()
            if module.putInPkg:
                if not module.FindAncestor():
                    dataValid = False
        # bail out if module processing failed
        if not dataValid:
            return False
        # process targets
        for target in self.targets.itervalues():
            target.MergeBundles()
            target.AssesPlatformSupport()
            target.FindModuleDefFile()
            target.CollectLibsFromMods()
        # we can only do this once all targets have been processed
        for target in self.targets.itervalues():
            target.CollectLibsFromDepends()
        # second pass since the first pass may not have picked everything up
        for target in self.targets.itervalues():
            target.CollectLibsFromDepends()
        # finalize modules and targets
        for module in self.modules.itervalues():
            module.Finalize()
        for target in self.targets.itervalues():
            target.Finalize()
        # process workspaces
        for workspace in self.workspaces.itervalues():
            workspace.CollectPreprocessorDefs()
            # Doing this after workspace.CollectPreprocessorDefs() means that
            # any defines for transitive targets that haven't been explicitely
            # added to the workspace will be ignored. I think that's a good
            # thing, because transitive dependencies are supposed to be taken
            # care of by the build system, if you have to add defines to them
            # then you might as well list them in the workspace explicitely.
            workspace.CollectTransitiveTargetDependencies()
        return True

    #--------------------------------------------------------------------------
    def CleanRelPath(self, relPath):
        if '' != relPath:
            if len(relPath) > 2:
                # strip off the ./ at the start
                if './' == relPath[:2]:
                    relPath = relPath[2:]
                # now replace any / with the platform specific equivalent
                relPath = string.replace(relPath, '/', os.sep)
        return relPath

    #--------------------------------------------------------------------------
    # Figures out the relative path from one path in the Nebula directory
    # hierarchy to another. Input paths must be relative to root Nebula
    # directory.
    def FindRelPath(self, fromPath, toPath):
        #print 'From Path: ' + fromPath
        #print 'To Path: ' + toPath
        fromPathComponents = string.split(fromPath, os.sep)
        toPathComponents = string.split(toPath, os.sep)
        #print 'To Path Components: ' + str(toPathComponents)
        if fromPathComponents and toPathComponents:
            maxIdx = min(len(fromPathComponents), len(toPathComponents))
            curIdx = 0
            relPath = ''
            # skip past the common path components
            while curIdx < maxIdx:
                if fromPathComponents[curIdx] == toPathComponents[curIdx]:
                    curIdx += 1
                else:
                    break
            #print 'Relative Path A: ' + relPath
            # convert the rest of the components in fromPath to ..
            i = curIdx
            while i < len(fromPathComponents):
                if fromPathComponents[i] != '':
                    relPath = os.path.join(relPath, os.pardir)
                    i += 1
                else:
                    break
            #print 'Relative Path B: ' + relPath
            # now slap the remaining components of toPath onto the back
            i = curIdx
            while i < len(toPathComponents):
                if toPathComponents[i] != '':
                    relPath = os.path.join(relPath, toPathComponents[i])
                    #print 'ToPathComponent: ' + toPathComponents[i]
                    i += 1
                else:
                    break
            #print 'Relative Path C: ' + relPath
            # all done
            return relPath
        else:
            raise BuildSysError("Can't figure out how the hell to get from " \
                                + fromPath + " to " + toPath)

    #--------------------------------------------------------------------------
    def GetAbsPathFromRel(self, relPath):
        return os.path.join(self.homeDir, relPath)

#--------------------------------------------------------------------------
# EOF
#--------------------------------------------------------------------------
