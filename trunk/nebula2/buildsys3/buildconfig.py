#--------------------------------------------------------------------------
# 3rd Generation Nebula 2 Build System
#
# (c) 2005 Vadim Macagon
#
# Contents are licensed under the Nebula license.
#--------------------------------------------------------------------------

import os.path, string, glob, traceback

class BuildConfig:
    # The latest version of the build config file format
    CUR_VERSION = 1
        
    #--------------------------------------------------------------------------
    def __init__(self):
        self.bldFiles = []
            
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
                cfg = cfgEnv.get('buildSysCfg', None)
                if cfg != None:
                    fileVersion = cfg.get('version', BuildConfig.CUR_VERSION)
                    if fileVersion != BuildConfig.CUR_VERSION:
                        return False
                    searchPaths = cfg.get('searchPaths', [])
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
        pass # not needed at present
        
#--------------------------------------------------------------------------
# EOF
#--------------------------------------------------------------------------
