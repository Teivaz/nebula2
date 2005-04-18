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
    
    #--------------------------------------------------------------------------
    def Clean(self):
        self.modDefFile = self.buildSys.CleanRelPath(self.modDefFile)
    
    #--------------------------------------------------------------------------
    # Resolve the filenames in the module definition to real paths, relative
    # to the Nebula home directory.
    def ResolvePaths(self):
        #print 'Resolving paths for module ' + self.name
        
        # get the trunk dir at home/baseDir/(inc/src)/dir
        for projectDir in self.buildSys.projectDirs:
            #print projectDir
        
            # extract the last component of projectDir
            head, tail = os.path.split(string.rstrip(projectDir, os.sep))
            
            if os.path.isdir(os.path.join(projectDir, 'inc', self.dir)):
                self.codeDir = tail
                break
        
            if os.path.isdir(os.path.join(projectDir, 'src', self.dir)):
                self.codeDir = tail
                break

            # if current subdir is contrib search inside
            if 'contrib' == tail:
                contribMod = False
                for contribDir in self.buildSys.contribDirs:
                    #print contribDir
                    
                    # extract the last component of contribDir
                    head, tail = os.path.split(string.rstrip(contribDir, os.sep))

                    if os.path.isdir(os.path.join(contribDir, 'inc', self.dir)):
                        self.codeDir = os.path.join('contrib', tail)
                        contribMod = True
                        break

                    if os.path.isdir(os.path.join(contribDir, 'src', self.dir)):
                        self.codeDir = os.path.join('contrib', tail)
                        contribMod = True
                        break
                    
                if contribMod:
                    break;
    
        # special case to deal with dummy.cc >:|
        if '.' == self.dir:
            self.dir = ''
            self.codeDir = 'nebula2'
    
        self.resolvedFiles = []
        for srcFile in self.files:
            root, ext = os.path.splitext(srcFile)
            # no extension? add the default .cc extension
            if '' == ext:
                srcFile = srcFile + '.cc'
            resolvedPath = os.path.join('code', self.codeDir, 'src', self.dir, 
                                        srcFile)
            self.resolvedFiles.append(resolvedPath)
        
        self.resolvedHeaders = []
        for hdrFile in self.headers:
            root, ext = os.path.splitext(hdrFile)
            # no extension? add the default .cc extension
            if '' == ext:
                hdrFile = hdrFile + '.h'
            resolvedPath = os.path.join('code', self.codeDir, 'inc', self.dir, 
                                        hdrFile)
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
                            '\(([a-zA-Z0-9_," ]+)\);?\s*$')
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
