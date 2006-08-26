#--------------------------------------------------------------------------
# 3rd Generation Nebula 2 Build System
#
# (c) 2005 Vadim Macagon
#
# Contents are licensed under the Nebula license.
#--------------------------------------------------------------------------

import os.path, string, glob, traceback

STR_CFG_HEADER = """\
#--------------------------------------------------------------------------
# Nebula 2 Build System Configuration File (Version 1)
#
# This file is read in by Python, watch your syntax and formatting!
#--------------------------------------------------------------------------
"""

STR_CFG_VERSION = """
# OPTIONAL SETTING
# An integer number indicating the version of the configuration file, this
# may be used in the future to auto-convert old configuration files. If
# the version number isn't supplied it is assumed the configuration file
# can be used by the build system without conversion.
buildSysCfg['version'] = %d
"""

STR_CFG_BLD_SEARCH_PATHS = """
# REQUIRED SETTING
# An array of glob compatible search paths the build system can use to
# look for bld files.
buildSysCfg['searchPaths'] = """

STR_CFG_DOXYGEN_SETTINGS_START = """

# DOXYGEN GENERATOR settings follow.
buildSysCfg['doxygen'] = {}
"""

STR_CFG_AUTORUN_DOXYGEN = """
# OPTIONAL SETTING
# Set to True if you want the Doxygen Generator to run doxygen automatically
# to generate documentation. It's True by default.
buildSysCfg['doxygen']['autoRunDoxygen'] = %r
"""

STR_CFG_DOXYGEN_DIR = """
# OPTIONAL SETTING
# Specify the directory where the doxygen binary resides, if this setting is
# not supplied the Doxygen Generator will try to use whatever the OS can find
# (i.e. if you have the doxygen directory in your PATH it will use that).
buildSysCfg['doxygen']['doxygenDir'] = %r
"""

STR_CFG_GENERATE_CHM = """
# OPTIONAL SETTING
# Set to True if you want the Doxygen Generator to generate a single CHM file
# with all the documentation. CHMs can only be generated on Windows if you
# have the Microsoft HTML Help Workshop installed.
buildSysCfg['doxygen']['autoGenerateCHM'] = %r
"""

STR_CFG_HTML_HELP_WORKSHOP_DIR = """
# OPTIONAL SETTING
# Specify the directory where the HTML Help Compiler (hhc.exe) that comes
# with the Microsoft HTML Help Workshop resides, if this setting is
# not supplied the Doxygen Generator will try to use whatever the OS can find
# (i.e. if you have the HTML Help Workshop directory in your PATH it will use
# that).
buildSysCfg['doxygen']['htmlHelpWorkshopDir'] = %r
"""

class BuildConfig:
    # The latest version of the build config file format
    CUR_VERSION = 1

    #--------------------------------------------------------------------------
    def __init__(self):
        self.bldFiles = []
        self.cfg = None

    #--------------------------------------------------------------------------
    def Read(self, filename):
        if os.path.exists(filename):
            # setup exec environment
            cfgEnv = { 'buildSysCfg' : {} }
            try:
                # execute config
                execfile(filename, cfgEnv)
            except:
                print 'Error: Failed to execute config ' + filename
                traceback.print_exc()
            else:
                # extract config information
                self.cfg = cfgEnv.get('buildSysCfg', None)
                if self.cfg != None:
                    fileVersion = self.cfg.get('version', BuildConfig.CUR_VERSION)
                    if fileVersion != BuildConfig.CUR_VERSION:
                        return False
                    searchPaths = self.cfg.get('searchPaths', [])
                    for searchPath in searchPaths:
                        # convert path separators to match the platform
                        if string.find(searchPath, '/') != -1:
                            searchPath = string.replace(searchPath, '/', os.sep)
                        matches = glob.glob(searchPath)
                        for match in matches:
                            if os.path.isfile(match):
                                self.bldFiles.append(match)
                    return True
        return False

    #--------------------------------------------------------------------------
    def Write(self, filename):
        try:
            cfgFile = file(filename, 'w')
        except IOError:
            print 'Failed to open %s for writing.' % filename
            traceback.print_exc()
        else:
            # NOTE: instead of writing every setting one at a time with
            # comments for the user we could just do a single
            # cfgFile.write(repr(self.cfg))... but that all ends up in a
            # single line and as such isn't very easy to modify by hand :|
            cfgFile.write(STR_CFG_HEADER)
            cfgFile.write(STR_CFG_VERSION % BuildConfig.CUR_VERSION)
            # bld file search paths
            indent = ' ' * 30
            searchPaths = self.cfg.get('searchPaths', [])
            numSearchPaths = len(searchPaths)
            out = STR_CFG_BLD_SEARCH_PATHS + '['
            for i in range(numSearchPaths):
                if i > 0:
                    out += indent
                if numSearchPaths > 1:
                    if i < (numSearchPaths - 1):
                        out += "'" + searchPaths[i] + "',\n"
                    else:
                        out += "'" + searchPaths[i] + "'"
            out += ']'
            cfgFile.write(out)
            # doxygen generator settings
            doxygenSettings = self.cfg.get('doxygen', None)
            if doxygenSettings != None:
                cfgFile.write(STR_CFG_DOXYGEN_SETTINGS_START)
                setting = doxygenSettings.get('autoRunDoxygen', None)
                if setting != None:
                    cfgFile.write(STR_CFG_AUTORUN_DOXYGEN % setting)
                setting = doxygenSettings.get('doxygenDir', None)
                if setting != None:
                    cfgFile.write(STR_CFG_DOXYGEN_DIR % setting)
                setting = doxygenSettings.get('autoGenerateCHM', None)
                if setting != None:
                    cfgFile.write(STR_CFG_GENERATE_CHM % setting)
                setting = doxygenSettings.get('htmlHelpWorkshopDir', None)
                if setting != None:
                    cfgFile.write(STR_CFG_HTML_HELP_WORKSHOP_DIR % setting)
            cfgFile.close()

#--------------------------------------------------------------------------
# EOF
#--------------------------------------------------------------------------
