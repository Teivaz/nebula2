#--------------------------------------------------------------------------
# Nebula 2 Build System Configuration File (Version 1)
#
# The format is described below.
#
# buildSysCfg['version'] (optional)
#     An integer number indicating the version of the configuration file, this
#     may be used in the future to auto-convert old configuration files. If
#     the version number isn't supplied it is assumed the configuration file
#     can be used by the build system without conversion.
#
# buildSysCfg['searchPaths'] (required)
#     An array of glob compatible search paths the build system can use to
#     look for bld files.
#--------------------------------------------------------------------------

buildSysCfg['version'] = 1
buildSysCfg['searchPaths'] = ['code/*/bldfiles/*.bld',
                              'code/*/bldfiles/*/*.bld',
                              'code/contrib/*/bldfiles/*.bld',
                              'code/contrib/*/bldfiles/*/*.bld']
                        
#--------------------------------------------------------------------------
# EOF
#--------------------------------------------------------------------------
