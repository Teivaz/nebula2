#--------------------------------------------------------------------------
# Generates doxygen files for script interfaces and uses doxygen to 
# generate HTML documentation for the Nebula 2 source base.
#
# (c) 2005 Vadim Macagon
# Based on autodoc.py written by Leaf Garland and Bruce Mitchener.
#
# Contents are licensed under the Nebula license.
#--------------------------------------------------------------------------

import sys, os, fnmatch, re, time, string, glob, subprocess, platform
from buildsys3.externaltask import ExternalTask

#--------------------------------------------------------------------------

STR_SCRIPT_INTERFACE_MAIN_PAGE_S1 = '''\
/**
@page NebulaTwoScriptInterface Nebula 2 Script Interface

@section NebulaTwoScriptInterfaceForUsers Notes for Users

Select the class you're interested in from the tree below. Each class documents
the commands that that class defines, plus a class also inherits commands from
its parent. For example @c nanimator also accepts @c nscenenode
commands, and every class inherits @c nobject commands.

You can use the command @c getcmds on any class to see a full
list of commands for that class.

The @b input / @b output parameter information for each command uses a single 
letter code to represent the type of the parameter.
   - f - float value
   - i - integer value
   - o - object reference
   - s - string value
   - l - list value
   - b - true|false
   - v - no parameters

@section NebulaTwoScriptInterfaceTree Scriptable Classes
'''

STR_SCRIPT_INTERFACE_MAIN_PAGE_S2 = '''\

@section NebulaTwoScriptInterfaceForDevelopers Notes for Developers

<p>
In order for your commands to be documented here you must put the functions
in a file called <code>*_cmds.cc</code>. To describe the class use a comment
block like this, usually put in front of the <code>n_initcmds</code> function:
</p>

@verbatim
/**
    @ scriptclass
    ntimeserver

    @ superclass
    nroot

    @ classinfo
    The ntimeserver object lives under the name /sys/servers/time
    and provides a central time source for Nebula.
*/
@endverbatim

Then put a comment block in front of each script function like this:

@verbatim
/**
    @ cmd
    gettime

    @ input
    v

    @ output
    f (Time)

    @ info
    Return current global time in seconds since the time server
    was created.
*/
@endverbatim

This will be output <i>as is</i> so make sure to wrap your text at a 
reasonable point, and try to use spaces instead of tabs. See any 
@c *_cmds.cc file for more detail.
*/
'''

STR_CLASS_INFO_SCRIPT_CLASS = '''\
@page N2ScriptInterface_%(className)s %(cppClassName)s Script Interface

@section N2ScriptInterfaceClass_%(className)s %(className)s:
'''

STR_CLASS_INFO_SUPER_CLASS_LINK = '''\

@subsection N2ScriptInterfaceSuperClass_%(className)s Super Class:
    - @ref N2ScriptInterface_%(superClassName)s "%(superClassName)s"
'''

STR_CLASS_INFO_SUBCLASSES_HEADER = '''\
    
@subsection N2ScriptInterfaceSubClasses_%(className)s Subclasses:
'''

STR_CLASS_INFO_SUBCLASS_LINK = '''\
    - @subpage N2ScriptInterface_%s "%s"
'''

STR_CLASS_INFO_CPP_CLASS_LINK = '''\

@subsection N2ScriptInterface_%(className)sCPPClass C++ Class:
    - @ref %(cppClassName)s
'''
    
STR_CLASS_INFO_CMDS_HEADER = '''\

@subsection N2ScriptInterface_%(className)sInfo Info

%(classInfo)s

@subsection N2ScriptInterfaceCommands_%(className)s Commands:
'''    

STR_CLASS_INFO_CMD_LINK = '''\
    - @ref N2ScriptInterface_%s_%s
'''
    
STR_CLASS_INFO_FOOTER = '''\


<hr>

'''

STR_CMD_BLOCK = '''\
@subsubsection N2ScriptInterface_%(className)s_%(cmdName)s %(cmdName)s

@par Command Input:
<tt>%(cmdIn)s</tt>

@par Command Output:
<tt>%(cmdOut)s</tt>

@par Command Description:
%(cmdDesc)s

<hr>
'''

#--------------------------------------------------------------------------
class aCmd:
    """simple structure for storing cmd details"""
    name = ''
    input = ''
    output = ''
    info = ''
    
    #--------------------------------------------------------------------------
    def __repr__(self):
        return self.name

#--------------------------------------------------------------------------
class aClass:
    """simple structure for storing class details"""
    name = ""
    info = ""
    
    #--------------------------------------------------------------------------
    def findSubclasses(self, classes):
        """Recursively find subclasses, and add them into subclasses list 
        (very brute force)."""
        self.subclasses = []
        for k in classes.keys():
            if classes[k].superclass == self.name:
                classes[k].findSubclasses(classes)
                self.subclasses.append(classes[k])
        self.subclasses.sort()
        
    #--------------------------------------------------------------------------
    def __repr__(self):
        return self.name
        
    #--------------------------------------------------------------------------
    def __cmp__(self, other):
        if self.name < other.name: return -1
        if self.name > other.name: return 1
        return 0

#--------------------------------------------------------------------------
def escapeHtml(text):
    return text.replace("&", "&amp;").replace(">", "&gt;").replace("<", "&lt;")

#--------------------------------------------------------------------------
class doxygen:
    
    #--------------------------------------------------------------------------
    def __init__(self, buildSys):
        self.buildSys = buildSys
        self.description = 'Nebula 2 Documentation Generator'
        self.classes = {} # classes to document
        self.moduleNames = [] # modules to document
        # regular expressions to extract class/cmd info
        self.rClass = re.compile(r"@scriptclass\s*(?P<classname>\S*)(?:(?:\s*)|(?:\s*@cppclass\s*(?P<cppname>\S*?))\s*)@superclass\s*(?P<superclass>\S*)\s*@classinfo\s*(?P<info>.*?)\s*\*/", re.S)
        self.rCmd = re.compile(r"@cmd\s*(?P<cmd>\S*)\s*@input\s*(?P<input>.*?)\s*@output\s*(?P<output>.*?)\s*@info\s*(?P<info>.*?)\s*\*/", re.S)
        # setup the paths
        self.autodocDir = os.path.join(buildSys.homeDir, 'doc', 'autodoc')
        self.classPagesDir = os.path.join(self.autodocDir, 'classes')
        self.doxycfgDir = os.path.join(self.buildSys.homeDir, 'code', 'doxycfg')
        self.doxygenPath = ''
        self.htmlHelpCompilerPath = ''
        self.srcDirs = []
        self.defaultSettings = { 'doxygenDir'          : '',
                                 'autoRunDoxygen'      : True,
                                 'htmlHelpWorkshopDir' : '',
                                 'autoGenerateCHM'     : False }
        
    #--------------------------------------------------------------------------
    def HasSettings(self):
        return True
        
    #--------------------------------------------------------------------------
    def GetSettings(self):
        settings = self.buildSys.GetBuildConfigSetting('doxygen')
        if None == settings:
            settings = self.defaultSettings
        return settings.copy()
        
    #--------------------------------------------------------------------------
    def SetSettings(self, settings):
        self.buildSys.SetBuildConfigSetting('doxygen', settings)
        
    #--------------------------------------------------------------------------
    def SaveSettings(self):
        self.buildSys.SaveBuildConfig()
        
    #--------------------------------------------------------------------------
    def GetSetting(self, settingName):
        settings = self.GetSettings()
        return settings.get(settingName, self.defaultSettings[settingName])
        
    #--------------------------------------------------------------------------
    def SetSetting(self, settingName, settingValue):
        settings = self.GetSettings()
        settings[settingName] = settingValue
        self.SetSettings(settings)
        
    #--------------------------------------------------------------------------
    def Generate(self, workspaceNames):
        self.classes = {}
        try:
            doRunDoxygen = self.GetSetting('autoRunDoxygen')
            doGenerateCHM = self.GetSetting('autoGenerateCHM')
            
            self.collectModulesToDocument(workspaceNames)
            self.collectDoxygenInputDirs()

            keepGoing = True
            progressVal = 0
            self.buildSys.CreateProgressDialog('Extracting Script Docs From Source', 
                                               ' ' * 130, 
                                               len(self.srcDirs))
            for srcDir in self.srcDirs:
                self.parseFiles(srcDir)
                progressVal += 1
                self.buildSys.UpdateProgressDialog(progressVal,
                                                   'Processing ' + srcDir)
                if self.buildSys.ProgressDialogCancelled():
                    keepGoing = False
                    break
            self.buildSys.DestroyProgressDialog()
            
            if keepGoing:
                self.writeDoxygenConfig(workspaceNames)
                self.getRootClass().findSubclasses(self.classes)
                self.writeDoxygenScriptInterfaceMainPage(self.getRootClass())
                self.prepareClassPagesDir()
                self.writeDoxygenScriptInterfacePages(self.getRootClass())
                
                if doRunDoxygen:
                    if self.resolveDoxygenPath():
                        keepGoing = self.runDoxygen()
                    else:
                        self.buildSys.logger.error("Doxygen not found, can't " \
                                                   "generate documentation.")
                        keepGoing = False
                
            if keepGoing:
                if doGenerateCHM:
                    if self.resolveHtmlHelpCompilerPath():
                        self.buildSys.logger.info('HTML Help Workshop detected.')
                        self.createCHM()
                    else:
                        self.buildSys.logger.error("HTML Help Compiler not found," \
                                                   " can't generate CHM.")
        except:
            self.buildSys.logger.exception('Exception in doxygen.Generate()')
        
        #summaryDetails = { 'numOfWorkspacesBuilt' : progressVal,
        #                   'totalNumOfWorkspaces' : len(workspaceNames) }
        #self.buildSys.DisplaySummaryDialog(summaryDetails)
        self.buildSys.logger.info('Done building documentation!')

    #--------------------------------------------------------------------------
    # Private Stuff
    #--------------------------------------------------------------------------

    #--------------------------------------------------------------------------
    # Create or clean up the directory where script interface documentation
    # will be generated
    def prepareClassPagesDir(self):
        if os.path.exists(self.classPagesDir):
            # remove any existing .dox files
            fileList = os.listdir(self.classPagesDir)
            for fileName in fileList:
                if fnmatch.fnmatch(fileName, '*.dox'):
                    filePath = os.path.join(self.classPagesDir, fileName)
                    if os.path.isfile(filePath):
                        os.remove(filePath)
        else:
            os.makedirs(self.classPagesDir)

    #--------------------------------------------------------------------------
    # Check if doxygen is available, if it is the return True
    # otherwise return False.
    def resolveDoxygenPath(self):
        self.doxygenPath = ''
        doxygenPath = os.path.join(self.GetSetting('doxygenDir'), 'doxygen')
        cmdPipe = os.popen('"%s" --help' % doxygenPath, 'r')
        helpStr = cmdPipe.readline()
        cmdPipe.close()
        if '' != helpStr:
            self.doxygenPath = doxygenPath
            return True
        else:
            # user supplied path is invalid, see if the OS can find doxygen
            cmdPipe = os.popen('doxygen --help', 'r')
            helpStr = cmdPipe.readline()
            cmdPipe.close()
            if '' != helpStr:
                self.doxygenPath = 'doxygen'
                return True
        return False

    #--------------------------------------------------------------------------
    # Runs Doxygen and return True when it finishes running, or False if it
    # doesn't finish running for some reason.
    def runDoxygen(self):
        task = ExternalTask('Doxygen', 
                            '"%s" auto_nebula2.cfg' % self.doxygenPath, 
                            self.doxycfgDir, self.buildSys.GetMainFrame())
        if task.Run():
            if platform.system() == 'Windows':
                # if we're not going to generate a CHM then display the 
                # index page for the documentation doxygen just generated
                if not self.GetSetting('autoGenerateCHM'):
                    docPath = self.buildSys.GetAbsPathFromRel(
                              os.path.join('doc', 'doxydoc', 'nebula2', 'html', 
                                           'index.html'))
                    os.startfile(docPath)
            return True
        
        return False
        
    #-------------------------------------------------------------------------
    # collect all the modules from the selected workspaces
    def collectModulesToDocument(self, workspaceNames):
        self.moduleNames = []
        for workspaceName in workspaceNames:
            workspace = self.buildSys.workspaces[workspaceName]
            for targetName in workspace.targets:
                target = self.buildSys.targets[targetName]
                for moduleName in target.modules:
                    if ('pkg_' != moduleName[:4]) and ('dummy' != moduleName):
                        if moduleName not in self.moduleNames:
                            self.moduleNames.append(moduleName)
        
    #--------------------------------------------------------------------------
    # Find all directories that need to be fed as input to doxygen
    def collectDoxygenInputDirs(self):
        # we want directory paths to be relative to the directory with the
        # doxygen config files, so change to it now to make things easier
        oldPath = os.getcwd()
        os.chdir(self.doxycfgDir)
        
        self.doxygenInputDirs = []
        
        for moduleName in self.moduleNames:
            module = self.buildSys.modules[moduleName]
            # directories with *.h files
            inDir = os.path.join(os.pardir, os.pardir, module.GetBaseIncDir(), 
                                 module.dir)
            if inDir not in self.doxygenInputDirs:
                if os.path.isdir(inDir):
                    self.doxygenInputDirs.append(inDir)
            # directories with *.cc files
            inDir = os.path.join(os.pardir, os.pardir, module.GetBaseSrcDir(),
                                 module.dir)
            if os.path.isdir(inDir):
                if inDir not in self.srcDirs:
                    self.srcDirs.append(os.path.join(self.buildSys.homeDir,
                                                     module.GetBaseSrcDir(),
                                                     module.dir))
                if inDir not in self.doxygenInputDirs:
                    self.doxygenInputDirs.append(inDir)
                    
            # directories with *.dox files
            inDir = os.path.join(os.pardir, module.codeDir, 'doc')
            if inDir not in self.doxygenInputDirs:
                if os.path.isdir(inDir):
                    self.doxygenInputDirs.append(inDir)
        
        #print 'src dirs ' + str(self.srcDirs)
        
        os.chdir(oldPath)
        
    #--------------------------------------------------------------------------
    def writeDoxygenConfig(self, workspaceNames):
        # we want directory paths to be relative to the directory with the
        # doxygen config files, so change to it now to make things easier
        oldPath = os.getcwd()
        os.chdir(self.doxycfgDir)

        # read in the base config at code/doxycfg/base_nebula2.cfg
        autoCfg = ''
        try:
            baseCfgFile = file('base_nebula2.cfg', 'r')
        except IOError:
            self.buildSys.logger.error("Couldn't open base_nebula2.cfg for reading.")
        else:
            autoCfg = baseCfgFile.read()
            baseCfgFile.close()

        # append the input dirs to the config
        autoCfg += 'INPUT            = '
        indent =   '                   '
        # the special directories
        autoCfg += os.path.join(os.pardir, os.pardir, 
                                'doc', 'nebula2') + ' \\\n'
        autoCfg += indent + os.path.join(os.pardir, os.pardir, 
                                         'doc', 'autodoc') + ' \\\n'
        autoCfg += indent + os.path.join(os.pardir, os.pardir, 'data', 
                                         'shaders', 'lib') + ' \\\n'
        # ok so this isn't a directory
        autoCfg += indent + os.path.join(os.pardir, 'contrib', 
                                         'contrib.dox') + ' \\\n'
        # the rest
        numDirs = len(self.doxygenInputDirs)
        for i in range(numDirs):
            if i == (numDirs - 1):
                autoCfg += indent + self.doxygenInputDirs[i] + ' \n'
            else:
                autoCfg += indent + self.doxygenInputDirs[i] + ' \\\n'
        
        # write the config out to code/nebula2/doxycfg/auto_nebula2.cfg
        try:
            autoCfgFile = file('auto_nebula2.cfg', 'w')
        except IOError:
            self.buildSys.logger.error("Couldn't open auto_nebula2.cfg for writing.")
        else:
            autoCfgFile.write(autoCfg)
            autoCfgFile.close()

        os.chdir(oldPath)
                
    #--------------------------------------------------------------------------
    # Check if the Microsoft HTML Help Compiler (hhc) is available, if it is 
    # return True, otherwise return False.
    def resolveHtmlHelpCompilerPath(self):
        self.htmlHelpCompilerPath = ''
        hhcPath = os.path.join(self.GetSetting('htmlHelpWorkshopDir'), 'hhc')
        cmdPipe = os.popen('"%s"' % hhcPath, 'r')
        helpStr = cmdPipe.readline()
        cmdPipe.close()
        if '' != helpStr:
            self.htmlHelpCompilerPath = hhcPath
            return True
        else:
            # user supplied path is invalid, see if the OS can find hhc
            cmdPipe = os.popen('hhc', 'r')
            helpStr = cmdPipe.readline()
            cmdPipe.close()
            if '' != helpStr:
                self.htmlHelpCompilerPath = 'hhc'
                return True
        return False
        
    #--------------------------------------------------------------------------
    def createCHM(self):
        hhpPath = os.path.join(self.buildSys.homeDir, 'doc', 'doxydoc', 
                               'nebula2', 'html')
        task = ExternalTask('HTML Help Compiler', 
                            '"%s" index.hhp' % self.htmlHelpCompilerPath, 
                            hhpPath, self.buildSys.GetMainFrame())
        if task.Run():
            if platform.system() == 'Windows':
                # display the CHM the HTML Help Compiler just generated
                docPath = self.buildSys.GetAbsPathFromRel(
                          os.path.join('doc', 'doxydoc', 'nebula2', 'html', 
                                       'nebula2.chm'))
                os.startfile(docPath)
            return True
            
        return False

    #--------------------------------------------------------------------------
    def getRootClass(self):
        return self.classes['nobject']

    #--------------------------------------------------------------------------
    def parseFiles(self, pathname):
        """Recursively scan a directory tree, looking for *_cmds.cc files and 
        extract class/cmd information from them."""
        dirlist = os.listdir(pathname)
        for filename in dirlist:
            filepath = os.path.join(pathname, filename)
            if os.path.isdir(filepath):
                self.parseFiles(filepath)
            if fnmatch.fnmatch(filename, '*_cmds.cc'):
                cmdsFile = open(filepath, "r")
                contents = cmdsFile.read()
                mClass = self.rClass.search(contents)
                if mClass:
                    clazz = aClass()
                    clazz.name = mClass.group('classname')
                    clazz.superclass = mClass.group('superclass')
                    clazz.info = escapeHtml(mClass.group('info'))
                    clazz.cppname = mClass.group('cppname') or clazz.name
                    cmds = []
                    mCmd = self.rCmd.search(contents)
                    while mCmd:
                        cmd = aCmd()
                        cmd.name = mCmd.group('cmd')
                        cmd.input = mCmd.group('input')
                        cmd.output = mCmd.group('output')
                        cmd.info = escapeHtml(mCmd.group('info'))
                        cmds.append(cmd)
                        mCmd = self.rCmd.search(contents, mCmd.end())
                    clazz.cmds = cmds
                    self.classes[clazz.name] = clazz
                else:
                    self.buildSys.logger.warning('No class information found in '
                                                 + filepath)
                cmdsFile.close()
    
    #--------------------------------------------------------------------------
    # Generate a class tree in doxygen format.
    def generateDoxygenClassTree(self, clazz, indent = 0):
        if indent == 0:
            out = '%s - @subpage N2ScriptInterface_%s "%s"\n' % (' ' * indent, 
                                                                 clazz.name, clazz.name)
        else:
            out = '%s - @ref N2ScriptInterface_%s "%s"\n' % (' ' * indent, 
                                                             clazz.name, clazz.name)
        if clazz.subclasses:
            for sc in clazz.subclasses:
                out += self.generateDoxygenClassTree(sc, indent + 4)
        return out
        
    #--------------------------------------------------------------------------
    # Write out the main page for the script interface documentation.
    def writeDoxygenScriptInterfaceMainPage(self, rootClass):
        doxPath = os.path.join(self.autodocDir, 'tree.dox')
        try:
            doxFile = file(doxPath, 'w')
        except IOError:
            self.buildSys.logger.error("Couldn't open %s for writing.", doxPath)
        else:
            doxFile.write(STR_SCRIPT_INTERFACE_MAIN_PAGE_S1)
            doxFile.write(self.generateDoxygenClassTree(rootClass))
            doxFile.write(STR_SCRIPT_INTERFACE_MAIN_PAGE_S2)
            doxFile.close()
        
    #--------------------------------------------------------------------------
    def writeDoxygenScriptInterfacePageForClass(self, clazz):
        doxPath = os.path.join(self.classPagesDir, '%s.dox' % clazz)
        try:
            doxFile = file(doxPath, 'w')
        except IOError:
            self.buildSys.logger.error("Couldn't open %s for writing.", doxPath)
        else:
            args = { 'className'      : clazz.name,
                     'superClassName' : clazz.superclass,
                     'cppClassName'   : clazz.cppname,
                     'classInfo'      : clazz.info.replace("\n    ", "\n") }
            out = '/**\n'
            out += STR_CLASS_INFO_SCRIPT_CLASS % args
            if clazz.superclass != '---':
                out += STR_CLASS_INFO_SUPER_CLASS_LINK % args
            if clazz.subclasses:
                out += STR_CLASS_INFO_SUBCLASSES_HEADER % args
                for subclass in clazz.subclasses:
                    out += STR_CLASS_INFO_SUBCLASS_LINK % (subclass.name,
                                                           subclass.name)
            out += STR_CLASS_INFO_CPP_CLASS_LINK % args
            out += STR_CLASS_INFO_CMDS_HEADER % args
            # do command block
            numCmds = len(clazz.cmds)
            for i in range(numCmds):
                cmd = clazz.cmds[i]
                out += STR_CLASS_INFO_CMD_LINK % (clazz.name, cmd.name)
            out += STR_CLASS_INFO_FOOTER
            for cmd in clazz.cmds:
                out += self.generateDoxygenCmdBlock(cmd, clazz)
            out += '*/\n'
            doxFile.write(out)
            doxFile.close()

    #--------------------------------------------------------------------------
    def generateDoxygenCmdBlock(self, cmd, clazz):
        args = { 'className' : clazz.name,
                 'cmdName'   : cmd.name,
                 'cmdIn'     : cmd.input.replace("\n    ", "\n"),
                 'cmdOut'    : cmd.output.replace("\n    ", "\n"),
                 'cmdDesc'   : cmd.info.replace("\n    ", "\n") }
        return STR_CMD_BLOCK % args

    #--------------------------------------------------------------------------
    # Recursively write doxygen pages for the script interfaces of the given
    # class and all it's subclasses.
    def writeDoxygenScriptInterfacePages(self, superClass):
        self.writeDoxygenScriptInterfacePageForClass(superClass)
        for clazz in superClass.subclasses:
            self.writeDoxygenScriptInterfacePages(clazz)


#--------------------------------------------------------------------------
# EOF
#--------------------------------------------------------------------------
