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
#    setrtti               true|false
#    setexceptions         true|false
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
#----------------------------------------------------------------------------
proc loadbldfiles { } {
    global home
    
    puts "\n****Loading bld files"
    puts "home = $home"
    set startdir "[string trim $home /]/code/"
    
    puts "startdir = $startdir"

    foreach ext [glob -nocomplain -directory $startdir */] {
        set ext [string trim $ext '/']
        puts "searching: $ext"
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
        puts "  [file tail $ext]"    
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
    global current_file
    
    if {$current_block != 0} {
        puts "ERROR: previous begin/end block not closed in file: $current_file"
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
    global wspace
    global mod
    global num_wspaces
    global num_mods
    
    if {$current_block == 1} {
        set wspace($num_wspaces,dir) [cleanpath $dir]
        return
    }
    
    # This is not cleaned as this is a naming convention
    # not a direct path
    set mod($num_mods,dir) $dir
}

#----------------------------------------------------------------------------
#  settargets $target_list
#  Sets a list of targets to either a bundle or workspace
#----------------------------------------------------------------------------
proc settargets {target_list} {
    global current_block
    global wspace
    global bundle
    global num_wspaces
    global num_bundles
    
    if {$current_block == 1} {
        set wspace($num_wspaces,targets) $target_list
        return
    }
    
    set bundle($num_bundles,targets) $target_list
}

#----------------------------------------------------------------------------
#  settype $type
#  For a module this is (c|cpp) and for a target it is (dll|exe|lib)
#----------------------------------------------------------------------------
proc settype {type} {
    global current_block
    global mod
    global tar
    global num_tars
    global num_mods
    
    if {$current_block == 2} {
        set tar($num_tars,type) $type
        return
    }
    
    set mod($num_mods,type) $type
}

#----------------------------------------------------------------------------
#  setrtti $allowrtti
#  a boolean that determines whether Run Time Type Identification is allowed.
#  false is the default (see begintarget)
#----------------------------------------------------------------------------
proc setrtti {allowrtti} {
    global current_block
    global tar
    global num_tars
    
    if {$current_block == 2} {
        if { $allowrtti == true } {
          set tar($num_tars,rtti) true
        }
    }    
}

#----------------------------------------------------------------------------
#  setexceptions $allowexceptions
#  a boolean that determines whether C++ exception handling is supported.
#  false is the default (see begintarget)
#----------------------------------------------------------------------------
proc setexceptions {allowexceptions} {
    global current_block
    global tar
    global num_tars
    
    if {$current_block == 2} {
        if { $allowexceptions == true } {
          set tar($num_tars,exceptions) true
        }
    }    
}


#----------------------------------------------------------------------------
#  setmodules $module_list
#  sets the modules to a target or bundle
#----------------------------------------------------------------------------
proc setmodules {module_list} {
    global current_block
    global tar
    global bundle
    global num_tars
    global num_bundles
    
    if {$current_block == 2} {
        set tar($num_tars,modules) $module_list
        return
    }
    
    set bundle($num_bundles,modules) $module_list
}

#----------------------------------------------------------------------------
#  annotate $annotation
#  Provides an annotation that the file author can leave behind for use by
#  build system tools
#----------------------------------------------------------------------------
proc annotate {annotation} {
    global current_block
    global wspace
    global tar
    global bundle
    global mod
    global num_wspaces
    global num_tars
    global num_bundles
    global num_mods
    
    if {$current_block == 1} {
        set wspace($num_tars,annotate) $annotation
    } elseif {$current_block == 2} {
        set tar($num_tars,annotate) $annotation
    } elseif {$current_block == 3} {
        set bundle($num_bundles,annotate) $annotation
    } else {    
        set mod($num_mods,annotate) $annotation
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
    global current_file
    
    check_noblock
    set current_block 1
    
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
    global wspace
    global num_wspaces
    
    set wspace($num_wspaces,binarydir) [cleanpath $dir]
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
    global wspace
    global num_wspaces
    
    set wspace($num_wspaces,libdir) [cleanpath $dir]
}

#----------------------------------------------------------------------------
#  addglobaldef $defname $value
#  Adds a optional preprocessor define name/value pair to be used by all
#  targets in this workspace
#----------------------------------------------------------------------------
proc addglobaldef {defname value} {
    global wspace
    global num_wspaces
    
    lappend wspace($num_wspaces,globaldefs) $defname $value
}

#----------------------------------------------------------------------------
#  addtargetdef $targetname $defname $value
#  As addglobaldef() but the symbols should only be added to the target
#  given by target name
#----------------------------------------------------------------------------
proc addtargetdef {targetname defname value} {
    global wspace
    global num_wspaces
    
    lappend wspace($num_wspaces,targetdefs) [list $targetname $defname $value]
}

#----------------------------------------------------------------------------
#  endworkspace
#----------------------------------------------------------------------------
proc endworkspace { } {
    global num_wspaces
    global current_block
    
    set current_block 0
    incr num_wspaces
}


#----------------------------------------------------------------------------
#  begintarget $name
#----------------------------------------------------------------------------
proc begintarget {name} {
    global tar
    global num_tars
    global current_block
    global current_file
    
    check_noblock
    set current_block 2
    
    #bld file data
    set tar($num_tars,name)        $name
    set tar($num_tars,annotate)    ""
    set tar($num_tars,type)        lib
    set tar($num_tars,rtti)        false
    set tar($num_tars,exceptions)  false
    set tar($num_tars,modules)     ""
    set tar($num_tars,bundles)     ""
    set tar($num_tars,targetdeps)  ""
    
    #generated data
    set tar($num_tars,bldfile)            $current_file
    set tar($num_tars,mergedmods)         ""
    set tar($num_tars,libs_win32)         ""
    set tar($num_tars,libs_win32_release) ""
    set tar($num_tars,libs_win32_debug)   ""
    set tar($num_tars,libs_linux)         ""
    set tar($num_tars,libs_macosx)        ""   
    set tar($num_tars,platform)           ""
}

#----------------------------------------------------------------------------
#  setbundles $bundle_list
#  Adds bundles to the target.  Modules set to the bundle are treated the
#  same as modules added to the target through the setmodules() call.
#  Targets added to the bundle are treated the same as targets added through
#  the settargetdeps() call.
#----------------------------------------------------------------------------
proc setbundles {bundle_list} {
    global tar
    global num_tars
    
    addtolist tar($num_tars,bundles)  $bundle_list
}

#----------------------------------------------------------------------------
#  settargetdeps $target_list
#  Specifies the targets that are required to be built before the current
#  target
#----------------------------------------------------------------------------
proc settargetdeps {target_list} {
    global tar
    global num_tars
    
    addtolist tar($num_tars,targetdeps)  $target_list
}

#----------------------------------------------------------------------------
#  endtarget
#----------------------------------------------------------------------------
proc endtarget { } {
    global num_tars
    global current_block
    
    set current_block 0
    incr num_tars
}

#----------------------------------------------------------------------------
#  beginbundle $name
#----------------------------------------------------------------------------
proc beginbundle {name} {
    global bundle
    global num_bundles
    global current_block
    global current_file
    
    check_noblock
    set current_block 4
    
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
    global num_bundles
    global current_block
    
    set current_block 0
    incr num_bundles
}

#----------------------------------------------------------------------------
#  beginmodule $name
#----------------------------------------------------------------------------
proc beginmodule {name} {
    global mod
    global num_mods
    global current_block
    global current_file
    
    check_noblock
    set current_block 3
    
    #bld file data
    set mod($num_mods,name)              $name
    set mod($num_mods,dir)               ""
    set mod($num_mods,annotate)          ""
    set mod($num_mods,platform)          "all"
    set mod($num_mods,type)              "cpp"
    set mod($num_mods,files)             ""
    set mod($num_mods,headers)           ""
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
#  setplatform $platform_list
#  A list of valid platforms that this module will compile on.  Valid values
#  are (for now) win32|linux|macosx or 'all'
#----------------------------------------------------------------------------
proc setplatform {platform_list} {
    global mod
    global num_mods
    
    set mod($num_mods,platform)  $platform_list
}

#----------------------------------------------------------------------------
#  setfiles $file_list
#  The list of source files that make up this module.  Filenames are 
#  specified without path or extension
#----------------------------------------------------------------------------
proc setfiles {file_list} {
    global mod
    global num_mods

    addtolist mod($num_mods,files)  $file_list
}

#----------------------------------------------------------------------------
#  setheaders $header_list
#  The list of header files that make up this module directly.  Filenames are
#  specified without path or extension
#----------------------------------------------------------------------------
proc setheaders {header_list} {
    global mod
    global num_mods

    addtolist mod($num_mods,headers)  $header_list
}

#----------------------------------------------------------------------------
#  setlib_win32 $lib_list
#  A list of libs that this module requires to be linked against in both
#  release and debug builds.  Filenames should be specified without path
#  or extension
#----------------------------------------------------------------------------
proc setlibs_win32 {lib_list} {
    global mod
    global num_mods

    addtolist mod($num_mods,libs_win32)  $lib_list
}

#----------------------------------------------------------------------------
#  setlibs_win32_release $lib_list
#  As setlibs_win32 - but for libs that only apply to a release build
#----------------------------------------------------------------------------
proc setlibs_win32_release {lib_list} {
    global mod
    global num_mods

    addtolist mod($num_mods,libs_win32_release)  $lib_list
}

#----------------------------------------------------------------------------
#  setlibs_win32_debug $lib_list
#  As setlibs_win32_release - but for debug builds
#----------------------------------------------------------------------------
proc setlibs_win32_debug {lib_list} {
    global mod
    global num_mods

    addtolist mod($num_mods,libs_win32_debug)  $lib_list
}

#----------------------------------------------------------------------------
#  setlibs_linux $lib_list
#  Libs that are linked against on linux variants.  Filenames are specified
#  without path or extension
#----------------------------------------------------------------------------
proc setlibs_linux {lib_list} {
    global mod
    global num_mods

    addtolist mod($num_mods,libs_linux)  $lib_list
}

#----------------------------------------------------------------------------
#  setlibs_macosx $lib_list
#  Libs that are linked against on Mac OSX.  Filenames are specified without
#  path or extension
#----------------------------------------------------------------------------
proc setlibs_macosx {lib_list} {
    global mod
    global num_mods

    addtolist mod($num_mods,libs_macosx)  $lib_list
}

#----------------------------------------------------------------------------
#  setmoduledeps $mod_list
#  List of modules that this module depends on for proper functioning.  This 
#  list should include both nRoot and non-nRoot modules.  Inherited nRoot
#  class dependencies will be generated by the build system.
#----------------------------------------------------------------------------
proc setmoduledeps {mod_list} {
    global mod
    global num_mods

    addtolist mod($num_mods,moduledeps)  $mod_list
}

#----------------------------------------------------------------------------
#  setnopkg true|false
#  Set this value to true if this module is nRoot-derived and it is desired
#  that it not appear in pkg_XXX.cc files.
#----------------------------------------------------------------------------
proc setnopkg {bool} {
    global mod
    global num_mods
    
    set mod($num_mods,forcenopkg)  $bool
}

#----------------------------------------------------------------------------
#  endmodule
#----------------------------------------------------------------------------
proc endmodule { } {
    global num_mods
    global current_block
    
    set current_block 0
    incr num_mods
}
