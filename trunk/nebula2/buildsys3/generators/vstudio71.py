#--------------------------------------------------------------------------
# Visual Studio 2003 (7.1) Project & Solution Generator
#
# (c) 2005 Vadim Macagon
#
# Contents are licensed under the Nebula license.
#--------------------------------------------------------------------------

import os, string
import buildsys3.guid

#--------------------------------------------------------------------------

STR_PROJECT_HEADER = '''\
<?xml version="1.0" encoding="Windows-1252"?>
<VisualStudioProject
    ProjectType="Visual C++"
    Version="7.10"
    Name="%(targetName)s"
    Keyword="Win32Proj"
    ProjectGUID="%(targetUUID)s">
    <Platforms>
        <Platform Name="Win32"/>
    </Platforms>
'''

STR_PROJECT_FOOTER = '''\
    <Globals>
    </Globals>
</VisualStudioProject>
'''

STR_SRC_FILE = '''\
            <File
                RelativePath="%(relPath)s">
                <FileConfiguration Name="Debug|Win32">
                    <Tool
                        Name="VCCLCompilerTool"
                        PreprocessorDefinitions="%(nebSyms)s"
                        ObjectFile="$(IntDir)/%(objectName)s.obj"
                        CompileAs="%(compileAsFlag)i"/>
                </FileConfiguration>
                <FileConfiguration Name="Release|Win32">
                    <Tool
                        Name="VCCLCompilerTool"
                        PreprocessorDefinitions="%(nebSyms)s"
                        ObjectFile="$(IntDir)/%(objectName)s.obj"
                        CompileAs="%(compileAsFlag)i"/>
                </FileConfiguration>
            </File>
'''

STR_SLN_PROJECT_CONFIGS = '''\
\t\t{%(uuid)s}.Debug.ActiveCfg = Debug|Win32
\t\t{%(uuid)s}.Debug.Build.0 = Debug|Win32
\t\t{%(uuid)s}.Release.ActiveCfg = Release|Win32
\t\t{%(uuid)s}.Release.Build.0 = Release|Win32
'''

STR_SLN_GLOBAL_HEADER = '''\
Global
\tGlobalSection(SolutionConfiguration) = preSolution
\t\tDebug = Debug
\t\tRelease = Release
\tEndGlobalSection
\tGlobalSection(ProjectConfiguration) = postSolution
'''

STR_SLN_GLOBAL_FOOTER = '''\
\tEndGlobalSection
\tGlobalSection(ExtensibilityGlobals) = postSolution
\tEndGlobalSection
\tGlobalSection(ExtensibilityAddIns) = postSolution
\tEndGlobalSection
EndGlobal
'''

STR_PROJ_CONFIG_S1 = '''\
        <Configuration
            Name="%(configName)s"
            OutputDirectory="%(binaryDir)s"
            IntermediateDirectory="%(interDir)s"
            ConfigurationType="%(configType)i"
            CharacterSet="2">
            <Tool
                Name="VCCLCompilerTool"
                AdditionalIncludeDirectories="%(incDirs)s"
                PreprocessorDefinitions="__WIN32__;_WIN32_WINNT=WINVER;%(defs)s"
'''

STR_PROJ_CONFIG_S2_DEBUG = '''\
                Optimization="0"
                StringPooling="TRUE"
                ExceptionHandling="%(exceptions)s"
                RuntimeTypeInfo="%(rtti)s"
                MinimalRebuild="TRUE"
                BasicRuntimeChecks="3"
                RuntimeLibrary="1"
                UsePrecompiledHeader="0"
                WarningLevel="3"
                DebugInformationFormat="4"/>
'''

STR_PROJ_CONFIG_S2_RELEASE = '''\
                StringPooling="TRUE"
                GlobalOptimizations="TRUE"
                ExceptionHandling="%(exceptions)s"
                RuntimeTypeInfo="%(rtti)s"
                BasicRuntimeChecks="0"
                RuntimeLibrary="0"
                UsePrecompiledHeader="0"
                WarningLevel="3"
                Detect64BitPortabilityProblems="FALSE"
                BufferSecurityCheck="FALSE"
                DebugInformationFormat="0"/>
'''

STR_PROJ_CONFIG_LIBRARIAN_TOOL = '''\
            <Tool
                Name="VCLibrarianTool"
                OutputFile="%s"/>
'''

STR_PROJ_CONFIG_LINKER_TOOL_S1 = '''\
            <Tool
                Name="VCLinkerTool"
                AdditionalDependencies="%(win32Libs)s"
                OutputFile="%(outputFile)s"
'''

STR_PROJ_CONFIG_LINKER_TOOL_S2_DEBUG = '''\
                GenerateDebugInformation="TRUE"
                LinkIncremental="2"
                ProgramDatabaseFile="%s"
'''

STR_PROJ_CONFIG_LINKER_TOOL_S2_RELEASE = '''\
                LinkIncremental="1"
                OptimizeReferences="2"
                EnableCOMDATFolding="2"
'''

STR_PROJ_CONFIG_LINKER_TOOL_S3 = '''\
                AdditionalLibraryDirectories="%(libDirs)s"
                ModuleDefinitionFile="%(modDef)s"
                SubSystem="0"
                TargetMachine="1"/>
'''

STR_PROJ_CONFIG_OTHER_TOOLS = '''\
            <Tool Name="VCMIDLTool"/>
            <Tool Name="VCPostBuildEventTool"/>
            <Tool Name="VCPreBuildEventTool"/>
            <Tool Name="VCPreLinkEventTool"/>
            <Tool Name="VCWebServiceProxyGeneratorTool"/>
            <Tool Name="VCXMLDataGeneratorTool"/>
            <Tool Name="VCManagedWrapperGeneratorTool"/>
            <Tool Name="VCAuxiliaryManagedWrapperGeneratorTool"/>
'''

STR_PROJ_CONFIG_TOOL_RES_COMPILER_EXE = '''\
            <Tool
                Name="VCResourceCompilerTool"
                PreprocessorDefinitions="_DEBUG"
                Culture="1033"/>
'''

STR_PROJ_CONFIG_TOOL_RES_COMPILER = '''\
            <Tool Name="VCResourceCompilerTool"/>
'''

#--------------------------------------------------------------------------
class vstudio71:
    
    #--------------------------------------------------------------------------
    def __init__(self, buildSys):
        self.buildSys = buildSys
        self.description = 'Microsoft Visual Studio 2003 (7.1) ' \
                           'Project & Solution Generator'
        self.vcprojLocation = ''
        self.incDirStr = ''
        self.libDirStr = ''
        
    #--------------------------------------------------------------------------
    def HasSettings(self):
        return False
        
    #--------------------------------------------------------------------------
    def Generate(self, workspaceNames):
        defaultLocation = os.path.join('build', 'vstudio71')
        
        progressVal = 0
        solutionAbsDirs = []
        self.buildSys.CreateProgressDialog('Generating Solutions', '', 
                                           len(workspaceNames))
        try:
            for workspaceName in workspaceNames:
                workspace = self.buildSys.workspaces[workspaceName]
            
                # calculate these once for the workspace
                self.vcprojLocation = workspace.GetWorkspacePath(defaultLocation)
                self.incDirStr = workspace.GetIncSearchDirsString(defaultLocation)
                self.libDirStr = workspace.GetLibSearchDirsString('win32_vc71_i386',
                                                                  defaultLocation)
            
                # make sure the workspace/projects directory exists
                absPath = os.path.join(self.buildSys.homeDir, self.vcprojLocation)
                if not os.path.exists(absPath):
                    os.makedirs(absPath)
            
                self.buildSys.UpdateProgressDialog(progressVal,
                    'Generating %s...' % workspaceName)
                if self.buildSys.ProgressDialogCancelled():
                    break
            
                # spit out the files
                self.GenerateSolution(workspace)
                for targetName in workspace.targets:
                    self.GenerateProject(self.buildSys.targets[targetName],
                                         workspace)
                
                absPath = absPath.lower()
                if absPath not in solutionAbsDirs:
                    solutionAbsDirs.append(absPath)
                                    
                progressVal += 1
        except:
            self.buildSys.logger.exception('Exception in vstudio71.Generate()')
            
        self.buildSys.DestroyProgressDialog()
        
        summaryDetails = { 'numOfWorkspacesBuilt' : progressVal,
                           'totalNumOfWorkspaces' : len(workspaceNames) }
        self.buildSys.DisplaySummaryDialog(summaryDetails)
        
        for absPath in solutionAbsDirs:
            os.startfile(absPath)
        
    #--------------------------------------------------------------------------
    # .sln files aren't in XML, they require the use of tabs instead of spaces.
    def GenerateSolution(self, workspace):
        self.buildSys.logger.info('Generating VS.NET 2003 solution %s',
                                  workspace.name)

        # write .sln file
        solutionPath = os.path.join(self.buildSys.homeDir, self.vcprojLocation, 
                                    workspace.name + '.sln')
        try:
            slnFile = file(solutionPath, 'w')
        except IOError:
            self.buildSys.logger.error("Couldn't open %s for writing.", 
                                       solutionPath)
        else:
            # header
            slnFile.write('Microsoft Visual Studio Solution File, Format Version 8.00\n')

            # get a uuid for each project
            for targetName in workspace.targets:
                target = self.buildSys.targets[targetName]
                if '' == target.uuid: # may have been generated previously
                    if target.SupportsPlatform('win32'):
                        #print 'Target ' + target.name + ' supports win32.'
                        target.uuid = self.GenerateUUID()
    
            # now with the UUIDs we can write the targets
            for targetName in workspace.targets:
                target = self.buildSys.targets[targetName]
                slnFile.write('Project("{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}") = "%s", "%s.vcproj", "{%s}"\n' \
                              % (targetName, targetName, target.uuid))
                slnFile.write('\tProjectSection(ProjectDependencies) = postProject\n')
                for targetDepName in target.depends:
                    targetDep = self.buildSys.targets[targetDepName]
                    if targetDep.SupportsPlatform('win32'):
                        slnFile.write('\t\t{%s} = {%s}\n' \
                                      % (targetDep.uuid, targetDep.uuid))
                slnFile.write('\tEndProjectSection\n')
                slnFile.write('EndProject\n')
    
            slnFile.write(STR_SLN_GLOBAL_HEADER)
            # configurations
            for targetName in workspace.targets:
                target = self.buildSys.targets[targetName]
                if target.SupportsPlatform('win32'):
                    slnFile.write(STR_SLN_PROJECT_CONFIGS \
                                  % {'uuid' : target.uuid })
            slnFile.write(STR_SLN_GLOBAL_FOOTER)

            slnFile.close()
        
    #--------------------------------------------------------------------------
    def GenerateProject(self, target, workspace):
        print 'Generating VS.NET 2003 project: ' + target.name

        # write .vcproj file
        projPath = os.path.join(self.buildSys.homeDir, self.vcprojLocation, 
                                target.name + '.vcproj')
        try:
            projFile = file(projPath, 'w')
        except IOError:
            self.buildSys.logger.error('Failed to open %s for writing!', 
                                       projPath)
        else:
            self.writeProjectHeader(target, projFile)
            projFile.write('    <Configurations>\n')
            self.writeProjectConfig(target, workspace, projFile, True)
            self.writeProjectConfig(target, workspace, projFile, False)
            projFile.write('    </Configurations>\n' \
                           '    <References>\n' \
                           '    </References>\n')
            self.writeProjectFiles(target, projFile)
            self.writeProjectFooter(projFile)
            projFile.close()
        
    #--------------------------------------------------------------------------
    # Generates and returns a UUID (a string).
    # If an error occured the return value will be an empty string.
    def GenerateUUID(self):
        uuidStr = buildsys3.guid.GenerateGUID()
        if uuidStr != '':
            uuidStr = string.upper(string.strip(uuidStr))
        return uuidStr
        
    #--------------------------------------------------------------------------
    # Private Stuff
    #--------------------------------------------------------------------------
        
    #--------------------------------------------------------------------------
    def writeProjectHeader(self, target, projFile):
        args = { 'targetName' : target.name,
                 'targetUUID' : target.uuid }
        projFile.write(STR_PROJECT_HEADER % args)

    #--------------------------------------------------------------------------
    def writeProjectConfig(self, target, workspace, projFile, debugMode):
        defStr = '' # preprocessor definitions

        modDefFileName = target.modDefFile
        if '' != modDefFileName:
            modDefFileName = self.buildSys.FindRelPath(self.vcprojLocation,
                                                       modDefFileName)

        configType = 0
        extension = target.GetExtension('win32')
        prefix = ''
        if 'lib' == target.type:
            configType = 4
            if debugMode:
                prefix = 'd_'
            defStr = 'N_STATIC;'
        elif 'dll' == target.type:
            configType = 2
        elif 'exe' == target.type:
            configType = 1
            defStr = 'N_STATIC;'

        defStr += workspace.GetTargetDefsStringForTarget(target.name)
        
        interDir = 'inter'
        binaryDir = workspace.GetBinaryOutputPath('bin')
        binaryDir = self.buildSys.FindRelPath(self.vcprojLocation, binaryDir)
        
        configName = ''
        win32Libs = ''
        
        if debugMode:
            configName = 'Debug|Win32'
            interDir = os.path.join(interDir, 'win32d')
            binaryDir = os.path.join(binaryDir, 'win32d')
            firstLib = True
            for lib in target.libsWin32DebugAll:
                if not firstLib:
                    win32Libs += ' '
                win32Libs += lib
                firstLib = False
            defStr += 'WIN32;_DEBUG;NT_PLUGIN;'
        else:
            configName = 'Release|Win32'
            interDir = os.path.join(interDir, 'win32')
            binaryDir = os.path.join(binaryDir, 'win32')
            firstLib = True
            for lib in target.libsWin32ReleaseAll:
                if not firstLib:
                    win32Libs += ' '
                win32Libs += lib
                firstLib = False
            defStr += 'WIN32;NDEBUG;NT_PLUGIN;'

        args = { 'configName' : configName,
                 'binaryDir'  : binaryDir,
                 'interDir'   : os.path.join(interDir, target.name),
                 'configType' : configType,
                 'incDirs'    : self.incDirStr,
                 'defs'       : defStr }
        projFile.write(STR_PROJ_CONFIG_S1 % args)

        if debugMode:
            args = { 'exceptions' : string.upper(str(target.exceptions)),
                     'rtti'       : string.upper(str(target.rtti)) }
            projFile.write(STR_PROJ_CONFIG_S2_DEBUG % args)
        else:
            args = { 'exceptions' : string.upper(str(target.exceptions)),
                     'rtti'       : string.upper(str(target.rtti)) }
            projFile.write(STR_PROJ_CONFIG_S2_RELEASE % args)
    
        projFile.write('            <Tool Name="VCCustomBuildTool"/>\n')

        # now either the linker or the librarian
        if 'lib' == target.type:
            projFile.write(STR_PROJ_CONFIG_LIBRARIAN_TOOL \
                           % os.path.join(interDir, 
                                 prefix + target.name + '.' + extension))
        else:
            args = { 'win32Libs'  : win32Libs,
                     'outputFile' : os.path.join(binaryDir,
                                        target.name + '.' + extension) }
            projFile.write(STR_PROJ_CONFIG_LINKER_TOOL_S1 % args)
            if debugMode:
                projFile.write(STR_PROJ_CONFIG_LINKER_TOOL_S2_DEBUG \
                               % os.path.join(interDir, target.name + '.pdb'))
            else:
                projFile.write(STR_PROJ_CONFIG_LINKER_TOOL_S2_RELEASE)
            
            args = { 'libDirs' : interDir + ';' + self.libDirStr,
                     'modDef'  : modDefFileName }
            projFile.write(STR_PROJ_CONFIG_LINKER_TOOL_S3 % args)

        # the other tools
        projFile.write(STR_PROJ_CONFIG_OTHER_TOOLS)

        if 'exe' == target.type:
            projFile.write(STR_PROJ_CONFIG_TOOL_RES_COMPILER_EXE)
        else:
            projFile.write(STR_PROJ_CONFIG_TOOL_RES_COMPILER)

        projFile.write('        </Configuration>\n')
        
    #--------------------------------------------------------------------------
    def writeProjectFiles(self, target, projFile):
        projFile.write('    <Files>\n')

        for moduleName in target.modules:
            module = self.buildSys.modules[moduleName]
            compileAsFlag = 0
            if 'c' == module.modType:
                compileAsFlag = 1
            elif 'cpp' == module.modType:
                compileAsFlag = 2
            
            safeModName = module.GetFullNameNoColons()
            nebSyms = 'N_INIT=n_init_' + safeModName + ';' \
                      'N_NEW=n_new_' + safeModName + ';' \
                      'N_INITCMDS=n_initcmds_' + safeModName
                    
            projFile.write('        <Filter Name="%s" Filter="cpp;c;cxx;cc;h;hxx;hcc">\n' \
                           % module.name)

            # source files
            for fileName in module.resolvedFiles:
                relPath = self.buildSys.FindRelPath(self.vcprojLocation, 
                                                    fileName)
                ignore, shortFileName = os.path.split(fileName)
                objectName, ignore = os.path.splitext(shortFileName)
                args = { 'relPath' : relPath,
                         'nebSyms' : nebSyms,
                         'objectName' : ('%s_%s' % (safeModName, objectName)),
                         'compileAsFlag' : compileAsFlag }
                projFile.write(STR_SRC_FILE % args)

            # header files
            for fileName in module.resolvedHeaders:
                relPath = self.buildSys.FindRelPath(self.vcprojLocation, 
                                                    fileName)
                projFile.write('            <File RelativePath="%s"/>\n' \
                               % relPath)

            projFile.write('        </Filter>\n')

        # if the module definition file is set and target is dll then add it
        if ('' != target.modDefFile) and ('dll' == target.type):
            relPath = self.buildSys.FindRelPath(self.vcprojLocation,
                                                target.modDefFile)
            projFile.write('        <Filter Name="Module Definition" Filter="def">\n' \
                           '            <File RelativePath="%s"/>\n' \
                           '        </Filter>\n' % relPath)

        # resource files
        if ('exe' == target.type) or ('dll' == target.type):
            projFile.write('        <Filter Name="Resource Files" Filter="rc">\n')
            # add standard nebula rsrc to exe
            if 'exe' == target.type:
                relPath = self.buildSys.FindRelPath(self.vcprojLocation,
                                                    os.path.join('build',
                                                                 'pkg'))
                #print 'Relative Resource Path: ' + relPath
                relPath = os.path.join(relPath, 'res_' + target.name + '.rc')
                projFile.write('            <File RelativePath="%s"/>\n' \
                               % relPath)
            # add any custom resource files
            if '' != target.win32Resource:
                relPath = self.buildSys.FindRelPath(self.vcprojLocation,
                                                    target.win32Resource)
                projFile.write('            <File RelativePath="%s"/>\n' \
                               % relPath)
            projFile.write('        </Filter>\n')
            
        projFile.write('    </Files>\n')
        
    #--------------------------------------------------------------------------
    def writeProjectFooter(self, projFile):
        projFile.write(STR_PROJECT_FOOTER)
    
#--------------------------------------------------------------------------
# EOF
#--------------------------------------------------------------------------
