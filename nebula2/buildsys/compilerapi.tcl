#============================================================================
#  Compiler Generator Helper API
#
#  Compiler generator authors should only use the data exposed by these
#  functions to prevent the generators from being broken when and if the
#  back end data structures change in the future.
#
#  If data you need isn't exposed here don't go after the data directly -
#  write a routine and add it here.  The only coding convention is to try to
#  keep all data returns within TCL's 'foreach' syntax to make everyone's
#  life easy.
#
#   Cobbled up from the original buildsys courtesy of Radon Labs
#   Copyright (c) 2003 Radon Labs
#
#   This file is licenesed under the terms of the Nebula License
#============================================================================

#----------------------------------------------------------------------------
#  procs:
#    get_workspaces
#
#    set_currentworkspace   $workspace_name
#    set_workspacepath      $defaultpath
#    set_outputpath         $defaultpath
#    set_interpath          $defaultpath
#    use_workspace          $name $wspacedir $outputdir $interdir
#
#    get_targets
#    get_tardeps            $tarname
#    get_tarmods            $tarname
#    get_tartype            $tarname
#    test_tarplatform       $tarname $platform
#    get_tarplatform        $tarname
#    get_tardefs            $tarname
#    get_rtti               $tarname
#    get_exceptions         $tarname
#    get_icon               $tarname
#    get_win32resource      $tarname
#    get_moddeffile         $tarname
#    get_dllextensition     $tarname
#    get_win32libs_release  $tarname
#    get_win32libs_debug    $tarname
#    get_linuxlibs          $tarname
#    get_osxlibs            $tarname
#
#    test_modplatform       $tarname $platform
#    get_modplatform        $tarname
#    get_modheaders         $modname
#    get_modsources         $modname
#    get_modheaders_dressed $modname - path, no ext
#    get_modsources_dressed $modname - path, no ext
#    get_modtype            $modname
#    get_modpath            $modname [inc|src]
#
#    path_srctoinc          $modname
#    path_wspacetosrc       $modname
#    path_wspacetooutput
#    path_wspacetointer
#
#    get_incsearchdirs
#    get_libsearchdirs
#    write_resfiles
#    write_pkgfiles
#    pathto                 $pathfile
#
#----------------------------------------------------------------------------

global cur_workspace
global cur_workpacepath

#----------------------------------------------------------------------------
#  get_workspaces
#
#  Retrieves a list of all valid workspace names.  Only proc that may be
#  called before setting the current workspace.
#----------------------------------------------------------------------------
proc get_workspaces { wslist } {
    global wspace
    global num_wspaces

    set wlist ""

    for {set i 0} {$i < $num_wspaces} {incr i} {
        if {$wslist != ""} {
            if {[lsearch $wslist $wspace($i,name)] == -1} {
                continue
            }
        }
        addtolist wlist $wspace($i,name)
    }
    return $wlist
}

#----------------------------------------------------------------------------
#  set_current_workspace
#
#  Sets the currently active workspace by name.
#
#  CAVEAT: It is mandatory to do this before calling any of the other procs
#          in this file for each given workspace (or use_workspace) except
#          get_workspaces.
#----------------------------------------------------------------------------
proc set_currentworkspace { workspace_name } {
    global cur_workspace

    set cur_workspace [findwspacebyname $workspace_name]
}

#----------------------------------------------------------------------------
#  set_workspacepath
#
#  As each generator can have its own default this makes everything live
#  happy together.
#
#  CAVEAT: It is mandatory to call this to make sure proper default paths
#          and overrides are handled correctly and the build files are
#          written at the correct location. (or use_workspace)
#  CAVEAT: Calling this function will create the required directories
#          (if missing) and write out the pkg files for the targets
#          within the workspace.
#
#          $outputpath/
#          $outputpath/pkg/
#----------------------------------------------------------------------------
proc set_workspacepath {path} {
    global cur_workspacepath
    global cur_workspace
    global wspace
    global home

    if {$wspace($cur_workspace,dir) == "" } {
        set cur_workspacepath $path
    } else {
        set cur_workspacepath $wspace($cur_workspace,dir)
    }

    set dir [findrelpath $home $cur_workspacepath]
    check_makedir $dir
}

#----------------------------------------------------------------------------
#  set_outputpath
#
#  As each generator can have its own default this makes everything live
#  happy together.
#
#  CAVEAT: It is mandatory to call this to make sure proper default paths
#          and overrides are handled correctly and the build files are
#          written at the correct location.  (or use_workspace)
#  CAVEAT: Calling this function will create the required directories
#          if they are missing.
#
#          $path/win32
#          $path/win32d
#          $path/win32p
#          $path/linux
#          $path/linuxd
#          $path/linuxp
#          $path/macosx
#          $path/macosxd
#          $path/macosxp
#----------------------------------------------------------------------------
proc set_outputpath {path} {
    global cur_outputpath
    global cur_workspace
    global wspace
    global home
    global platform

    if {$wspace($cur_workspace,binarydir) == "" } {
        set cur_outputpath $path
    } else {
        set cur_outputpath $wspace($cur_workspace,binarydir)
    }

    set dir [findrelpath $home $cur_outputpath]
    if {$platform == "win32"} {
        check_makedir $dir/win32
        check_makedir $dir/win32d
    }
    if {$platform == "linux"} {
        check_makedir $dir/linux
        check_makedir $dir/linuxd
    }
    if {$platform == "macosx"} {
        check_makedir $dir/macosx
        check_makedir $dir/macosxd
    }
}

#----------------------------------------------------------------------------
#  set_interpath
#
#  As each generator can have its own default this makes everything live
#  happy together.
#
#  CAVEAT: It is mandatory to call this to make sure proper default paths
#          and overrides are handled correctly and the build files are
#          written at the correct location
#  CAVEAT: Calling this function will create the required directories
#          if they are missing.
#
#          $path/win32
#          $path/win32d
#          $path/win32p
#          $path/linux
#          $path/linuxd
#          $path/linuxp
#          $path/macosx
#          $path/macosxd
#          $path/macosxp
#----------------------------------------------------------------------------
proc set_interpath {path} {
    global cur_interpath
    global cur_workspace
    global wspace
    global home
    global platform

    #if {$wspace($cur_workspace,libdir) == "" } {
    #    set cur_interpath $path
    #} else {
    #    set cur_interpath $wspace($cur_workspace,libdir)
    #}
    set cur_interpath $path

    set dir [findrelpath $home $cur_interpath]
    if {$platform == "win32"} {
        check_makedir $dir/win32
        check_makedir $dir/win32d
    }
    if {$platform == "linux"} {
        check_makedir $dir/linux
        check_makedir $dir/linuxd
    }
    if {$platform == "macosx"} {
        check_makedir $dir/macosx
        check_makedir $dir/macosxd
    }
}

#----------------------------------------------------------------------------
#  use_workspace
#
#  Short hand version of the above set_??? procs.
#
#  CAVEAT:  If not using the above set_??? procs then this must be used
#           when creating workspace build files.  See the above files for
#           more information.
#----------------------------------------------------------------------------
proc use_workspace {wspace_name wspacedir outputdir interdir} {
    set_currentworkspace $wspace_name
    set_workspacepath $wspacedir
    set_outputpath $outputdir
    set_interpath  $interdir
}



#----------------------------------------------------------------------------
#  get_targets
#
#  Returns a list of target names to build for the current workspace
#----------------------------------------------------------------------------
proc get_targets { } {
    global wspace
    global cur_workspace

    return $wspace($cur_workspace,targets)
}


#----------------------------------------------------------------------------
#  get_tardeps
#
#  Returns a list of target names that the given target requries to be built
#  first (build order dependency)
#----------------------------------------------------------------------------
proc get_tardeps {tarname} {
    global wspace
    global cur_workspace

    return $wspace($cur_workspace,$tarname,targetdeps)
}

#----------------------------------------------------------------------------
#  get_tarmods
#
#  Returns a list of module names that the given target uses
#----------------------------------------------------------------------------
proc get_tarmods {tarname} {
    global wspace
    global cur_workspace

    return $wspace($cur_workspace,$tarname,modules)
}

#----------------------------------------------------------------------------
#  test_tarplatform
#
#  Returns true|false if the target is valid for this platform
#----------------------------------------------------------------------------
proc test_tarplatform {tarname platform_test} {
    global tar

    set idx [findtargetbyname $tarname]
    return [test_platform $tar($idx,platform) $platform_test]
}

#----------------------------------------------------------------------------
#  get_tarplatform
#
#  returns the unadulterated tar platform list (this is generated from
#  the modules this target depends on)
#----------------------------------------------------------------------------
proc get_tarplatform {tarname} {
    global wspace
    global cur_workspace

    return $wspace($cur_workspace,$tarname,platform)
}

#----------------------------------------------------------------------------
#  get_tartype
#
#  Returns the target type (lib|exe|dll)
#----------------------------------------------------------------------------
proc get_tartype {tarname} {
    global wspace
    global cur_workspace

    return $wspace($cur_workspace,$tarname,type)
}

#----------------------------------------------------------------------------
#  get_tardefs
#
#  Returns a list of all the optional preprocessor symbols that should be
#  active for this target as a list of lists.
#
#  eg. { {$name $value} {$name $value} }
#----------------------------------------------------------------------------
proc get_tardefs {tarname} {
    global wspace
    global cur_workspace

    return $wspace($cur_workspace,$tarname,defs)
}

#----------------------------------------------------------------------------
#  get_rtti
#
#  Returns true iff Runtime Type Identification should be activated for this
#  target
#
#  In .bld file, write "setrtti true"
#----------------------------------------------------------------------------
proc get_rtti {tarname} {
    global wspace
    global cur_workspace
    return $wspace($cur_workspace,$tarname,rtti)
}

#----------------------------------------------------------------------------
#  get_exceptions
#
#  Returns true iff c++ exceptions should be supported for this target
#
#  In .bld file, write "setexceptions true"
#----------------------------------------------------------------------------
proc get_exceptions {tarname} {
    global wspace
    global cur_workspace
    return $wspace($cur_workspace,$tarname,exceptions)
}

#----------------------------------------------------------------------------
#  get_icon
#
#  Returns the filename of the target's icon
#
#  In .bld file, write "seticon $filename"
#----------------------------------------------------------------------------
proc get_icon {tarname} {
    global tar

    set idx [findtargetbyname $tarname]
    return $tar($idx,icon)
}

#----------------------------------------------------------------------------
#  get_win32resource
#
#  Returns the filename of the target's resource
#
#  In .bld file, write "setresource_win32 $filename"
#----------------------------------------------------------------------------
proc get_win32resource {tarname} {
    global wspace
    global cur_workspace
    return $wspace($cur_workspace,$tarname,resource_win32)
}

#----------------------------------------------------------------------------
# get_moddeffile
# Returns the file and path to the module definition file
# Returns "" if not set
#----------------------------------------------------------------------------
proc get_moddeffile {tarname}  {
    global wspace
    global cur_workspace

    return $wspace($cur_workspace,$tarname,moddeffile)
}

#----------------------------------------------------------------------------
#  get_dllextension
#
#  Returns the extension for the dll, which can be changed for plugin development.
#----------------------------------------------------------------------------
proc get_dllextension {tarname} {
    global wspace
    global cur_workspace

    return $wspace($cur_workspace,$tarname,dllextension)
}

#----------------------------------------------------------------------------
#  get_win32libs_relase
#
#  Returns a full list of libnames to link in a win32 release build.  These
#  names are already dressed with the extension .lib
#----------------------------------------------------------------------------
proc get_win32libs_release {tarname} {
    global wspace
    global cur_workspace

    return $wspace($cur_workspace,$tarname,libs_win32_release)
}


#----------------------------------------------------------------------------
#  get_win32libs_debug
#
#  Returns a full list of lib names to link in a win32 debug build.  These
#  names are already dressed with the extension .lib
#----------------------------------------------------------------------------
proc get_win32libs_debug {tarname} {
    global wspace
    global cur_workspace

    return $wspace($cur_workspace,$tarname,libs_win32_debug)
}

#----------------------------------------------------------------------------
#  get_linuxlibs
#
#  Returns a full list of lib names to link in a linux build.  These are
#  not dressed for .a .o etc. and the extension will have to be provided
#  by the compiler generator
#----------------------------------------------------------------------------
proc get_linuxlibs {tarname} {
    global wspace
    global cur_workspace

    return $wspace($cur_workspace,$tarname,libs_linux)
}

#----------------------------------------------------------------------------
#  get_osxlibs
#
#  Returns a full list of lib names to link in a linux build.  These are
#  not dressed for .a .o etc. and the extension will have to be provided
#  by the compiler generator
#----------------------------------------------------------------------------
proc get_osxlibs {tarname} {
    global wspace
    global cur_workspace

    return $wspace($cur_workspace,$tarname,libs_macosx)
}

#----------------------------------------------------------------------------
#  get_modheaders
#
#  Returns a list of undressed header file names (no path or ext)
#----------------------------------------------------------------------------
proc get_modheaders {modname} {
    global mod

    set i [findmodbyname $modname]

    return $mod($i,headers)
}


#----------------------------------------------------------------------------
#  get_modsources
#
#  Returns  undressed source file names (no path or ext)
#----------------------------------------------------------------------------
proc get_modsources {modname} {
    global mod

    set i [findmodbyname $modname]

    return $mod($i,files)
}

#----------------------------------------------------------------------------
#  get_modheaders_dressed
#
#  Returns a list of dressed header file names (path, no ext) for the given
#  module.
#----------------------------------------------------------------------------
proc get_modheaders_dressed {modname} {
    global mod

    set i [findmodbyname $modname]

    return $mod($i,hdrs)
}


#----------------------------------------------------------------------------
#  get_modsources_dressed
#
#  Returns a list of undressed source file names (path, no ext) for the
#  given module.
#----------------------------------------------------------------------------
proc get_modsources_dressed {modname} {
    global mod

    set i [findmodbyname $modname]

    return $mod($i,srcs)
}

#----------------------------------------------------------------------------
#  get_modtype
#
#  Returns c|cpp representing the language convention this module should
#  be compiled under.
#----------------------------------------------------------------------------
proc get_modtype {modname} {
    global mod

    set i [findmodbyname $modname]

    return $mod($i,type)
}

#----------------------------------------------------------------------------
#  get_modpath $modname inc|src|lib
#
#  Returns the relative path from $home to the inc, src or lib dir root for the
#  given module. eg. for nkernel = ./code/nebula2/inc
#----------------------------------------------------------------------------
proc get_modpath {modname type} {
    global mod

    set i [findmodbyname $modname]
    set path_part ./code/$mod($i,trunkdir)
    set src_part "src"
    if {$type == "inc"} {
        set src_part "inc"
    }
    if {$type == "lib"} {
        set src_part "lib"
    }
    return [cleanpath $path_part/$src_part]
}

#----------------------------------------------------------------------------
#  test_modplatform
#
#  Returns true|false if the target is valid for this platform
#----------------------------------------------------------------------------
proc test_modplatform {modname platform_test} {
    global mod

    return [test_platform $mod([findmodbyname $modname],platform) $platform_test]
}

#----------------------------------------------------------------------------
#  get_modplatform
#
#  returns the unadulterated mod platform list
#----------------------------------------------------------------------------
proc get_modplatform {modname} {
    global mod

    return $mod([findmodbyname $modname],platform)
}


#----------------------------------------------------------------------------
#  path_srctoinc modname
#
#  Returns the path from the src dir to the inc dir for the given module.
#----------------------------------------------------------------------------
proc path_srctoinc {modname} {
    set pfrom [get_modpath $modname "src"]
    set pto   [get_modpath $modname "inc"]

    return [findrelpath $pfrom $pto]
}

#----------------------------------------------------------------------------
#  path_wspacetosrc modname
#
#  Returns the path from the wspace directory to the src dir for the
#  given module
#----------------------------------------------------------------------------
proc path_wspacetosrc {modname} {
    global cur_workspacepath

    set pto [getmodpath $modname "src"]
    return [findrelpath $cur_workspacepath $pto]
}

#----------------------------------------------------------------------------
#  path_wspacetooutput
#
#  Returns the path from the wspace directory to the binary binarydir dir
#----------------------------------------------------------------------------
proc path_wspacetooutput {} {
    global cur_workspacepath
    global cur_outputpath

    return [findrelpath $cur_workspacepath $cur_outputpath]
}

#----------------------------------------------------------------------------
#  path_wspacetointer
#
#  Returns the path from the wspace to the intermediate compilation dir.
#----------------------------------------------------------------------------
proc path_wspacetointer {} {
    global cur_workspacepath
    global cur_interpath

    return [findrelpath $cur_workspacepath $cur_interpath]
}

#----------------------------------------------------------------------------
#  get_incsearchdirs
#
#  Simply returns a list of all valid inc dir paths as a relative from the
#  current workspace path generated from a list of all modules used within
#  the workspace
#----------------------------------------------------------------------------
proc get_incsearchdirs { } {
    global cur_workspacepath

    set clean_cur_workspacepath [cleanpath $cur_workspacepath]
    set retlist {}
    foreach target [get_targets] {
        foreach module [get_tarmods $target] {
            # get_modpath returns a clean path...
            lappend retlist [findrelpath_clean $clean_cur_workspacepath [get_modpath $module inc]]
        }
    }
    return [lsort -unique $retlist]
}


#----------------------------------------------------------------------------
#  get_libsearchdirs
#
#  Simply returns a list of all valid lib dir paths as a relative from the
#  current workspace path generated from a list of all modules used within
#  the workspace
#----------------------------------------------------------------------------
proc get_libsearchdirs { } {
    global cur_workspacepath

    set clean_cur_workspacepath [cleanpath $cur_workspacepath]
    set retlist {}
    foreach target [get_targets] {
        foreach module [get_tarmods $target] {
            # get_modpath returns a clean path...
            lappend retlist [findrelpath_clean $clean_cur_workspacepath [get_modpath $module lib]]
        }
    }
    return [lsort -unique $retlist]
}


#----------------------------------------------------------------------------
#  write_resfiles
#
#  writes out res_XXX.rc files for each executable target that point to
#  the target's icon in /bin/win32.
#
#  TODO: Find a better way of handling app icons. - includes linux and macosx
#----------------------------------------------------------------------------
proc write_resfiles { } {
    global home
    global tar
    global num_tars

    set pdir [cleanpath ./build/pkg]
    set relpath [findrelpath $pdir ./bin/win32]
    set dir  [cleanpath $home/$pdir]

    for {set i 0} {$i < $num_tars} {incr i} {
        if {$tar($i,type) != "exe"} {
            continue
        }
        set target $tar($i,name)
        # if {![test_tarplatform $target "win32"]} {
        #     continue
        # }
        set cid [open $dir/res_$target.rc w]
        puts $cid "nebula_icon ICON \"$relpath/[get_icon $target]\""
        close $cid
    }
}

#----------------------------------------------------------------------------
#  write_pkgfiles
#
#  writes out pkg_XXX.cc files for the lib targets
#----------------------------------------------------------------------------
proc write_pkgfiles { } {
    global home
    global tar
    global num_tars
    global mod
    global num_mods
    global modsbyname

    set pkgdir "./build/pkg"
    set pdir [cleanpath $pkgdir]
    set dir  [cleanpath $home/$pdir]

    # Now, do the pkg files for each lib target
    for {set i 0} {$i < $num_tars} {incr i} {
        set target $tar($i,name)

        # collect the targets
        set mod_list ""
        foreach module $tar($i,mergedmods) {
            if {$mod([findmodbyname $module],forcenopkg)} {
                continue
            }
            addtolist mod_list $module
        }

        set mod_list [sort_mods $mod_list]

        if {[llength $mod_list] == 0} {
            continue
        }

        set cid [open $dir/pkg_$target.cc w]

        puts $cid "//----------------------------------------------------------"
        puts $cid "// pkg_$target.cc"
        puts $cid "// MACHINE GENERATED, DON'T EDIT!"
        puts $cid "//----------------------------------------------------------"
        puts $cid "#include \"kernel/ntypes.h\""
        puts $cid "#include \"kernel/nkernelserver.h\""
        puts $cid "#ifdef __XBxX__"
        puts $cid "#undef __WIN32__"
        puts $cid "#endif"
        puts $cid ""
        puts $cid "extern \"C\" void $target\();"
        puts $cid ""

        foreach module $mod_list {
            #write out the extern lines
            set ifdefd false
            set plat_list ""
            if {![test_modplatform $module all]} {
                set ifdefd true
                set plat_list [translate_platdefs [get_modplatform $module]]
                set deflist [join $plat_list " || "]
                puts $cid "#ifdef $deflist"
            }
            puts $cid "extern bool n_init_$module (nClass *, nKernelServer *);"
            puts $cid "extern void *n_new_$module (void);"

            if {$ifdefd} {
                puts $cid "#endif"
            }
            puts $cid ""
        }

        puts $cid ""
        puts $cid "void $target\()"
        puts $cid "\{"

        foreach module $mod_list {
            #write out the addmodules lines
            set ifdefd false
            set plat_list ""
            if {![test_modplatform $module all]} {
                set ifdefd true
                set plat_list [translate_platdefs [get_modplatform $module]]
                set deflist [join $plat_list " || "]
                puts $cid "#ifdef $deflist"
            }
            set initproc "n_init_$module"
            set newproc  "n_new_$module"
            puts $cid "    nKernelServer::ks->AddModule(\"$module\","
            puts $cid "                                 $initproc,"
            puts $cid "                                 $newproc);"

            if {$ifdefd} {
                puts $cid "#endif"
            }
        }

        puts $cid "\}"
        puts $cid ""
        puts $cid "//----------------------------------------------------------"
        puts $cid "// EOF"
        puts $cid "//----------------------------------------------------------"

        close $cid

        #create a mod entry for it - needed
        set mod($num_mods,name)                 pkg_$target
        set mod($num_mods,dir)                  $dir
        set mod($num_mods,annotate)             ""
        set mod($num_mods,platform)             all
        set mod($num_mods,type)                 cpp
        set mod($num_mods,files)                pkg_$target
        set mod($num_mods,headers)              ""
        set mod($num_mods,moddeffile)           ""
        set mod($num_mods,libs_win32)           ""
        set mod($num_mods,libs_win32_release)   ""
        set mod($num_mods,libs_win32_debug)     ""
        set mod($num_mods,libs_linux)           ""
        set mod($num_mods,libs_macosx)          ""
        set mod($num_mods,moduledeps)           ""
        set mod($num_mods,forcenopkg)           true
        set mod($num_mods,trunkdir)             ""
        set mod($num_mods,bldfile)              "** autogenerated **"
        set mod($num_mods,hdrs)                 ""
        set mod($num_mods,srcs)                 $pkgdir/pkg_$target
        set mod($num_mods,autonopak)            true
        set mod($num_mods,ancestor)             ""
        set modsbyname(pkg_$target) $num_mods
        incr num_mods

        # And add it to the target so that it gets built
        lappend tar($i,mergedmods) pkg_$target
    }
}

#----------------------------------------------------------------------------
#  pathto pathfile
#  Just so the globals don't have to be counted on.
#----------------------------------------------------------------------------
proc pathto {pathfile} {
    global cur_workspacepath

    return [findrelpath $cur_workspacepath $pathfile]
}

#----------------------------------------------------------------------------
# get_generator_info
# List info about the loaded generator scripts
#----------------------------------------------------------------------------
proc get_generator_info {} {
    global loaded_generators
    set results {}
    foreach gen $loaded_generators {
        if {[catch { set desc [namespace inscope $gen description] } result]} {
            set desc "  ERROR: $result"
        } else {
            lappend results [list $gen $desc]
        }
    }
    return $results
}
#----------------------------------------------------------------------------
# EOF
#----------------------------------------------------------------------------
