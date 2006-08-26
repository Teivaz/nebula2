#--------------------------------------------------------------------------
# (c) 2005 Vadim Macagon
#
# Contents are licensed under the Nebula license.
#--------------------------------------------------------------------------

import os, string
from module import Module

class Target:

    #--------------------------------------------------------------------------
    def __init__(self, targetName, bldFilename):
        self.buildSys = None
        # these more or less directly correspond to stuff in bld files
        self.name = targetName
        self.annotation = ''
        self.type = 'lib'
        self.rtti = False
        self.exceptions = False
        self.icon = os.path.join('code', 'nebula2', 'res', 'nebula.ico')
        self.win32Resource = ''
        self.modules = []
        self.bundles = []
        self.depends = []
        self.dirs = []
        self.extension = ''
        self.bldFile = bldFilename
        # these are deduced later on
        self.modDefFile = ''
        self.libsWin32 = [] # libs to link against in debug & release
        self.libsWin32Debug = [] # libs to link against only in debug
        self.libsWin32Release = [] # libs to link against only in release
        self.libsLinux = []
        self.libsMacOSX = []
        self.frameworksMacOSX = []
        self.libsWin32DebugAll = [] # libsWin32Debug + libsWin32
        self.libsWin32ReleaseAll = [] # libsWin32Release + libsWin32
        # platforms this target can be built on
        self.platforms = {'win32'  : False,
                          'linux'  : False,
                          'macosx' : False,
                          'all'    : False}
        # filled out and used by generators
        self.uuid = '' # only used by Visual Studio generators atm

    #--------------------------------------------------------------------------
    # Get the fully qualified name of the target, but replace all :: with _
    # so the string can be safely used as part of a filename or as part of
    # a C++ identifier.
    def GetFullNameNoColons(self):
        return self.name.replace('::', '_')

    #--------------------------------------------------------------------------
    def Clean(self):
        self.win32Resource = self.buildSys.CleanRelPath(self.win32Resource)
        self.modDefFile = self.buildSys.CleanRelPath(self.modDefFile)

    #--------------------------------------------------------------------------
    def GetExtension(self, platform):
        if '' != self.extension:
            return self.extension
        else:
            if 'exe' == self.type:
                return 'exe'
            elif 'lib' == self.type:
                return 'lib'
            elif 'dll' == self.type:
                if 'win32' == platform:
                    return 'dll'
                elif 'linux' == platform:
                    return 'so'
            else:
                self.buildSys.logger.error('Unrecognized target type %s' \
                                           ' for target %s in %s',
                                           self.type, self.name, self.bldFile)
        return 'exe' # should never get here, normally :)

    #--------------------------------------------------------------------------
    def Validate(self):
        dataValid = True
        # check target modules are defined
        for moduleName in self.modules:
            if moduleName not in self.buildSys.modules:
                self.buildSys.logger.error('Undefined module %s referenced' \
                                           ' in target %s from %s',
                                           moduleName, self.name, self.bldFile)
                dataValid = False
            if self.buildSys.modules[moduleName].dir not in self.dirs:
                self.dirs.append(self.buildSys.modules[moduleName].dir)
        # check target bundles are defined
        for bundleName in self.bundles:
            if bundleName not in self.buildSys.bundles:
                self.buildSys.logger.error('Undefined bundle %s referenced' \
                                           ' in target %s from %s',
                                           bundleName, self.name, self.bldFile)
                dataValid = False
        # check target dependencies are defined
        for targetDepName in self.depends:
            if targetDepName not in self.buildSys.targets:
                self.buildSys.logger.error('Undefined target dependency %s' \
                                           ' referenced in target %s from %s',
                                           targetDepName, self.name,
                                           self.bldFile)
                dataValid = False
        return dataValid

    #--------------------------------------------------------------------------
    # Merge the bundled targets and modules.
    def MergeBundles(self):
        for bundleName in self.bundles:
            curBundle = self.buildSys.bundles[bundleName]
            for moduleName in curBundle.modules:
                if moduleName not in self.modules:
                    self.modules.append(moduleName)
                    if self.buildSys.modules[moduleName].dir not in self.dirs:
                        self.dirs.append(self.buildSys.modules[moduleName].dir)
            for targetName in curBundle.targets:
                if targetName not in self.depends:
                    self.depends.append(targetName)

    #--------------------------------------------------------------------------
    # Simply figures out what platforms this target can be built on based on
    # what platforms the modules it contains can be built on.
    def AssesPlatformSupport(self):
        for moduleName in self.modules:
            curModule = self.buildSys.modules[moduleName]
            self.platforms[curModule.platform] = True
        if self.platforms['all']:
            for platform in self.platforms.keys():
                self.platforms[platform] = True
        #print 'Platform Support for ' + self.name + ': ' + str(self.platforms)

    #--------------------------------------------------------------------------
    # Returns True if the target can be built on the specified platform,
    # false otherwise.
    def SupportsPlatform(self, platform):
        return self.platforms[platform]

    #--------------------------------------------------------------------------
    # Find the .def file if any.
    def FindModuleDefFile(self):
        for moduleName in self.modules:
            curModule = self.buildSys.modules[moduleName]
            if '' != curModule.modDefFile:
                modDefFile = curModule.modDefFile
                root, ext = os.path.splitext(modDefFile)
                # no extension? add the default .def extension
                if '' == ext:
                    modDefFile = modDefFile + '.def'
                self.modDefFile = os.path.join('code', curModule.codeDir,
                                               'src', curModule.dir,
                                               modDefFile)
                if not os.path.exists(os.path.join(self.buildSys.homeDir,
                                                   self.modDefFile)):
                    self.buildSys.logger.warning("%s referenced in module %s "\
                                                 "doesn't exist!",
                                                 self.modDefFile, moduleName)

    #--------------------------------------------------------------------------
    # Collect libraries to be linked against from this target's modules.
    def CollectLibsFromMods(self):
        # obtain the libraries from the modules
        for moduleName in self.modules:
            curModule = self.buildSys.modules[moduleName]
            for libName in curModule.libsWin32:
                if libName not in self.libsWin32:
                    self.libsWin32.append(libName)
            for libName in curModule.libsWin32Debug:
                if libName not in self.libsWin32Debug:
                    self.libsWin32Debug.append(libName)
            for libName in curModule.libsWin32Release:
                if libName not in self.libsWin32Release:
                    self.libsWin32Release.append(libName)
            for libName in curModule.libsLinux:
                if libName not in self.libsLinux:
                    self.libsLinux.append(libName)
            for libName in curModule.libsMacOSX:
                if libName not in self.libsMacOSX:
                    self.libsMacOSX.append(libName)
            for frameworkName in curModule.frameworksMacOSX:
                if frameworkName not in self.frameworksMacOSX:
                    self.frameworksMacOSX.append(frameworkName)
        # win32 libs need some more shuffling
        self.libsWin32DebugAll.extend(self.libsWin32Debug)
        self.libsWin32ReleaseAll.extend(self.libsWin32Release)
        for libName in self.libsWin32:
            if libName not in self.libsWin32Debug:
                self.libsWin32DebugAll.append(libName)
            if libName not in self.libsWin32Release:
                self.libsWin32ReleaseAll.append(libName)

    #--------------------------------------------------------------------------
    # Collect libraries to be linked against from this target's dependecies.
    def CollectLibsFromDepends(self):
        for targetName in self.depends:
            target = self.buildSys.targets[targetName]
            for libName in target.libsWin32DebugAll:
                if libName not in self.libsWin32DebugAll:
                    self.libsWin32DebugAll.append(libName)
            for libName in target.libsWin32ReleaseAll:
                if libName not in self.libsWin32ReleaseAll:
                    self.libsWin32ReleaseAll.append(libName)
            for libName in target.libsLinux:
                if libName not in self.libsLinux:
                    self.libsLinux.append(libName)
            for libName in target.libsMacOSX:
                if libName not in self.libsMacOSX:
                    self.libsMacOSX.append(libName)
            for libName in target.frameworksMacOSX:
                if libName not in self.frameworksMacOSX:
                    self.frameworksMacOSX.append(libName)

    #--------------------------------------------------------------------------
    # Sort modules
    def Finalize(self):
        self.modules.sort()

    #--------------------------------------------------------------------------
    # Generate a pkg file for this target.
    def GeneratePkgFile(self):
        pkgDirectory = os.path.join(self.buildSys.homeDir, 'build', 'pkg')
        pkgMods = []
        # collect the modules that will need to be in the pkg
        for moduleName in self.modules:
            curModule = self.buildSys.modules[moduleName]
            if curModule.putInPkg:
                pkgMods.append(curModule)
        # if there are no modules to put in the pkg no point going on
        if len(pkgMods) == 0:
            return
        # sort the collected modules by ancestor
        sortedMods = []
        # step 1: find modules without ancestors
        for module in pkgMods:
            if module.ancestor not in pkgMods:
                sortedMods.append(module)
        # step 2: sort the modules that do have ancestors
        sortedModIdx = 0
        while sortedModIdx < len(pkgMods):
            for module in pkgMods:
                if module.ancestor == sortedMods[sortedModIdx]:
                    sortedMods.append(module)
            sortedModIdx += 1
        pkgMods = sortedMods
        # write out the header
        safeTarName = self.GetFullNameNoColons()
        pkgShortFileName = 'pkg_' + safeTarName + '.cc'
        pkgPath = os.path.join(pkgDirectory, pkgShortFileName)
        pkgFile = file(pkgPath, 'w')
        pkgFile.write('//' + ('-' * 77) + '\n')
        pkgFile.write('// ' + pkgShortFileName + '\n')
        pkgFile.write('// MACHINE GENERATED FROM\n')
        pkgFile.write('// ' + self.bldFile)
        pkgFile.write('\n// DON\'T EDIT!\n')
        pkgFile.write('//' + ('-' * 77) + '\n')
        pkgFile.write('#include "kernel/ntypes.h"\n')
        pkgFile.write('#include "kernel/nkernelserver.h"\n')
        pkgFile.write('#ifdef __XBxX__\n')
        pkgFile.write('#undef __WIN32__\n')
        pkgFile.write('#endif\n\n')
        pkgFile.write('extern "C" void ' + safeTarName + '();\n\n')
        # write out the extern declarations
        for module in pkgMods:
            platformTag = module.platform
            platformDef = ''
            if platformTag != 'all':
                if platformTag in self.buildSys.platformTagDefMap:
                    platformDef = self.buildSys.platformTagDefMap[platformTag]
            if platformDef != '':
                pkgFile.write('#ifdef ' + platformDef + '\n')
            safeModName = module.GetFullNameNoColons()
            pkgFile.write('extern bool n_init_%s (nClass *, nKernelServer *);\n' %
                          safeModName)
            pkgFile.write('extern void *n_new_%s (void);\n' % safeModName)
            if platformDef != '':
                pkgFile.write('#endif //' + platformDef + '\n\n')
        pkgFile.write('\nvoid ' + safeTarName + '()\n{\n')
        # write out the AddModule(s)
        for module in pkgMods:
            platformTag = module.platform
            platformDef = ''
            if platformTag != 'all':
                if platformTag in self.buildSys.platformTagDefMap:
                    platformDef = self.buildSys.platformTagDefMap[platformTag]
            if platformDef != '':
                pkgFile.write('#ifdef ' + platformDef + '\n')
            pkgFile.write('    nKernelServer::ks->AddModule("' \
                          + module.name + '",\n')
            safeModName = module.GetFullNameNoColons()
            pkgFile.write((' ' * 33) + 'n_init_%s,\n' % safeModName)
            pkgFile.write((' ' * 33) + 'n_new_%s);\n' % safeModName)
            if platformDef != '':
                pkgFile.write('#endif //' + platformDef + '\n')
        pkgFile.write('}\n\n')
        pkgFile.write('//' + ('-' * 77) + '\n')
        pkgFile.write('// EOF\n')
        pkgFile.write('//' + ('-' * 77) + '\n\n')
        pkgFile.close()
        # create a module for the pkg file
        pkgMod = Module('pkg_' + self.name, '** autogenerated **')
        pkgMod.resolvedFiles = [os.path.join('build', 'pkg', pkgShortFileName)]
        pkgMod.putInPkg = False
        self.buildSys.modules[pkgMod.name] = pkgMod
        # add it to the current target so that it gets built
        if pkgMod.name not in self.modules:
            self.modules.append(pkgMod.name)

    #--------------------------------------------------------------------------
    # Writes out a res_XXX.rc files an executable target that points to
    # the target's icon (code/nebula2/res by default).
    def GenerateResFile(self):
        if self.type != 'exe':
            return

        relPath = self.buildSys.FindRelPath(os.path.join('build', 'pkg'),
                                            self.icon)
        safeTarName = self.GetFullNameNoColons()
        absPath = os.path.join(self.buildSys.homeDir, 'build', 'pkg',
                               'res_' + safeTarName + '.rc')
        resFile = file(absPath, 'w')
        relPath = string.replace(relPath, os.sep, '\\\\')
        relPath = string.replace(relPath, '/', '\\\\')
        resFile.write('Icon ICON "' + relPath + '"')
        resFile.close()

#--------------------------------------------------------------------------
# EOF
#--------------------------------------------------------------------------
