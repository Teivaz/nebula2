#--------------------------------------------------------------------------
# (c) 2007 Vadim Macagon
#
# Contents are licensed under the Nebula license.
#--------------------------------------------------------------------------

from buildsys3.workspace import Workspace
from buildsys3.target import Target
from buildsys3.bundle import Bundle
from buildsys3.module import Module
from buildsys3.Plex import *
import os, string

class BldScannerError(Exception):
    def __init__(self, value):
        self.value = value
    def __str__(self):
        return `self.value`

#
# This is a pretty basic scanner that parses a bld file
#
class BldScanner(Scanner):
    # stores all workspace related info
    workspaces = []
    # stores all module related info
    modules = []
    # stores all target related info
    targets = []
    # stores all bundle related info
    bundles = []
    # something to help detect faulty brace placement
    blockBraceCount = 0

    # maps tokens to module states
    moduleStates = {'annotate'              : 'm_annotate',
                    'setdir'                : 'm_setdir',
                    'setplatform'           : 'm_setplatform',
                    'settype'               : 'm_settype',
                    'setfiles'              : 'm_setfiles',
                    'setheaders'            : 'm_setheaders',
                    'setlibs_win32'         : 'm_setlibs_win32',
                    'setlibs_win32_release' : 'm_setlibs_win32_release',
                    'setlibs_win32_debug'   : 'm_setlibs_win32_debug',
                    'setlibs_linux'         : 'm_setlibs_linux',
                    'setlibs_macosx'        : 'm_setlibs_macosx',
                    'setframeworks_macosx'  : 'm_setframeworks_macosx',
                    'setmoduledeps'         : 'm_setmoduledeps',
                    'setnopkg'              : 'm_setnopkg',
                    'setmoddeffile'         : 'm_setmoddeffile',
                    'set_extra_includes'    : 'm_set_extra_includes'}
    # maps tokens to target states
    targetStates = {'annotate'           : 't_annotate',
                    'settype'            : 't_settype',
                    'setrtti'            : 't_setrtti',
                    'setexceptions'      : 't_setexceptions',
                    'seticon'            : 't_seticon',
                    'setresource_win32'  : 't_setresource_win32',
                    'setmodules'         : 't_setmodules',
                    'setbundles'         : 't_setbundles',
                    'settargetdeps'      : 't_settargetdeps',
                    'changedllextension' : 't_changedllextension'}
    # maps tokens to bundle states
    bundleStates = {'annotate'   : 'b_annotate',
                    'setmodules' : 'b_setmodules',
                    'settargets' : 'b_settargets'}
    # maps tokens to workspace states
    workspaceStates = {'annotate'     : 'w_annotate',
                       'setdir'       : 'w_setdir',
                       'setbinarydir' : 'w_setbinarydir',
                       'setlibdir'    : 'w_setlibdir',
                       'settargets'   : 'w_settargets',
                       'addglobaldef' : 'w_addglobaldef',
                       'addtargetdef' : 'w_addtargetdef'}

    def StartBlock(self, text):
        self.blockBraceCount += 1
        #print '*** start block *** state: ' + self.state_name

    def EndBlock(self, text):
        self.blockBraceCount -= 1
        #print '*** end block *** state before: ' + self.state_name
        if 'm_' == self.state_name[:2]:
            self.begin('module')
            #print 'switching state to module'
        elif 't_' == self.state_name[:2]:
            self.begin('target')
            #print 'switching state to target'
        elif 'b_' == self.state_name[:2]:
            self.begin('bundle')
            #print 'switching state to bundle'
        elif 'w_' == self.state_name[:2]:
            self.begin('workspace')
            #print 'switching state to workspace'

    def InBlock(self):
        return (self.blockBraceCount > 0)

    def CheckBraceCount(self):
        if self.blockBraceCount != 0:
            pos = self.position()
            err = 'Missing brace.\n'
            err += pos.name + ' line ' + pos.line
            raise PakScannerError(err)

    #
    # workspace actions
    #

    def CheckWorkspaceNameDefined(self):
        if not self.workspaceNameDefined:
            pos = self.position()
            err = 'Workspace name not defined.\n'
            err += pos.name + ' line ' + pos.line
            raise BldScannerError(err)

    def BeginWorkspace(self, text):
        self.workspaceNameDefined = False
        self.begin('workspace')

    def ProcessNameInWorkspace(self, text):
        if self.workspaceNameDefined:
            if 'endworkspace' == text:
                self.CheckBraceCount()
                self.begin('')
            elif text in self.workspaceStates:
                self.begin(self.workspaceStates[text])
        else:
            self.workspaceNameDefined = True
            self.workspaces.append(Workspace(text, self.name))

    def SetWorkspaceAnnotation(self, text):
        self.CheckWorkspaceNameDefined()
        self.workspaces[len(self.workspaces) - 1].annotation = text
        self.begin('workspace') # switch back to parent state

    def SetWorkspaceDir(self, text):
        self.CheckWorkspaceNameDefined()
        self.workspaces[len(self.workspaces) - 1].dir = text
        self.begin('workspace') # switch back to parent state

    def SetWorkspaceBinaryDir(self, text):
        self.CheckWorkspaceNameDefined()
        self.workspaces[len(self.workspaces) - 1].binaryDir = text
        self.begin('workspace') # switch back to parent state

    def SetWorkspaceInterDir(self, text):
        self.CheckWorkspaceNameDefined()
        self.logger.warning('setlibdir in workspace %s in %s is deprecated.',
                            self.workspaces[len(self.workspaces) - 1].name,
                            self.name)
        self.begin('workspace') # switch back to parent state

    def AddWorkspaceTarget(self, text):
        self.CheckWorkspaceNameDefined()
        self.workspaces[len(self.workspaces) - 1].targets.append(text)
        if not self.InBlock():
            self.begin('workspace') # switch back to parent state

    def AddWorkspaceGlobalDef(self, text):
        self.CheckWorkspaceNameDefined()
        #print '*** globaldef: ' + text
        tokens = string.split(text)
        if len(tokens) > 2:
            pos = self.position()
            err = 'Invalid addglobaldef syntax.\n'
            err += pos.name + ' line ' + pos.line
            raise BldScannerError(err)
        self.workspaces[len(self.workspaces) - 1].globalDefs.append(tokens)
        self.begin('workspace') # switch back to parent state

    def AddWorkspaceTargetDef(self, text):
        self.CheckWorkspaceNameDefined()
        tokens = string.split(text)
        if len(tokens) > 3:
            pos = self.position()
            err = 'Invalid addtargetdef syntax.\n'
            err += pos.name + ' line ' + pos.line
            raise BldScannerError(err)
        targetName = string.strip(tokens[0])
        defs = self.workspaces[len(self.workspaces) - 1].targetDefs
        if targetName not in defs:
            defs[targetName] = [tokens[1:]]
        else:
            defs[targetName].append(tokens[1:])
        self.begin('workspace') # switch back to parent state

    #
    # bundle actions
    #

    def CheckBundleNameDefined(self):
        if not self.bundleNameDefined:
            pos = self.position()
            err = 'Bundle name not defined.\n'
            err += pos.name + ' line ' + pos.line
            raise BldScannerError(err)

    def BeginBundle(self, text):
        self.bundleNameDefined = False
        self.begin('bundle')

    def ProcessNameInBundle(self, text):
        if self.bundleNameDefined:
            if 'endbundle' == text:
                self.CheckBraceCount()
                self.begin('')
            elif text in self.bundleStates:
                self.begin(self.bundleStates[text])
        else:
            self.bundleNameDefined = True
            self.bundles.append(Bundle(text, self.name))

    def SetBundleAnnotation(self, text):
        self.CheckBundleNameDefined()
        self.bundles[len(self.bundles) - 1].annotation = text
        self.begin('bundle') # switch back to parent state

    def AddBundleModule(self, text):
        self.CheckBundleNameDefined()
        self.bundles[len(self.bundles) - 1].modules.append(text)
        if not self.InBlock():
            self.begin('bundle') # switch back to parent state

    def AddBundleTarget(self, text):
        self.CheckBundleNameDefined()
        self.bundles[len(self.bundles) - 1].targets.append(text)
        if not self.InBlock():
            self.begin('bundle') # switch back to parent state

    #
    # module actions
    #

    def CheckModuleNameDefined(self):
        if not self.moduleNameDefined:
            pos = self.position()
            err = 'Module name not defined.\n'
            err += pos.name + ' line ' + pos.line
            raise BldScannerError(err)

    def BeginModule(self, text):
        self.moduleNameDefined = False
        self.begin('module')

    def ProcessNameInModule(self, text):
        if self.moduleNameDefined:
            if 'endmodule' == text:
                self.CheckBraceCount()
                self.begin('')
            elif text in self.moduleStates:
                self.begin(self.moduleStates[text])
        else:
            self.moduleNameDefined = True
            self.modules.append(Module(text, self.name))

    def SetModuleAnnotation(self, text):
        self.CheckModuleNameDefined()
        self.modules[len(self.modules) - 1].annotation = text
        self.begin('module') # switch back to parent state

    def SetModuleDir(self, text):
        self.CheckModuleNameDefined()
        self.modules[len(self.modules) - 1].dir = string.strip(text, '"')
        if not self.InBlock():
            self.begin('module') # switch back to parent state

    def SetModulePlatform(self, text):
        self.CheckModuleNameDefined()
        self.modules[len(self.modules) - 1].platform = text
        if not self.InBlock():
            self.begin('module') # switch back to parent state

    def SetModuleType(self, text):
        self.CheckModuleNameDefined()
        self.modules[len(self.modules) - 1].modType = text
        if not self.InBlock():
            self.begin('module') # switch back to parent state

    def AddModuleHeader(self, text):
        self.CheckModuleNameDefined()
        self.modules[len(self.modules) - 1].headers.append(text)
        if not self.InBlock():
            self.begin('module') # switch back to parent state

    def AddModuleFile(self, text):
        self.CheckModuleNameDefined()
        self.modules[len(self.modules) - 1].files.append(text)
        if not self.InBlock():
            self.begin('module') # switch back to parent state

    def AddModuleWin32Lib(self, text):
        self.CheckModuleNameDefined()
        self.modules[len(self.modules) - 1].libsWin32.append(text)
        if not self.InBlock():
            self.begin('module') # switch back to parent state

    def AddModuleWin32DebugLib(self, text):
        self.CheckModuleNameDefined()
        self.modules[len(self.modules) - 1].libsWin32Debug.append(text)
        if not self.InBlock():
            self.begin('module') # switch back to parent state

    def AddModuleWin32ReleaseLib(self, text):
        self.CheckModuleNameDefined()
        self.modules[len(self.modules) - 1].libsWin32Release.append(text)
        if not self.InBlock():
            self.begin('module') # switch back to parent state

    def AddModuleLinuxLib(self, text):
        self.CheckModuleNameDefined()
        self.modules[len(self.modules) - 1].libsLinux.append(text)
        if not self.InBlock():
            self.begin('module') # switch back to parent state

    def AddModuleMacOSXLib(self, text):
        self.CheckModuleNameDefined()
        self.modules[len(self.modules) - 1].libsMacOSX.append(text)
        if not self.InBlock():
            self.begin('module') # switch back to parent state

    def AddModuleMacOSXFramework(self, text):
        self.CheckModuleNameDefined()
        self.modules[len(self.modules) - 1].frameworksMacOSX.append(text)
        if not self.InBlock():
            self.begin('module') # switch back to parent state

    def AddModuleDependency(self, text):
        self.CheckModuleNameDefined()
        self.modules[len(self.modules) - 1].moduleDeps.append(text)
        if not self.InBlock():
            self.begin('module') # switch back to parent state

    def SetModuleNoPkg(self, text):
        self.CheckModuleNameDefined()
        val = string.lower(text)
        if 'true' == val:
            self.modules[len(self.modules) - 1].putInPkg = False
        elif 'false' == val:
            self.modules[len(self.modules) - 1].putInPkg = True
        self.begin('module') # switch back to parent state

    def SetModuleDefFile(self, text):
        self.CheckModuleNameDefined()
        self.modules[len(self.modules) - 1].modDefFile = text
        self.begin('module') # switch back to parent state
        
    def AddExtraIncludeDir(self, text):
        self.CheckModuleNameDefined()
        self.modules[len(self.modules) - 1].extraIncDirs.append(text)
        if not self.InBlock():
            self.begin('module') # switch back to parent state

    #
    # target actions
    #

    def CheckTargetNameDefined(self):
        if not self.targetNameDefined:
            pos = self.position()
            err = 'Target name not defined.\n'
            err += pos.name + ' line ' + pos.line
            raise BldScannerError(err)

    def BeginTarget(self, text):
        self.targetNameDefined = False
        self.begin('target')

    def ProcessNameInTarget(self, text):
        if self.targetNameDefined:
            if 'endtarget' == text:
                self.CheckBraceCount()
                self.begin('')
            elif text in self.targetStates:
                self.begin(self.targetStates[text])
        else:
            self.targetNameDefined = True
            self.targets.append(Target(text, self.name))

    def SetTargetAnnotation(self, text):
        self.CheckTargetNameDefined()
        self.targets[len(self.targets) - 1].annotation = text
        self.begin('target') # switch back to parent state

    def SetTargetType(self, text):
        self.CheckTargetNameDefined()
        self.targets[len(self.targets) - 1].type = text
        if not self.InBlock():
            self.begin('target') # switch back to parent state

    def SetTargetRTTI(self, text):
        self.CheckTargetNameDefined()
        val = string.lower(text)
        if 'true' == val:
            self.targets[len(self.targets) - 1].rtti = True
        elif 'false' == val:
            self.targets[len(self.targets) - 1].rtti = False
        self.begin('target') # switch back to parent state

    def SetTargetExceptions(self, text):
        self.CheckTargetNameDefined()
        val = string.lower(text)
        if 'true' == val:
            self.targets[len(self.targets) - 1].exceptions = True
        elif 'false' == val:
            self.targets[len(self.targets) - 1].exceptions = False
        self.begin('target') # switch back to parent state

    def SetTargetIcon(self, text):
        self.CheckTargetNameDefined()
        self.targets[len(self.targets) - 1].icon = string.strip(text, '"')
        self.begin('target')  # switch back to parent state

    def SetTargetWin32Resource(self, text):
        self.CheckTargetNameDefined()
        self.targets[len(self.targets) - 1].win32Resource = text
        self.begin('target') # switch back to parent state

    def AddTargetModule(self, text):
        self.CheckTargetNameDefined()
        self.targets[len(self.targets) - 1].modules.append(text)
        if not self.InBlock():
            self.begin('target') # switch back to parent state

    def AddTargetBundle(self, text):
        self.CheckTargetNameDefined()
        self.targets[len(self.targets) - 1].bundles.append(text)
        if not self.InBlock():
            self.begin('target') # switch back to parent state

    def AddTargetDependency(self, text):
        self.CheckTargetNameDefined()
        self.targets[len(self.targets) - 1].depends.append(text)
        if not self.InBlock():
            self.begin('target') # switch back to parent state

    def SetTargetExtension(self, text):
        self.CheckTargetNameDefined()
        self.targets[len(self.targets) - 1].extension = text
        self.begin('target') # switch back to parent state


    # lexer stuff
    letter = Range('AZaz')
    digit = Range('09')
    alphanum = letter | digit
    name = letter + Rep(alphanum | Str('_') | Str('.') | Str('::'))
    filename = Rep(alphanum | Any('_.-'))
    spaceortab = Any(' \t')
    globaldef = Alt(Rep1(alphanum | Str('_')),
                    Rep1(alphanum | Str('_')) + spaceortab + Rep1(AnyBut(' \t\n')))
    targetdef = name + spaceortab + globaldef
    space = Any(' \t\n')
    comment = Str('#') + Rep(AnyBut('\n'))
    annotation = Alt(Rep1(AnyBut('" \t\n')),
                     Str('"') + Rep(AnyBut('"')) + Str('"'))
    pathname = Alt(Rep1(alphanum | Any(':_-./\\')),
                   Str('"') + Rep1(alphanum | Any(' : _-./\\')) + Str('"'))
    boolean = NoCase(Str('true', 'false'))

    lexicon = Lexicon([ (Str('beginworkspace'), BeginWorkspace),
                        State('workspace', [
                            (name, ProcessNameInWorkspace),
                            (AnyChar, IGNORE)
                        ]),
                        State('w_annotate', [
                            (annotation, SetWorkspaceAnnotation),
                            (AnyChar, IGNORE)
                        ]),
                        State('w_setdir', [
                            (pathname, SetWorkspaceDir),
                            (AnyChar, IGNORE)
                        ]),
                        State('w_setbinarydir', [
                            (pathname, SetWorkspaceBinaryDir),
                            (AnyChar, IGNORE)
                        ]),
                        State('w_setlibdir', [
                            (pathname, SetWorkspaceInterDir),
                            (AnyChar, IGNORE)
                        ]),
                        State('w_settargets', [
                            (Str('{'), StartBlock),
                            (Str('}'), EndBlock),
                            (name, AddWorkspaceTarget),
                            (AnyChar, IGNORE)
                        ]),
                        State('w_addglobaldef', [
                            (globaldef, AddWorkspaceGlobalDef),
                            (AnyChar, IGNORE)
                        ]),
                        State('w_addtargetdef', [
                            (targetdef, AddWorkspaceTargetDef),
                            (AnyChar, IGNORE)
                        ]),
                        (Str('beginbundle'), BeginBundle),
                        State('bundle', [
                            (name, ProcessNameInBundle),
                            (AnyChar, IGNORE)
                        ]),
                        State('b_annotate', [
                            (annotation, SetBundleAnnotation),
                            (AnyChar, IGNORE)
                        ]),
                        State('b_setmodules', [
                            (Str('{'), StartBlock),
                            (Str('}'), EndBlock),
                            (name, AddBundleModule),
                            (AnyChar, IGNORE)
                        ]),
                        State('b_settargets', [
                            (Str('{'), StartBlock),
                            (Str('}'), EndBlock),
                            (name, AddBundleTarget),
                            (AnyChar, IGNORE)
                        ]),
                        (Str('beginmodule'), BeginModule),
                        State('module', [
                            (name, ProcessNameInModule),
                            (AnyChar, IGNORE)
                        ]),
                        State('m_annotate', [
                            (annotation, SetModuleAnnotation),
                            (AnyChar, IGNORE)
                        ]),
                        State('m_setdir', [
                            (pathname, SetModuleDir),
                            (AnyChar, IGNORE)
                        ]),
                        State('m_setplatform', [
                            (name, SetModulePlatform),
                            (AnyChar, IGNORE)
                        ]),
                        State('m_settype', [
                            (name, SetModuleType),
                            (AnyChar, IGNORE)
                        ]),
                        State('m_setfiles', [
                            (Str('{'), StartBlock),
                            (Str('}'), EndBlock),
                            (filename, AddModuleFile),
                            (AnyChar, IGNORE)
                        ]),
                        State('m_setheaders', [
                            (Str('{'), StartBlock),
                            (Str('}'), EndBlock),
                            (filename, AddModuleHeader),
                            (AnyChar, IGNORE)
                        ]),
                        State('m_setlibs_win32', [
                            (Str('{'), StartBlock),
                            (Str('}'), EndBlock),
                            (filename, AddModuleWin32Lib),
                            (AnyChar, IGNORE)
                        ]),
                        State('m_setlibs_win32_release', [
                            (Str('{'), StartBlock),
                            (Str('}'), EndBlock),
                            (filename, AddModuleWin32ReleaseLib),
                            (AnyChar, IGNORE)
                        ]),
                        State('m_setlibs_win32_debug', [
                            (Str('{'), StartBlock),
                            (Str('}'), EndBlock),
                            (filename, AddModuleWin32DebugLib),
                            (AnyChar, IGNORE)
                        ]),
                        State('m_setlibs_linux', [
                            (Str('{'), StartBlock),
                            (Str('}'), EndBlock),
                            (filename, AddModuleLinuxLib),
                            (AnyChar, IGNORE)
                        ]),
                        State('m_setlibs_macosx', [
                            (Str('{'), StartBlock),
                            (Str('}'), EndBlock),
                            (filename, AddModuleMacOSXLib),
                            (AnyChar, IGNORE)
                        ]),
                        State('m_setframeworks_macosx', [
                            (Str('{'), StartBlock),
                            (Str('}'), EndBlock),
                            (filename, AddModuleMacOSXFramework),
                            (AnyChar, IGNORE)
                        ]),
                        State('m_setmoduledeps', [
                            (Str('{'), StartBlock),
                            (Str('}'), EndBlock),
                            (name, AddModuleDependency),
                            (AnyChar, IGNORE)
                        ]),
                        State('m_setnopkg', [
                            (boolean, SetModuleNoPkg),
                            (AnyChar, IGNORE)
                        ]),
                        State('m_setmoddeffile', [
                            (pathname, SetModuleDefFile),
                            (AnyChar, IGNORE)
                        ]),
                        State('m_set_extra_includes', [
                            (Str('{'), StartBlock),
                            (Str('}'), EndBlock),
                            (pathname, AddExtraIncludeDir),
                            (AnyChar, IGNORE)
                        ]),
                        (Str('begintarget'), BeginTarget),
                        State('target', [
                            (name, ProcessNameInTarget),
                            (AnyChar, IGNORE)
                        ]),
                        State('t_annotate', [
                            (annotation, SetTargetAnnotation),
                            (AnyChar, IGNORE)
                        ]),
                        State('t_settype', [
                            (name, SetTargetType),
                            (AnyChar, IGNORE)
                        ]),
                        State('t_setrtti', [
                            (boolean, SetTargetRTTI),
                            (AnyChar, IGNORE)
                        ]),
                        State('t_setexceptions', [
                            (boolean, SetTargetExceptions),
                            (AnyChar, IGNORE)
                        ]),
                        State('t_seticon', [
                            (pathname, SetTargetIcon),
                            (AnyChar, IGNORE)
                        ]),
                        State('t_setresource_win32', [
                            (pathname, SetTargetWin32Resource),
                            (AnyChar, IGNORE)
                        ]),
                        State('t_setmodules', [
                            (Str('{'), StartBlock),
                            (Str('}'), EndBlock),
                            (name, AddTargetModule),
                            (AnyChar, IGNORE)
                        ]),
                        State('t_setbundles', [
                            (Str('{'), StartBlock),
                            (Str('}'), EndBlock),
                            (name, AddTargetBundle),
                            (AnyChar, IGNORE)
                        ]),
                        State('t_settargetdeps', [
                            (Str('{'), StartBlock),
                            (Str('}'), EndBlock),
                            (name, AddTargetDependency),
                            (AnyChar, IGNORE)
                        ]),
                        State('t_changedllextension', [
                            (name, SetTargetExtension),
                            (AnyChar, IGNORE)
                        ]),
                        (space, IGNORE),
                        (comment, IGNORE),
                        (AnyChar, IGNORE)])

    def __init__(self, stream, filename, logger):
        self.logger = logger
        self.workspaces = []
        self.modules = []
        self.targets = []
        self.bundles = []
        self.blockBraceCount = 0
        self.moduleNameDefined = False
        self.targetNameDefined = False
        self.bundleNameDefined = False
        self.workspaceNameDefined = False
        Scanner.__init__(self, self.lexicon, stream, filename)

#--------------------------------------------------------------------------
# EOF
#--------------------------------------------------------------------------
