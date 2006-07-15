#--------------------------------------------------------------------------
# (c) 2005 Vadim Macagon
#
# Contents are licensed under the Nebula license.
#--------------------------------------------------------------------------

import re, os, string

class Module:
    
    #--------------------------------------------------------------------------
    def __init__(self, moduleName, bldFilename):
        self.buildSys = None
        # these more or less directly correspond to stuff in bld files
        self.name = moduleName
        self.annotation = ''
        self.dir = ''
        self.platform = 'all'
        self.modType = 'cpp'
        self.files = []
        self.headers = []
        self.libsWin32 = []
        self.libsWin32Debug = []
        self.libsWin32Release = []
        self.libsLinux = []
        self.libsMacOSX = []
        self.frameworksMacOSX = []
        self.moduleDeps = []
        #self.nopkg = False
        self.putInPkg = True
        self.modDefFile = ''
        self.bldFile = bldFilename
        # these are deduced after reading in the bld files
        self.ancestor = None
        self.resolvedFiles = []
        self.resolvedHeaders = []
        self.codeDir = ''
        self.baseIncDir = ''
        self.baseSrcDir = ''
    
    #--------------------------------------------------------------------------
    # Get the fully qualified name of the module, but replace all :: with _
    # so the string can be safely used as part of a filename or as part of
    # a C++ identifier.
    def GetFullNameNoColons(self):
        return self.name.replace('::', '_')
    
    #--------------------------------------------------------------------------
    def Clean(self):
        self.modDefFile = self.buildSys.CleanRelPath(self.modDefFile)
    
    #--------------------------------------------------------------------------
    # Sort files and headers
    def Finalize(self):
        self.files.sort()
        self.headers.sort()

    #--------------------------------------------------------------------------
    # Returns True if the target can be built on the specified platform,
    # false otherwise.
    def SupportsPlatform(self, platform):
        if self.platform == 'all' or self.platform == platform:
            return True
        return False

    #--------------------------------------------------------------------------
    # Get the directory (relative to the Nebula home directory) that the 
    # compiler will need to add to the list of include directories for any 
    # source file that includes headers from this module.
    # Note that the result of os.path.join(self.GetBaseIncDir(), self.dir) is 
    # the directory where the module *.h files live.
    def GetBaseIncDir(self):
        return self.baseIncDir
        
    #--------------------------------------------------------------------------
    # Get the directory (relative to the Nebula home directory) that contains
    # the module source directory.
    # Note that the result of os.path.join(self.GetBaseSrcDir(), self.dir) is 
    # the directory where the module *.cc files live.
    def GetBaseSrcDir(self):
        return self.baseSrcDir

    #--------------------------------------------------------------------------
    # Get the base directory of the project this module resides in, the path
    # returned is absolute.
    def GetProjectDir(self):
        modProjectDir = ''
        for projectDir in self.buildSys.projectDirs:
            tempDir = os.path.commonprefix([projectDir, 
                          self.buildSys.GetAbsPathFromRel(self.bldFile)])
            if len(tempDir) > len(modProjectDir):
                modProjectDir = tempDir
        return modProjectDir

    #--------------------------------------------------------------------------
    # Resolve the filenames in the module definition to real paths, relative
    # to the Nebula home directory.
    def ResolvePaths(self):
        #print 'Resolving paths for module ' + self.name
        
        projectDir = self.GetProjectDir()
        ignore, projectDirName = os.path.split(string.rstrip(projectDir, os.sep))
        mangaloreMod = False

        if 'contrib' == projectDirName:
            # search inside contrib
            for contribDir in self.buildSys.contribDirs:
                # extract the last component of contribDir
                ignore, tail = os.path.split(string.rstrip(contribDir, os.sep))

                if os.path.isdir(os.path.join(contribDir, 'inc', self.dir)):
                    self.codeDir = os.path.join('contrib', tail)
                    break

                if os.path.isdir(os.path.join(contribDir, 'src', self.dir)):
                    self.codeDir = os.path.join('contrib', tail)
                    break
        else:
            # handle Nebula 2 and user projects
            if os.path.isdir(os.path.join(projectDir, 'inc', self.dir)):
                self.codeDir = projectDirName
            elif os.path.isdir(os.path.join(projectDir, 'src', self.dir)):
                self.codeDir = projectDirName            
            # try mangalore style directory layout
            elif os.path.isdir(os.path.join(projectDir, self.dir)):
                self.codeDir = projectDirName
                mangaloreMod = True
    
        if '' == self.codeDir:
            self.buildSys.logger.error('Failed to locate source code for '
                                       'module %s defined in %s', 
                                       self.name, self.bldFile)
    
        # special case to deal with dummy.cc >:|
        if '.' == self.dir:
            self.dir = ''
            self.codeDir = 'nebula2'
    
        if mangaloreMod:
            self.baseIncDir = os.path.join('code', self.codeDir)
            self.baseSrcDir = os.path.join('code', self.codeDir)
        else:
            self.baseIncDir = os.path.join('code', self.codeDir, 'inc')
            self.baseSrcDir = os.path.join('code', self.codeDir, 'src')
    
        self.resolvedFiles = []
        for srcFile in self.files:
            root, ext = os.path.splitext(srcFile)
            # no extension? add the default .cc extension
            resolvedPath = os.path.join(self.baseSrcDir, self.dir, srcFile)
            if '' == ext:
                if os.path.exists(resolvedPath + '.cc'):
                    resolvedPath = resolvedPath + '.cc'
                elif os.path.exists(resolvedPath + '.cpp'):
                    resolvedPath = resolvedPath + '.cpp'
                elif os.path.exists(resolvedPath + '.c'):
                    resolvedPath = resolvedPath + '.c'
            
            self.resolvedFiles.append(resolvedPath)
        
        self.resolvedHeaders = []
        for hdrFile in self.headers:
            root, ext = os.path.splitext(hdrFile)
            # no extension? add the default .h extension
            resolvedPath = os.path.join(self.baseIncDir, self.dir, hdrFile)
            if '' == ext:
                if os.path.exists(resolvedPath + '.h'):
                    resolvedPath = resolvedPath + '.h'
                elif os.path.exists(resolvedPath + '.hpp'):
                    resolvedPath = resolvedPath + '.hpp'
            self.resolvedHeaders.append(resolvedPath)
            
        #print "Resolved Files:"
        #print self.resolvedFiles
        #print "Resolved Headers:"
        #print self.resolvedHeaders
        
    #--------------------------------------------------------------------------
    # Find and set the ancestor module, also figure out if we'll need to
    # generate a pack file for this module later.
    # Returns False if any errors occured, True otherwise.
    def FindAncestor(self):
        foundClassMacro = False
        hasAncestor = False
        detectedError = False
        regexp = re.compile('^\s*('
                            'nNebulaRootClass|'
                            'nNebulaClassStaticInit|'
                            'nNebulaClass|'
                            'nNebulaScriptClassStaticInit|'
                            'nNebulaScriptClass|)'
                            '\(([a-zA-Z0-9:_," ]+)\);?\s*$')
        for fileName in self.resolvedFiles:
            if not os.path.exists(fileName):
                self.buildSys.logger.warning('%s referenced in module %s'\
                                             ' doesn\'t exist!', fileName, 
                                             self.name)
                continue
            srcFile = file(fileName, 'rU')
            while (not foundClassMacro) and (not detectedError):
                detectedError = False
                line = srcFile.readline()
                if '' == line:
                    break
                matches = regexp.match(line)
                if matches:
                    macroStr = matches.group(1)
                    argStr = matches.group(2)
                    args = string.split(argStr, ',')
                    if 'nNebulaRootClass' == macroStr:
                        if len(args) == 1:
                            foundClassMacro = True
                            hasAncestor = False
                        else:
                            detectedError = True
                            self.buildSys.logger.error('Malformed '\
                                'nNebulaRootClass macro in ' + fileName)
                    elif 'nNebulaClass' == macroStr:
                        if len(args) == 2:
                            foundClassMacro = True
                            hasAncestor = True
                        else:
                            detectedError = True
                            self.buildSys.logger.error('Malformed '\
                                'nNebulaClass macro in ' + fileName)
                    elif 'nNebulaScriptClass' == macroStr:
                        if len(args) == 2:
                            foundClassMacro = True
                            hasAncestor = True
                        else:
                            detectedError = True
                            self.buildSys.logger.error('Malformed '\
                                'nNebulaScriptClass macro in ' + fileName)
                    elif 'nNebulaClassStaticInit' == macroStr:
                        if len(args) == 3:
                            foundClassMacro = True
                            hasAncestor = True
                        else:
                            detectedError = True
                            self.buildSys.logger.error('Malformed ' \
                                'nNebulaClassStaticInit macro in ' + fileName)
                    elif 'nNebulaScriptClassStaticInit' == macroStr:
                        if len(args) == 3:
                            foundClassMacro = True
                            hasAncestor = True
                        else:
                            detectedError = True
                            self.buildSys.logger.error('Malformed ' \
                                'nNebulaScriptClassStaticInit macro in %s',
                                fileName)
                    if hasAncestor:
                        ancestorName = string.strip(args[1], '" ')
                        if ancestorName in self.buildSys.modules:
                            self.ancestor = self.buildSys.modules[ancestorName];
                        else:
                            detectedError = True
                            self.buildSys.logger.error('Undefined module %s' \
                                ' referenced in a nNebula*Class* macro in %s',
                                ancestorName, fileName)
            srcFile.close()
            if foundClassMacro or detectedError:
                break

        # if there is no class macro found the module won't be in a pkg
        if not foundClassMacro:
            #print 'Warning: module ' + self.name + ' is missing a class ' \
            #      'macro so it won\'t be added to a pkg!'
            self.putInPkg = False
        
        return not detectedError
        
#--------------------------------------------------------------------------
# EOF
#--------------------------------------------------------------------------
