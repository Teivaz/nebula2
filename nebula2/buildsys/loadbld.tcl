#----------------------------------------------------------------------------
#   loadbld.tcl
#
#   This file contains the data entry functions used by the .bld files
#
#   Cobbled up from the original buildsys courtesy of Radon Labs
#   Copyright (c) 2003 Radon Labs
#
#   This file is licenesed under the terms of the Nebula License
#----------------------------------------------------------------------------
global wspace
global tar
global mod
global bundle

global num_wspaces
global num_tars
global num_mods
global num_bundles

set num_wspaces 0
set num_tars    0
set num_mods    0
set num_bundles 0

#used to resolve naming collisions to make life nicer for .bld authors
global current_block
global noactive_block
global wspace_block
global target_block
global module_block
global bundle_block
#defines for the numbers for the blocks
set noactive_block 0
set wspace_block 1
set target_block 2
set module_block 3
set bundle_block 4

global current_file

set current_block 0
set current_file  ""

#============================================================================
# procs:
#
#    beginworkspace        $name
#    annotate              $annotation
#    setdir                $dir
#    setbinarydir          $dir
#    setlibdir             $dir
#    settargets            { $targetname $targetname ... }
#    addglobaldef          $defname $value
#    addtargetdef          $targetname $defname $value
#    endworkspace
#
#    begintarget           $name
#    annotate              $annotation
#    settype               lib|exe|dll
#    changedllextension    $ext
#    setrtti               true|false
#    setexceptions         true|false
#    seticon               $filename
#    setmodules            { $modulename $modulename ... }
#    setbundles            { $bundlename $bundlename ... }
#    settargetdeps         { $targetname $targetname ... }
#    endtarget
#
#    beginbundle           $name
#    annotate              $annotation
#    setmodules            { $modulename $modulename ... }
#    settargets            { $targetname $targetname ... }
#    endbundle
#
#    beginmodule           $name
#    annotate              $annotation
#    setdir                $dir
#    setplatform           all, win32, linux, macosx
#    setmodtype            c|cpp
#    setmoddeffile         $filename
#    setfiles              { $filename $filename ... }
#    setheaders            { $headername $headername ... }
#    setlibs_win32         { $libname.lib $libname.lib ... }
#    setlibs_win32_release { $libname.lib $libname.lib ... }
#    setlibs_win32_debug   { $libname.lib $libname.lib ... }
#    setlibs_linux         { $libname $libname ... }
#    setlibs_macosx        { $libname $libname ... }
#    setmoduledeps         { $modulename $modulename ... }
#    setnopkg              true | false
#    endmodule
#
#    loadbldfiles
#    getpakdir             $dir
#    recursepakdir         $dir
#
#    checknoblock
#
#============================================================================

#============================================================================
#  Pak file searching
#============================================================================

#----------------------------------------------------------------------------
#  loadbldfiles
#  Simply finds all relevant bld files laying out there and
#  'source's them - This is the generic entry point for this file.
#
#  nebula2/code/*/src
#  nebula2/code/*/bldfiles
#  nebula2/code/*/bldfiles/*
#  nebula2/code/contrib/*/src
#  nebula2/code/contrib/*/bldfiles
#  nebula2/code/contrib/*/bldfiles/*
#----------------------------------------------------------------------------
proc loadbldfiles { } {
    global home

    ::log::log info "\n****Loading bld files"
    ::log::log debug "home = $home"
    set startdir "[string trim $home /]/code/"
    set contribdir "[string trim $home /]/code/contrib/"

    ::log::log debug "startdir = $startdir"
    ::log::log debug "contribdir = $contribdir"

    foreach ext [glob -nocomplain -directory $startdir */] {
        set ext [string trim $ext '/']
        ::log::log debug "searching: $ext"
        getpakdir $ext/src

        recursepakdir $ext/bldfiles
    }

    foreach ext [glob -nocomplain -directory $contribdir */] {
        set ext [string trim $ext '/']
        ::log::log debug "searching: $ext"
        getpakdir $ext/src

        recursepakdir $ext/bldfiles
    }
}

#----------------------------------------------------------------------------
#  getpakdir search_dir
#  Recursively searches a directory for matching files
#----------------------------------------------------------------------------
proc getpakdir { sdir } {
    global current_file

    foreach ext [glob -nocomplain $sdir/*.bld] {
        set current_file $ext
        ::log::log debug "  [file tail $ext]"
        source $ext
    }
}

#----------------------------------------------------------------------------
#  recursepakdir search_dir
#  Recursively searches a directory for matching files
#----------------------------------------------------------------------------
proc recursepakdir { sdir } {
    getpakdir $sdir

    foreach ext [glob -nocomplain -directory $sdir */ ] {
        recursepakdir $ext
    }
}

#============================================================================
#  Shared Procs - Name collision garbage
#============================================================================

#----------------------------------------------------------------------------
#  check_noblock
#  Used to manage the function naming collisions
#----------------------------------------------------------------------------
proc check_noblock { } {
    global current_block
    global noactive_block
    global current_file

    if {$current_block != $noactive_block} {
        ::log::log error "ERROR: previous begin/end block not closed in file: $current_file"
        exit
    }
}

#----------------------------------------------------------------------------
#  setdir $dir
#  Sets the directory that a workspace should be written to or a module's
#  files can be found in
#----------------------------------------------------------------------------
proc setdir {dir} {
    global current_block
    global wspace_block
    global module_block

    if {$current_block == $wspace_block} {
        global wspace
        global num_wspaces
        set wspace($num_wspaces,dir) [cleanpath $dir]
        return
    } elseif {$current_block == $module_block} {
        global mod
        global num_mods
        # This is not cleaned as this is a naming convention
        # not a direct path
        set mod($num_mods,dir) $dir
    } else {
        ::log::log warning "FAILED to setdir for the currentblocktype $current_block"
    }
}

#----------------------------------------------------------------------------
#  settargets $target_list
#  Sets a list of targets to either a bundle or workspace
#----------------------------------------------------------------------------
proc settargets {target_list} {
    global current_block
    global wspace_block
    global bundle_block

    if {$current_block == $wspace_block} {
        global wspace
        global num_wspaces
        set wspace($num_wspaces,targets) $target_list
        return
    } elseif {$current_block == $bundle_block} {
        global bundle
        global num_bundles
        set bundle($num_bundles,targets) $target_list
    } else {
        ::log::log warning "FAILED to settargets for the currentblocktype $current_block"
    }
}
#----------------------------------------------------------------------------
#  settype $type
#  For a module this is (c|cpp) and for a target it is (dll|exe|lib)
#----------------------------------------------------------------------------
proc settype {type} {
    global current_block
    global target_block
    global module_block

    if {$current_block == $target_block} {
        global tar
        global num_tars
        set tar($num_tars,type) $type
    } elseif {$current_block == $module_block} {
        global mod
        global num_mods
        set mod($num_mods,type) $type
    } else {
        ::log::log warning "FAILED to settype for the currentblocktype $current_block."
    }
}

#----------------------------------------------------------------------------
#  setrtti $allowrtti
#  a boolean that determines whether Run Time Type Identification is allowed.
#  false is the default (see begintarget)
#----------------------------------------------------------------------------
proc setrtti {allowrtti} {
    if {$allowrtti == true} {
        global current_block
        global target_block

        if {$current_block == $target_block} {
            global tar
            global num_tars
            set tar($num_tars,rtti) true
        } else {
            ::log::log warning "FAILED to setrtti for currentblocktype $current_block"
        }
    }
}

#----------------------------------------------------------------------------
#  setexceptions $allowexceptions
#  a boolean that determines whether C++ exception handling is supported.
#  false is the default (see begintarget)
#----------------------------------------------------------------------------
proc setexceptions {allowexceptions} {
    if {$allowexceptions == true } {
        global current_block
        global target_block

        if {$current_block == $target_block} {
            global tar
            global num_tars
            set tar($num_tars,exceptions) true
        } else {
            ::log::log warning "FAILED to setexections for currentblocktype $current_block"
        }
    }
}

#----------------------------------------------------------------------------
#  seticon $filename
#  the filename of the target's icon (has no effect on library targets)
#  nebula.ico is the default
#----------------------------------------------------------------------------
proc seticon {filename} {
    global current_block
    global target_block

    if {$current_block == $target_block} {
        global tar
        global num_tars
        set tar($num_tars,icon) $filename
    } else {
        ::log::log warning "FAILED to seticon for currentblocktype $current_block"
    }
}

#----------------------------------------------------------------------------
#  setmodules $module_list
#  sets the modules to a target or bundle
#----------------------------------------------------------------------------
proc setmodules {module_list} {
    global current_block
    global target_block
    global bundle_block

    if {$current_block == $target_block} {
        global tar
        global num_tars
        set tar($num_tars,modules) $module_list
    } elseif { $current_block == $bundle_block } {
        global bundle
        global num_bundles
        set bundle($num_bundles,modules) $module_list
    } else {
        ::log::log warning "FAILED to setmodules for currentblocktype $current_blocktype"
    }
}

#----------------------------------------------------------------------------
#  annotate $annotation
#  Provides an annotation that the file author can leave behind for use by
#  build system tools
#----------------------------------------------------------------------------
proc annotate {annotation} {
    global current_block
    global wspace_block
    global target_block
    global bundle_block
    global module_block

    if {$current_block == $wspace_block} {
        global wspace
        global num_wspaces
        set wspace($num_wspaces,annotate) $annotation
    } elseif {$current_block == $target_block} {
        global tar
        global num_tars
        set tar($num_tars,annotate) $annotation
    } elseif {$current_block == $bundle_block} {
        global bundle
        global num_bundles
        set bundle($num_bundles,annotate) $annotation
    } elseif {$current_block == $module_block} {
        global mod
        global num_mods
        set mod($num_mods,annotate) $annotation
    } else {
        ::log::log warning "FAILED set annotate for currentblocktype $current_block"
    }
}

#============================================================================
#  Block types and data entry
#============================================================================

#----------------------------------------------------------------------------
#  beginworkspace $name
#----------------------------------------------------------------------------
proc beginworkspace {name} {
    global wspace
    global num_wspaces
    global current_block
    global wspace_block
    global current_file

    check_noblock
    set current_block $wspace_block

    #bld file data
    set wspace($num_wspaces,name)       $name
    set wspace($num_wspaces,dir)        ""
    set wspace($num_wspaces,annotate)   ""
    set wspace($num_wspaces,binarydir)  ""
    set wspace($num_wspaces,libdir)     ""
    set wspace($num_wspaces,targets)    ""
    set wspace($num_wspaces,globaldefs) ""
    set wspace($num_wspaces,targetdefs) ""

    #generated data
    set wspace($num_wspaces,bldfile)    $current_file
}

#----------------------------------------------------------------------------
#  setbinarydir $dir
#  Sets the root directory to be used for binary output - dll and exe
#----------------------------------------------------------------------------
proc setbinarydir {dir} {
    global current_block
    global wspace_block

    if {$current_block == $wspace_block } {
        global wspace
        global num_wspaces
        set wspace($num_wspaces,binarydir) [cleanpath $dir]
    } else {
        ::log::log warning "FAILED to setbinarydir for currentblocktype $current_block"
    }
}

#----------------------------------------------------------------------------
#  setlibdir $dir
#  Sets the root directory to be used for intermediate compilation and
#  possibly lib output - see the fixme.
#  FIXME:  For convention it should be decided by the powers that be
#          whether lib binarydir should mandatorily go in the inter dir,
#          output dir, or leave it to the compiler generators discretion.
#----------------------------------------------------------------------------
proc setlibdir {dir} {
    global current_block
    global wspace_block

    if {$current_block == $wspace_block} {
        global wspace
        global num_wspaces
        set wspace($num_wspaces,libdir) [cleanpath $dir]
    } else {
        ::log::log warning "FAILED to setlibdir for currentblocktype $current_block"
    }
}

#----------------------------------------------------------------------------
#  addglobaldef $defname $value
#  Adds a optional preprocessor define name/value pair to be used by all
#  targets in this workspace
#  FIXME how to deal with a redefine of a already used defname
#----------------------------------------------------------------------------
proc addglobaldef {defname value} {
    global current_block
    global wspace_block

    if {$current_block == $wspace_block} {
        global wspace
        global num_wspaces

        lappend wspace($num_wspaces,globaldefs) $defname $value
    } else {
        ::log::log warning "FAILED to addglobaldef for currentblocktype $current_block"
    }
}

#----------------------------------------------------------------------------
#  addtargetdef $targetname $defname $value
#  As addglobaldef() but the symbols should only be added to the target
#  given by target name
#----------------------------------------------------------------------------
proc addtargetdef {targetname defname value} {
    global current_block
    global wspace_block

    if {$current_block == $wspace_block} {
        global wspace
        global num_wspaces

        lappend wspace($num_wspaces,targetdefs) [list $targetname $defname $value]
    } else {
        ::log::log warning "FAILED to addtargetdef for currentblocktype $current_block"
    }
}

#----------------------------------------------------------------------------
#  endworkspace
#----------------------------------------------------------------------------
proc endworkspace { } {
    global current_block
    global wspace_block

    if {$current_block == $wspace_block} {
        global num_wspaces
        incr num_wspaces
        global noactive_block
        set current_block $noactive_block
    } else {
        ::log::log error "ERROR you have tryed to close a workspace but this is not the currentblocktype $current_block. EXIT NO."
        exit -1
    }
}

#----------------------------------------------------------------------------
#  begintarget $name
#----------------------------------------------------------------------------
proc begintarget {name} {
    global tar
    global num_tars
    global current_block
    global target_block
    global current_file

    check_noblock
    set current_block $target_block

    #bld file data
    set tar($num_tars,name)        $name
    set tar($num_tars,annotate)    ""
    set tar($num_tars,type)        lib
    set tar($num_tars,dllextension) "dll"
    set tar($num_tars,rtti)        false
    set tar($num_tars,exceptions)  false
    set tar($num_tars,icon)        "nebula.ico"
    set tar($num_tars,modules)     ""
    set tar($num_tars,bundles)     ""
    set tar($num_tars,targetdeps)  ""
    set tar($num_tars,defs)        ""

    #generated data
    set tar($num_tars,bldfile)            $current_file
    set tar($num_tars,mergedmods)         ""
    set tar($num_tars,libs_win32)         ""
    set tar($num_tars,libs_win32_release) ""
    set tar($num_tars,libs_win32_debug)   ""
    set tar($num_tars,libs_linux)         ""
    set tar($num_tars,libs_macosx)        ""
    set tar($num_tars,platform)           ""
    set tar($num_tars,moddeffile)         ""
}

#----------------------------------------------------------------------------
#  setbundles $bundle_list
#  Adds bundles to the target.  Modules set to the bundle are treated the
#  same as modules added to the target through the setmodules() call.
#  Targets added to the bundle are treated the same as targets added through
#  the settargetdeps() call.
#----------------------------------------------------------------------------
proc setbundles {bundle_list} {
    global current_block
    global target_block

    if {$current_block == $target_block} {
        global tar
        global num_tars
        addtolist tar($num_tars,bundles)  $bundle_list
    } else {
        ::log::log warning "FAILED to setbundles for currentblocktype $current_block"
    }
}

#----------------------------------------------------------------------------
#  settargetdeps $target_list
#  Specifies the targets that are required to be built before the current
#  target
#----------------------------------------------------------------------------
proc settargetdeps {target_list} {
    global current_block
    global target_block

    if {$current_block == $target_block} {
        global tar
        global num_tars

        addtolist tar($num_tars,targetdeps)  $target_list
    } else {
        ::log::log warning "FAILED to settargetdeps for currentblocktype $current_block"
    }
}

#----------------------------------------------------------------------------
# changedllextension $ext
# Set the extension of the target. Only useful for the dll targets.
# This is commonly used to change the extension to 'mll' or 'dle' for plugin
# dll creation.
#----------------------------------------------------------------------------
proc changedllextension { ext } {
    global current_block
    global target_block

    if {$current_block == $target_block} {
        global tar
        global num_tars

        set tar($num_tars,dllextension) $ext
    } else {
        ::log::log warning "FAILED to changedllextension for currentblocktype $current_block"
    }
}

#----------------------------------------------------------------------------
#  endtarget
#----------------------------------------------------------------------------
proc endtarget { } {
    global current_block
    global target_block

    if {$current_block == $target_block} {
        global num_tars
        incr num_tars
        global noactive_block
        set current_block $noactive_block
    } else {
        ::log::log error "ERROR you have tryed to close a target but this is not the currentblocktype $current_block. EXIT NO."
        exit -1
    }
}

#----------------------------------------------------------------------------
#  beginbundle $name
#----------------------------------------------------------------------------
proc beginbundle {name} {
    global bundle
    global num_bundles
    global current_block
    global bundle_block
    global current_file

    check_noblock
    set current_block $bundle_block

    #bld file data
    set bundle($num_bundles,name)     $name
    set bundle($num_bundles,annotate) ""
    set bundle($num_bundles,modules)  ""
    set bundle($num_bundles,targets)  ""

    #generated data
    set bundle($num_bundles,bldfile)  $current_file

}

#----------------------------------------------------------------------------
#  endbundle
#----------------------------------------------------------------------------
proc endbundle { } {
    global current_block
    global bundle_block

    if {$current_block == $bundle_block} {
        global num_bundles
        incr num_bundles
        global noactive_block
        set current_block $noactive_block
    } else {
        ::log::log error "ERROR you have tryed to close a bundle but this is not the currentblocktype $current_block. EXIT NO."
        exit -1
    }
}

#----------------------------------------------------------------------------
#  beginmodule $name
#----------------------------------------------------------------------------
proc beginmodule {name} {
    global mod
    global num_mods
    global current_block
    global module_block
    global current_file

    check_noblock
    set current_block $module_block

    #bld file data
    set mod($num_mods,name)              $name
    set mod($num_mods,dir)               ""
    set mod($num_mods,annotate)          ""
    set mod($num_mods,platform)          "all"
    set mod($num_mods,type)              "cpp"
    set mod($num_mods,files)             ""
    set mod($num_mods,headers)           ""
    set mod($num_mods,moddeffile)        ""
    set mod($num_mods,libs_win32)         ""
    set mod($num_mods,libs_win32_release) ""
    set mod($num_mods,libs_win32_debug)   ""
    set mod($num_mods,libs_linux)         ""
    set mod($num_mods,libs_macosx)        ""
    set mod($num_mods,moduledeps)        ""
    set mod($num_mods,forcenopkg)        false

    #generated data
    set mod($num_mods,trunkdir)   "nebula2"
    set mod($num_mods,bldfile)    $current_file
    set mod($num_mods,hdrs)       ""
    set mod($num_mods,srcs)       ""
    set mod($num_mods,kernel)     false
    set mod($num_mods,autonopak)  true
    set mod($num_mods,ancestor)   ""
}

#----------------------------------------------------------------------------
# setmoddeffile $filename
# set the moduledefinitionfilename. This is commonly used when writing a Max
# plugin and need to set this additional dllexport description.
#----------------------------------------------------------------------------
proc setmoddeffile { filename } {
    global current_block
    global module_block

    if {$current_block == $module_block} {
        global mod
        global num_mods

        set mod($num_mods,moddeffile) $filename
    } else {
        ::log::log warning "FAILED to setmoddeffile for currentblocktype $current_block"
    }
}

#----------------------------------------------------------------------------
#  setplatform $platform_list
#  A list of valid platforms that this module will compile on.  Valid values
#  are (for now) win32|linux|macosx or 'all'
#----------------------------------------------------------------------------
proc setplatform {platform_list} {
    global current_block
    global module_block

    if {$current_block == $module_block} {
        global mod
        global num_mods
        set mod($num_mods,platform)  $platform_list
    } else {
        ::log::log warning "FAILED to setplatform for currentblocktype $current_block"
    }
}

#----------------------------------------------------------------------------
#  setfiles $file_list
#  The list of source files that make up this module.  Filenames are
#  specified without path or extension
#----------------------------------------------------------------------------
proc setfiles {file_list} {
    global current_block
    global module_block

    if {$current_block == $module_block} {
        global mod
        global num_mods

        addtolist mod($num_mods,files)  $file_list
    } else {
        ::log::log warning "FAILED to setfiles for currentblocktype $current_block"
    }
}

#----------------------------------------------------------------------------
#  setheaders $header_list
#  The list of header files that make up this module directly.  Filenames are
#  specified without path or extension
#----------------------------------------------------------------------------
proc setheaders {header_list} {
    global current_block
    global module_block

    if {$current_block == $module_block} {
        global mod
        global num_mods

        addtolist mod($num_mods,headers)  $header_list
    } else {
        ::log::log warning "FAILED to setheaders for currentblocktype $current_block"
    }
}

#----------------------------------------------------------------------------
#  setlib_win32 $lib_list
#  A list of libs that this module requires to be linked against in both
#  release and debug builds.  Filenames should be specified without path
#  or extension
#----------------------------------------------------------------------------
proc setlibs_win32 {lib_list} {
    global current_block
    global module_block

    if {$current_block == $module_block} {
        global mod
        global num_mods

        addtolist mod($num_mods,libs_win32)  $lib_list
    } else {
        ::log::log warning "FAILED to setlibs_win32 for currentblocktype $current_block"
    }
}

#----------------------------------------------------------------------------
#  setlibs_win32_release $lib_list
#  As setlibs_win32 - but for libs that only apply to a release build
#----------------------------------------------------------------------------
proc setlibs_win32_release {lib_list} {
    global current_block
    global module_block

    if {$current_block == $module_block} {
        global mod
        global num_mods

        addtolist mod($num_mods,libs_win32_release)  $lib_list
    } else {
        ::log::log warning "FAILED to setlibs_win32_release for currentblocktype $current_block"
    }

}

#----------------------------------------------------------------------------
#  setlibs_win32_debug $lib_list
#  As setlibs_win32_release - but for debug builds
#----------------------------------------------------------------------------
proc setlibs_win32_debug {lib_list} {
    global current_block
    global module_block

    if {$current_block == $module_block} {
        global mod
        global num_mods

        addtolist mod($num_mods,libs_win32_debug)  $lib_list
    } else {
        ::log::log warning "FAILED to setlibs_win32_debug for currentblocktype $current_block"
    }
}

#----------------------------------------------------------------------------
#  setlibs_linux $lib_list
#  Libs that are linked against on linux variants.  Filenames are specified
#  without path or extension
#----------------------------------------------------------------------------
proc setlibs_linux {lib_list} {
    global current_block
    global module_block

    if {$current_block == $module_block} {
        global mod
        global num_mods

        addtolist mod($num_mods,libs_linux)  $lib_list
    } else {
        ::log::log warning "FAILED to setlibs_linux for currentblocktype $current_block"
    }

}

#----------------------------------------------------------------------------
#  setlibs_macosx $lib_list
#  Libs that are linked against on Mac OSX.  Filenames are specified without
#  path or extension
#----------------------------------------------------------------------------
proc setlibs_macosx {lib_list} {
    global current_block
    global module_block

    if {$current_block == $module_block} {
        global mod
        global num_mods

        addtolist mod($num_mods,libs_macosx)  $lib_list
    } else {
        ::log::log warning "FAILED to setlibs_maxos for currentblocktype $current_block"
    }
}

#----------------------------------------------------------------------------
#  setmoduledeps $mod_list
#  List of modules that this module depends on for proper functioning.  This
#  list should include both nRoot and non-nRoot modules.  Inherited nRoot
#  class dependencies will be generated by the build system.
#----------------------------------------------------------------------------
proc setmoduledeps {mod_list} {
    global current_block
    global module_block

    if {$current_block == $module_block} {
        global mod
        global num_mods

        addtolist mod($num_mods,moduledeps)  $mod_list
    } else {
        ::log::log warning "FAILED to setmoduledeps for currentblocktype $current_block"
    }
}

#----------------------------------------------------------------------------
#  setnopkg true|false
#  Set this value to true if this module is nRoot-derived and it is desired
#  that it not appear in pkg_XXX.cc files.
#----------------------------------------------------------------------------
proc setnopkg {bool} {
    global current_block
    global module_block

    if {$current_block == $module_block} {
        global mod
        global num_mods

        set mod($num_mods,forcenopkg)  $bool
    } else {
        ::log::log warning "FAILED to setnopkg for currentblocktype $current_block"
    }
}

#----------------------------------------------------------------------------
#  endmodule
#----------------------------------------------------------------------------
proc endmodule { } {
    global current_block
    global module_block

    if {$current_block == $module_block} {
        global num_mods
        incr num_mods

        global noactive_block
        set current_block $noactive_block
    } else {
        ::log::log error "ERROR you have tryed to close a module but this is not the currentblocktype $current_block. EXIT NO."
        exit -1
    }
}
