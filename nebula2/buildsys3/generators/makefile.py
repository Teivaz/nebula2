#--------------------------------------------------------------------------
# Makefile generator for nebula2
#
# (C) 2005 James Urquhart,
# derived from the example scripts (c) 2005 Vadim Macagon.
#
# Contents are licensed under the Nebula license.
#--------------------------------------------------------------------------

import os, string

#--------------------------------------------------------------------------
#   Description of system:
#   One main Makefile is generated, which contains references to the base targets -
#   the "workspaces" of nebula.
#   Each workspace is generated in a seperate <workspace>.mak file, which contains
#   all the targets required to build every project in the workspace.

#--------------------------------------------------------------------------
class makefile:

    #--------------------------------------------------------------------------
    def __init__(self, buildSys):
        self.buildSys = buildSys
        self.description = 'Makefile ' \
                           'Generates Win, Mac, and Linux compatible makefiles'
        self.incDirStr = ''
        self.libDirStr = ''
        self.workspacePath = ''

    #--------------------------------------------------------------------------
    def HasSettings(self):
        return False

    #--------------------------------------------------------------------------
    # Gets basenames of all module sources
    def getModuleSources(self, module):
        retSources = ""
        for source in module.resolvedFiles:
            retSources += "$(N_INTERDIR)%s$(OBJ) " % string.join(string.split(os.path.split(source)[-1], '.')[0:-1])
        return retSources

    #--------------------------------------------------------------------------
    # Entrypoint
    def Generate(self, workspaceNames):
        defaultLocation = os.path.join('build', 'makefile')
        releaseObjLocation = os.path.join('build', 'makefile', 'inter', 'linux')
        debugObjLocation = os.path.join('build', 'makefile', 'inter', 'linuxd')
        try:
            os.makedirs(releaseObjLocation)
        except OSError:
            pass # in case the dir already exists
        try:
            os.makedirs(debugObjLocation)
        except OSError:
            pass # in case the dir already exists

        progressVal = 0
        self.buildSys.CreateProgressDialog('Generating Makefiles', '',
                                           len(workspaceNames))

        try:
            makeFile = file(os.path.join(defaultLocation, 'Makefile'), "w")

            makeFile.write("# Makefile for nebula\n")
            makeFile.write("include ../../buildsys3/config.mak\n\n")

            for workspaceName in workspaceNames:
                workspace = self.buildSys.workspaces[workspaceName]

                # calculate these once for the workspace
                self.workspacePath = workspace.GetWorkspacePath(defaultLocation)
                self.incDirStr = workspace.GetIncSearchDirsString(defaultLocation)
                self.libDirStr = workspace.GetLibSearchDirsString('win32_vc_i386',
                                                                  defaultLocation)

                # make sure the workspace/projects directory exists
                absPath = os.path.join(self.buildSys.homeDir, self.workspacePath)
                if not os.path.exists(absPath):
                    os.makedirs(absPath)

                self.buildSys.UpdateProgressDialog(progressVal,
                    'Generating %s...' % workspaceName)
                if self.buildSys.ProgressDialogCancelled():
                    break

                # spit out the target
                makeFile.write("%s: \n" % workspaceName)
                makeFile.write("\t@$(MAKE) -f ./%s.mak EXTRA_CFLAGS=\"" % workspaceName)
                for include in string.split(self.incDirStr, ';'):
                    makeFile.write("$(IPATH_OPT)%s " % include)
                for lib in string.split(self.libDirStr, ';'):
                    makeFile.write("$(LPATH_OPT)%s " % lib)
                makeFile.write("\"\n\n")

                # spit out the files
                self.generateWorkspace(workspace)

                progressVal += 1

            # generic targets
            makeFile.write("all: ")
            for workspaceName in workspaceNames:
                makeFile.write("%s " % workspaceName)
            makeFile.write("\n\n")

            # spring clean
            makeFile.write("clean: \n")
            makeFile.write("\t$(RM) $(N_TARGETDIR)*\n")
            makeFile.write("\t$(RM) $(N_INTERDIR)*\n")

            makeFile.write("default: all\n")

            # none of these are real
            makeFile.write(".PHONY: all ")
            for workspaceName in workspaceNames:
                makeFile.write("%s " % workspaceName)
            makeFile.write("\n\n")

            makeFile.close()
        except:
            self.buildSys.logger.exception('Exception in makefile.Generate()')

        self.buildSys.DestroyProgressDialog()

        summaryDetails = { 'numOfWorkspacesBuilt' : progressVal,
                           'totalNumOfWorkspaces' : len(workspaceNames) }
        self.buildSys.DisplaySummaryDialog(summaryDetails)

    #--------------------------------------------------------------------------
    # Private Stuff
    #--------------------------------------------------------------------------

    #--------------------------------------------------------------------------
    # Writes each module section
    def writeModule(self, module, makeFile):
        safeModName = module.GetFullNameNoColons()
        more_syms = "$(SYM_OPT)N_INIT=n_init_%s $(SYM_OPT)N_NEW=n_new_%s $(SYM_OPT)N_INITCMDS=n_initcmds_%s" % (safeModName, safeModName, safeModName)

        if module.modType == "c":
            makeFile.write("DO_COMPILE_%s = $(CC) $(NOLINK_OPT) $(CFLAGS) %s $< $(OBJ_OPT) $@\n" % (safeModName, more_syms))
        elif module.modType == "cpp":
            makeFile.write("DO_COMPILE_%s = $(CXX) $(NOLINK_OPT) $(CXXFLAGS) %s $< $(OBJ_OPT) $@\n" % (safeModName, more_syms))
        else:
            makeFile.write("DO_COMPILE_%s = @echo ""Warning: Dummy Target for %s!""\n" % safeModName)

        for fileName in module.resolvedFiles:
            base_name = string.join(string.split(os.path.split(fileName)[-1], '.')[0:-1])
            relPath = self.buildSys.FindRelPath(self.workspacePath, fileName)
            makeFile.write("$(N_INTERDIR)%s$(OBJ): %s\n\t$(DO_COMPILE_%s)\n" % (base_name, relPath, safeModName))
        makeFile.write("\n")

    #--------------------------------------------------------------------------
    # Writes each target section
    def writeTarget(self, target, makeFile):
        print "Generating target line for %s" % target.name

        makeFile.write("\n# Target %s\n" % target.name)
        # handle platform specific targets (unless non-specified)
        needsEnd = False
        if not target.SupportsPlatform("all"):
            if target.SupportsPlatform("win32"):
                makeFile.write("ifeq ($(N_PLATFORM),__WIN32__)\n")
            elif target.SupportsPlatform("linux"):
                makeFile.write("ifeq ($(N_PLATFORM),__LINUX__)\n")
            elif target.SupportsPlatform("macosx"):
                makeFile.write("ifeq ($(N_PLATFORM),__MACOSX__)\n")
            needsEnd = True

        targetType = target.type
        filesWin32  = ""
        filesLinux  = ""
        filesMacOSX = ""

        # Add modules we need to the files line
        for moduleName in target.modules:
            module = self.buildSys.modules[moduleName]
            sources = self.getModuleSources(module)
            # Module has sources, onto the platform...
            if not module.SupportsPlatform("all"):
                if module.SupportsPlatform("win32"):
                    filesWin32  += sources
                elif module.SupportsPlatform("linux"):
                    filesLinux  += sources
                elif module.SupportsPlatform("macosx"):
                    filesMacOSX += sources
                else:
                    print "Module %s not found on any regular platform! RUN!!!" % module.name
            else:
                filesWin32  += " %s" % sources
                filesLinux  += " %s" % sources
                filesMacOSX += " %s" % sources

        # handle resource files
        resourceFiles = ""
        if targetType == "exe" or targetType == "dll":
            makeFile.write("# Resource Files\n")
            # add standard nebula resource (exe or dll)
            relPath = os.path.join(self.buildSys.FindRelPath(self.workspacePath,
                                                os.path.join('build',
                                                             'pkg')), "res_%s.rc" % target.name)
            resourceFiles += "./pkg/res_%s.res " % relPath

            # add any custom resource files
            if target.win32Resource != "":
                resourceFiles += self.buildSys.FindRelPath(self.workspacePath,
                                                           target.win32Resource) + " "

            makeFile.write("\n")

        # Get a list of libs to plonk on command
        libsWin32  = ""
        libsLinux  = ""
        libsMacOSX = ""
        #for lib in target.libsWin32DebugAll:
        for lib in target.libsWin32ReleaseAll:
            libsWin32  += "$(LIB_OPT)%s$(LIB_OPT_POST) " % lib
        for lib in target.libsLinux:
            libsLinux  += "$(LIB_OPT)%s$(LIB_OPT_POST) " % lib
        for lib in target.libsMacOSX:
            libsMacOSX += "$(LIB_OPT)%s$(LIB_OPT_POST) " % lib

        # generate list of target dependencies for that target
        depsWin32 = ""
        depsLinux = ""
        depsMacOSX = ""
        for dep in target.depends:
            if not self.buildSys.targets[dep].SupportsPlatform("all"):
                if self.buildSys.targets[dep].SupportsPlatform("win32"):
                    depsWin32  += " %s" % dep
                elif self.buildSys.targets[dep].SupportsPlatform("linux"):
                    depsLinux  += " %s" % dep
                elif self.buildsys.targets[dep].SupportsPlatform("macosx"):
                    depsMacOSX += " %s" % dep
                else:
                    print "Dependancy %s not found on any regular platform! RUN!!!"
            else:
                depsWin32  += " %s" % dep
                depsLinux  += " %s" % dep
                depsMacOSX += " %s" % dep

        makeFile.write("# Linking targets\n")

        # compile target itself
        # TODO: fix erroneous "only __VC__ must be win32" assumption
        if targetType == "dll":
            # A Dynamic Lazy Lounger (or Shared Library)
            # Add on special .def if required
            if target.modDefFile != "":
                modDefFileName = self.buildSys.FindRelPath(self.workspacePath,
                                                           target.modDefFile)
                # NOTE: seems like "dlltool" is requred to generate the linking lib,
                # which def's are used with. Need to find a better way of handling this...
                #gccDef = " %s" % modDefFileName
                vccDef = "/DEF:%s" % modDefFileName
            else:
                vccDef = ""
                #gccDef = ""

            # Generate target lines
            endTarget = "$(N_TARGETDIR)$(DLL_PRE)%s$(DLL_POST)" % target.name
            makeFile.write("ifeq ($(N_COMPILER),__VC__)\n")
            makeFile.write("%s : %s %s\n" % (target.name, depsWin32, endTarget))
            makeFile.write("%s : %s %s\n" % (endTarget, filesWin32, resourceFiles))
            makeFile.write("\t$(LD) $(LFLAGS) $^ %s /OUT:$@ $(LIBDIR) $(LIBS) %s /DLL\n" % (vccDef, libsWin32))
            makeFile.write("else\n")
            makeFile.write("ifeq ($(N_PLATFORM),__MACOSX__)\n")
            makeFile.write("%s : %s %s\n" % (target.name, depsMacOSX, endTarget))
            makeFile.write("%s : %s\n" % (endTarget, filesMacOSX))
            makeFile.write("\t$(CXX) -dynamiclib -compatibility_version 0.1 -current_version 0.1.0 $^ -o $@ $(CFLAGS) $(LIBDIR) $(LIBS) %s\n" % (libsMacOSX))
            makeFile.write("else\n")
            makeFile.write("%s : %s %s\n" % (target.name, depsLinux, endTarget))
            makeFile.write("%s : %s\n" % (endTarget, filesLinux))
            makeFile.write("\t$(CXX) $^ -o $@ $(CFLAGS) $(LIBDIR) $(LIBS) %s -shared\n" % (libsLinux))
            makeFile.write("endif\n")
            makeFile.write("endif\n")
        elif targetType == "exe":
            # our trusty executable
            endTarget = "$(N_TARGETDIR)%s$(EXE)" % target.name
            makeFile.write("ifeq ($(N_COMPILER),__VC__)\n")
            makeFile.write("%s : %s %s\n" % (target.name, depsWin32, endTarget))
            makeFile.write("%s : %s %s\n" % (endTarget, filesWin32, resourceFiles))
            makeFile.write("\t$(LD) $(LFLAGS) $^ /OUT:$@ $(LIBDIR) $(LIBS) %s\n" % libsWin32)
            makeFile.write("else\n")
            makeFile.write("ifeq ($(N_PLATFORM),__MACOSX__)\n")
            makeFile.write("%s : %s %s\n" % (target.name, depsMacOSX, endTarget))
            makeFile.write("%s : %s\n" % (endTarget, filesMacOSX))
            makeFile.write("\t$(CXX) $^ -o $@ $(LIBDIR) $(LIBS) %s -flat_namespace\n" % libsMacOSX)
            makeFile.write("else\n")
            makeFile.write("%s : %s %s\n" % (target.name, depsLinux, endTarget))
            makeFile.write("%s : %s\n" % (endTarget, filesLinux))

            # write depsLinux, which appears to be a list of nebula libs
            # as libraries to be linked. write it twice, for circular deps.
            # we could use a topological sort instead, but this is easier.
            depsAsLibs = ''.join(["$(LIB_OPT)" + entry + "$(LIB_OPT_POST) "
                                  for entry in depsLinux.split()])
            makeFile.write("\t$(CXX) $^ -o $@ $(LIBDIR) $(LIBS) %s %s %s\n" %\
                           (libsLinux, depsAsLibs, depsAsLibs))
            makeFile.write("endif\n")
            makeFile.write("endif\n")
        elif targetType == "lib":
            # our static lib; previously known as a "package"
            # TODO: need to fix the MACOSX options...

            endTarget = "$(N_TARGETDIR)$(LIB_PRE)%s$(LIB_POST)" % target.name
            makeFile.write("ifeq ($(N_COMPILER),__VC__)\n")
            makeFile.write("%s : %s %s\n" % (target.name, depsWin32, endTarget))
            makeFile.write("%s : %s %s\n" % (endTarget, filesWin32, resourceFiles))
            makeFile.write("\t$(AR) $(LFLAGS) $^ /OUT:$@ $(LIBDIR) $(LIBS) %s\n" % libsWin32)
            makeFile.write("else\n")
            makeFile.write("ifeq ($(N_PLATFORM),__MACOSX__)\n")
            makeFile.write("%s : %s %s\n" % (target.name, depsMacOSX, endTarget))
            makeFile.write("%s : %s\n" % (endTarget, filesMacOSX))
            makeFile.write("\t$(CXX) -dynamiclib -compatibility_version 0.1 -current_version 0.1.0 $^ -o $@ $(CFLAGS) $(LIBDIR) $(LIBS) %s\n" % libsMacOSX)
            makeFile.write("%s : packages/%s_bundle$(OBJ)\n" % (endTarget, target.name))
            makeFile.write("\t$(CXX) $^ -o $@ $(CFLAGS) $(LIBDIR) $(LIBS) $(LIB_OPT)%s %s -bundle -flat_namespace\n" % (target.name, libsMacOSX))
            makeFile.write("else\n")
            makeFile.write("%s : %s %s\n" % (target.name, depsLinux, endTarget))
            makeFile.write("%s : %s\n" % (endTarget, filesLinux))
            makeFile.write("\t$(AR) -cr $@ $^\n")
            makeFile.write("endif\n")
            makeFile.write("endif\n")

        # Put the end on if we are platform specific
        if needsEnd:
            makeFile.write("endif\n")

    #--------------------------------------------------------------------------
    # Writes each workspace makefile
    def generateWorkspace(self, workspace):

        makeFile = file("%s.mak" % os.path.join(self.buildSys.homeDir, self.workspacePath,
                                workspace.name), 'w')

        # Generate all possible lists of targets
        targetsWin32  = ""
        targetsLinux  = ""
        targetsMacOSX = ""
        modulesList = []
        for targetName in workspace.targets:
            if not self.buildSys.targets[targetName].SupportsPlatform("all"):
                if self.buildSys.targets[targetName].SupportsPlatform("win32"):
                    targetsWin32  += " %s" % targetName
                elif self.buildSys.targets[targetName].SupportsPlatform("linux"):
                    targetsLinux  += " %s" % targetName
                elif self.buildSys.targets[targetName].SupportsPlatform("macosx"):
                    targetsMacOSX += " %s" % targetName
                else:
                    print "Target %s not found on any regular platform! PANIC!!!" % targetName
            else:
                targetsWin32  += " %s" % targetName
                targetsLinux  += " %s" % targetName
                targetsMacOSX += " %s" % targetName

             # Add this target's modules to a temp list
            for moduleName in self.buildSys.targets[targetName].modules:
                 module = self.buildSys.modules[moduleName]
                 modulesList.append(module)

        # Header...
        makeFile.write("#--------------------------------------------------------------------\n")
        makeFile.write("#    %s.mak\n" % workspace.name)
        makeFile.write("#    AUTOMATICALLY GENERATED, DO NOT EDIT\n")
        makeFile.write("#--------------------------------------------------------------------\n")
        makeFile.write("include ../../buildsys3/config.mak\n")
        makeFile.write("BASECFLAGS += $(EXTRA_CFLAGS)\n")
        makeFile.write("LIBDIR += $(EXTRA_LIBDIR)\n\n")
        makeFile.write("ifeq ($(N_PLATFORM),__WIN32__)\n")
        makeFile.write("%s: %s\n" % (workspace.name, targetsWin32))
        makeFile.write("endif\n")
        makeFile.write("ifeq ($(N_PLATFORM),__LINUX__)\n")
        makeFile.write("%s: %s\n" % (workspace.name, targetsLinux))
        makeFile.write("endif\n")
        makeFile.write("ifeq ($(N_PLATFORM),__MACOSX__)\n")
        makeFile.write("%s: %s\n" % (workspace.name, targetsMacOSX))
        makeFile.write("endif\n\n")

        # Modules...
        makeFile.write("# Modules\n")
        for module in modulesList:
            self.writeModule(module, makeFile)

        # Targets...
        for targetName in workspace.targets:
            makeFile.write("# Targets\n\n")
            self.writeTarget(self.buildSys.targets[targetName], makeFile)

        makeFile.write("\n# Errata\n")
        # All base targets need to be .PHONY
        makeFile.write(".PHONY: ")
        for targetName in workspace.targets:
            makeFile.write("%s " % targetName)
        makeFile.write("\n\n")

        makeFile.close()

#--------------------------------------------------------------------------
# EOF
#--------------------------------------------------------------------------
