#
# 3rd Generation Nebula 2 Build System
#
#
# (c) 2005 Vadim Macagon
#
# Contents are licensed under the Nebula license.
#

from buildsys3.bldscanner import *
from buildsys3.buildsys import *
import os, dircache, string, time, sys
import hotshot, hotshot.stats

updateDir = os.getcwd()

#--------------------------------------------------------------------------
def PrintHelp():
    print """\
Usage:
update.py [-help] [-build generator] [workspaces]
          [-listworkspaces] [-listgenerators] [-verbose]
    
Examples:
(1) To generate Visual Studio 2003 .vcproj/.sln files for
    all available workspaces do...
    
    update.py -build vstudio71
    
(2) To generate Visual Studio 2003 files for just the Nebula2
    workspace do...
    
    update.py -build vstudio71 Nebula2
    
(3) To generate Visual Studio 2003 files for the Nebula2 and
    luaserver workspaces do...
    
    update.py -build vstudio71 Nebula2 luaserver

Details:
-help
    You're reading it.
-build
    Specify which build system generator should be used.
    If this isn't specified, then all available generators
    will be run (not recommended).
workspaces
    The list of workspaces that should be generated.
    By default, all workspaces will be generated.
-listworkspaces
    List the available workspaces.
-listgenerators
    List the available build system generators.
-verbose
    Print extra information (but not full debug
    information). This is off by default but basic
    information will still be printed.
-gui
    Launches the GUI.
"""

#--------------------------------------------------------------------------
def PrintWorkspaceList():
    buildSys = BuildSys(updateDir, 'build.cfg.py')
    if buildSys.Prepare():
        numWorkspaces = len(buildSys.workspaces)
        if numWorkspaces > 0:
            print '************************\n' \
                  '* Available Workspaces *\n' \
                  '************************'
            for workspaceName in buildSys.workspaces.keys():
                print workspaceName
            print '************************'
        else:
            print '************************\n' \
                  '* No Workspaces Found  *\n' \
                  '************************'
    else:
        # print out errors
        for err in buildSys.errors:
            print err

#--------------------------------------------------------------------------
def PrintGeneratorList():
    buildSys = BuildSys(updateDir, 'build.cfg.py')
    generators = buildSys.GetGenerators()
    if len(generators) > 0:
        print '************************\n' \
              '* Available Generators *\n' \
              '************************'
        for generatorName, generator in generators.items():
            print generatorName + '\n    ' + generator.description
        print '************************'
    else:
        print 'No Generators Found'

#--------------------------------------------------------------------------
def Build(generatorName, workspaces):
    buildSys = BuildSys(updateDir, 'build.cfg.py')
    if buildSys.HasGenerator(generatorName):
        if buildSys.Prepare():
            buildSys.logger.info('Validation Successful!')
            buildSys.Run(generatorName, workspaces)
    else:
        buildSys.logger.error('Unknown generator ' + generatorName)

#--------------------------------------------------------------------------
# To profile a build use the -build flag with desired parameters and also
# add -profile to the command line.
# e.g. python update.py -profile -build vstudio71 Nebula2
def Profile(generatorName, workspaces):
    prof = hotshot.Profile('build.prof')
    prof.runcall(Build, generatorName, workspaces)
    prof.close()
    stats = hotshot.stats.load('build.prof')
    stats.strip_dirs()
    #stats.sort_stats('cumulative')
    stats.sort_stats('time')
    stats.print_stats()

#--------------------------------------------------------------------------
def LaunchGUI():
    try:
        from buildsys3.gui import DisplayGUI
    except ImportError:
        print 'Error: Failed to import buildsys3.gui, is wxPython installed?'
    else:
        buildSys = BuildSys(updateDir, 'build.cfg.py')
        DisplayGUI(buildSys)

#--------------------------------------------------------------------------
def main():
    generatorName = ''
    doProfile = False
    workspaceList = []
    numCmdLineArgs = len(sys.argv)
    if numCmdLineArgs > 1:
        i = 1
        while i < numCmdLineArgs:
            if '-help' == sys.argv[i]:
                PrintHelp()
                return
            elif '-gui' == sys.argv[i]:
                LaunchGUI()
                return
            elif '-build' == sys.argv[i]:
                if (i + 1) < numCmdLineArgs:
                    i += 1
                    generatorName = sys.argv[i]
                else:
                    print 'Error: No generator name given.'
                    return
            elif '-profile' == sys.argv[i]:
                doProfile = True
            elif '-listworkspaces' == sys.argv[i]:
                PrintWorkspaceList()
                return
            elif '-listgenerators' == sys.argv[i]:
                PrintGeneratorList()
                return
            elif '-verbose' == sys.argv[i]:
                pass
            else:
                workspaceList.append(sys.argv[i])
            i += 1
    else:
        # build everything for everything? naaah :)
        print 'Clueless? Use update.py -help'
        return

    if doProfile:
        Profile(generatorName, workspaceList)
    else:
        Build(generatorName, workspaceList)
            


# Entry Point    
if __name__ == '__main__':
    main()

#--------------------------------------------------------------------------
# EOF
#--------------------------------------------------------------------------
