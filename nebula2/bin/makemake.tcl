#--------------------------------------------------------------------
#   makemake.tcl
#   ============
#   Tcl procs to convert Nebula package definitions into Makefiles
#   and Visual Studio workspace files.
#
#   Nebula package files define 'modules' and 'targets'. A module
#   is a collection of source files that belong together.
#   A target is a collection of modules, and specifies how the
#   modules should be compiled into a binary. This can be a
#   generic dll, a class package dll, or an executable.
#
#   (C) 2000 A.Weissflog
#
#   17-May-00   floh    added support for header files
#   07-Nov-00   floh    new target type 'mll' for maya plugins
#   01-Feb-01   floh    support for setdoxycfg keyword
#   24-Jul-02   floh    added Xbox support
#--------------------------------------------------------------------

global includePath
set headerPrefix "../inc/"

source "$env(RL_HOME)/bin/util.tcl"
source "$env(RL_HOME)/bin/makedepend.tcl"

#--------------------------------------------------------------------
#   structs
#   =======
#   mod(i,name)         - name of module
#   mod(i,dir)          - directory where source files reside
#   mod(i,files)        - list of source files for module 'name'
#   mod(i,headers)      - list of header files for module 'name'
#   mod(i,version)      - version file for module 'name', can be empty
#   mod(i,objs)         - object file names for win32 (generated)
#   mod(i,srcs)         - source file names for win32 (generated)
#   mod(i,hdrs)         - header file names
#   mod(i,deps,src)     - dependencies for 'src' (generated)
#   mod(i,type)         - "cpplib", "clib" -> these sources are just c 
#                         file collections
#
#   tar(i,name)                 - name of target
#   tar(i,platform)             - one of 'all|win32|unix|xbox'
#   tar(i,type)                 - one of 'dll|package|exe|mll|lib'
#   tar(i,mods)                 - module list for target 'name'
#   tar(i,depends)              - list of other targets, which this target depends on
#   tar(i,libs_win32)           - general win32 specific link libs
#   tar(i,libs_unix)            - unix specific link libs
#   tar(i,libs_xbox)            - xbox specific link libs
#   tar(i,libs_win32_debug)     - debug/win32 specific link libs 
#   tar(i,libs_win32_release)   - release/win32 specific link libs
#   tar(i,libs_xbox_debug)      - debug/xbox specific link libs
#   tar(i,libs_xbox_release)    - release/xbox specific link libs
#   tar(i,rsrc_win32)           - win32 specific resource files
#   tar(i,doxycfg)              - location of Doxygen config file
#--------------------------------------------------------------------

global mod
global tar
global num_mods
global num_tars

set num_mods 0
set num_tars 0

set platform [get_platform]
set header_post ".h"
set source_post ".cc"

#--------------------------------------------------------------------
#   procs
#   =====
#   beginmodule $mod_name
#       setdir $subdir
#       setfiles   $file $file $file...
#       setheaders $file $file $file...
#       setver $file
#   endmodule
#   begintarget $name
#       setmods $module $module $module...
#       setdepend $target $target...
#       setlibs_win32 $lib $lib...
#       setlibs_unix  $lib $lib...
#   endtarget
#--------------------------------------------------------------------

#====================================================================
#   UTILITY PROCEDURES
#====================================================================

#--------------------------------------------------------------------
#   cvs
#   Minimal tcl shell around cvs.
#--------------------------------------------------------------------
proc cvs { cmd dir pattern } {
    global platform
    
    set err "ok"
    set cwd [pwd]
    cd $dir
    set files [glob -nocomplain $pattern]

    if {$files != ""} {
        set sh "sh"
        set sh_arg "-c"
        if {($platform=="win2k") || ($platform=="winnt")} {
            # replace slashes by backslash in 'dir'
            set dir [string map {/ \\} $dir]
            set sh "cmd.exe"
            set sh_arg "/c"
            set sh_delim "&&"
        } elseif {$platform=="win9x"} {
            set dir [string map {/ \\} $dir]
            set sh "command.com"
            set sh_arg "/c"
            set sh_delim "&&"
        } elseif {$platform=="unix"} {
            set sh "sh"
            set sh_arg "-c"
            set sh_delim ";"
        }
        after 500
        switch $cmd {
            addascii { catch { exec $sh $sh_arg "cd $dir $sh_delim cvs add $files"  } err }
            commit   { catch { exec $sh $sh_arg "cd $dir $sh_delim cvs commit -m bla $files" } err }
            edit     { catch { exec $sh $sh_arg "cd $dir $sh_delim cvs edit $files" } err }
        }
        puts "-> cvs $cmd $files = $err"
    } else {
        puts "-> ignoring cvs on empty list"
    }
    cd $cwd
}

#--------------------------------------------------------------------
#   make_cvs_edit
#   Puts all affected files into 'cvs edit' state.
#   11-Mar-00   floh    created
#   20-Mar-00   floh    no longer affect .toc files
#--------------------------------------------------------------------
proc make_cvs_edit {path} {
    set cwd [pwd]
    cvs edit $path/src/packages *.cc
    cvs edit $path/src          *.mak
    cvs edit $path/vstudio      *.dsp
    cvs edit $path/vstudio      *.dsw
    cd $cwd
}

#--------------------------------------------------------------------
#   make_cvs_add
#   Add any new files.
#   11-Mar-00   floh    created
#   20-Mar-00   floh    + no longer affect .toc files
#                       + no longer commit changes, just add
#--------------------------------------------------------------------
proc make_cvs_add {path} {
    set cwd [pwd]
    cvs addascii $path/src/packages *.cc
    cvs addascii $path/src          *.mak
    cvs addascii $path/vstudio      *.dsp
    cvs addascii $path/vstudio      *.dsw
    cd $cwd
}

#--------------------------------------------------------------------
#   addtolist $var $list
#   04-Mar-00   floh    created
#--------------------------------------------------------------------
proc addtolist {var list} {
    upvar $var v
    for {set i 0} {$i < [llength $list]} {incr i} {
        append v [lindex $list $i] " "
    }
}

#--------------------------------------------------------------------
#   findmodbyname $name
#   Find module index by name.
#   04-Mar-00   floh    created
#--------------------------------------------------------------------
proc findmodbyname {name} {
    global mod
    global num_mods
    for {set i 0} {$i < $num_mods} {incr i} {
        if {$name == $mod($i,name)} {
            return $i
        }
    }
    puts "ERROR: module '$name' not defined!"
    exit
}

#--------------------------------------------------------------------
#   findtargetbyname $name
#   Find target index by name.
#   04-Mar-00   floh    created
#--------------------------------------------------------------------
proc findtargetbyname {name} {
    global tar
    global num_tars
    for {set i 0} {$i < $num_tars} {incr i} {
        if {$name == $tar($i,name)} {
            return $i
        }
    }
    puts "ERROR: target '$name' not defined!"
    exit
}

#--------------------------------------------------------------------
#   gen_filelists $module
#   Generate source and object file name lists for a given module.
#   04-Mar-00   floh    created
#   11-Mar-00   floh    + generate dependencies for files
#   20-Mar-00   floh    + global variable 'global_gendeps' used to
#                         turn dependency-generation on/off
#   17-May-00   floh    + support for header files
#--------------------------------------------------------------------
proc gen_filelists {module} {
    global mod
    global num_mods
    global global_gendeps
    global headerPrefix

    set i [findmodbyname $module]

    set num_files [llength $mod($i,files)]

    # generate object file lists
    set mod($i,objs) ""
    for {set j 0} {$j < $num_files} {incr j} {
        lappend mod($i,objs) "$mod($i,dir)/[lindex $mod($i,files) $j]\$(OBJ)"
    }

    # generate source file lists and dependencies
    set mod($i,srcs) ""   
    for {set j 0} {$j < $num_files} {incr j} {
        set cur_src "$mod($i,dir)/[lindex $mod($i,files) $j].cc"
        lappend mod($i,srcs) $cur_src
        if {$global_gendeps} {
            puts "-> dependencies for $cur_src"
            set tmp_dep_list [get_depends $cur_src $headerPrefix ""]
            set dep_list ""
            for {set k 0} {$k < [llength $tmp_dep_list]} {incr k} {
                append dep_list "$headerPrefix/[lindex $tmp_dep_list $k]" " "
            }
            set mod($i,deps,$cur_src) $dep_list
        }
    }

    # append header files to 'hdrs'
    set mod($i,hdrs) ""
    set num_headers [llength $mod($i,headers)]
    for {set j 0} {$j < $num_headers} {incr j} {
        set cur_hdr "$mod($i,dir)/[lindex $mod($i,headers) $j].h"
        lappend mod($i,hdrs) $cur_hdr
    }
}

#====================================================================
#   .PAK FILE PARSING PROCEDURES
#====================================================================

#--------------------------------------------------------------------
#   beginmodule $name
#   Start definition of a module.
#   04-Mar-00   floh    created
#--------------------------------------------------------------------
proc beginmodule {name} {
    global num_mods
    global mod
    set mod($num_mods,name)         $name
    set mod($num_mods,dir)          ""
    set mod($num_mods,files)        ""
    set mod($num_mods,headers)      ""
    set mod($num_mods,version)      ""
    set mod($num_mods,type)         ""
}

#--------------------------------------------------------------------
#   setdir $subdir
#   Specify path where module files live.
#   04-Mar-00   floh    created
#--------------------------------------------------------------------
proc setdir {subdir} {
    global mod
    global num_mods
    set mod($num_mods,dir) $subdir
}

#--------------------------------------------------------------------
#   setfiles $files
#   04-Mar-00   floh    created
#--------------------------------------------------------------------
proc setfiles {file_list} {
    global mod
    global num_mods
    addtolist mod($num_mods,files) $file_list
}

#--------------------------------------------------------------------
#   setmodtype type
#   24-Jul-02   floh    created
#--------------------------------------------------------------------
proc setmodtype {type} {
    global mod
    global num_mods
    set mod($num_mods,type) $type
}    

#--------------------------------------------------------------------
#   setheaders $files
#   17-May-00   floh    created
#--------------------------------------------------------------------
proc setheaders {file_list} {
    global mod
    global num_mods
    addtolist mod($num_mods,headers) $file_list
}

#--------------------------------------------------------------------
#   setversion $version_file
#   04-Mar-00   floh    created
#--------------------------------------------------------------------
proc setversion {version_file} {
    global mod
    global num_mods
    set mod($num_mods,version) $version_file
}

#--------------------------------------------------------------------
#   endmodule
#   04-Mar-00   floh    created
#--------------------------------------------------------------------
proc endmodule { } {
    global mod
    global num_mods

    # generate file lists for module
    set name $mod($num_mods,name)
    incr num_mods
    gen_filelists $name
}

#--------------------------------------------------------------------
#   dumpmodules
#   Print data of all defined modules.
#   04-Mar-00   floh    created
#--------------------------------------------------------------------
proc dumpmodules { } {
    global mod
    global num_mods
    puts "\n\n======================================================="
    puts " MODULES"
    puts " "
    for {set i 0} {$i < $num_mods} {incr i} {
        puts "-------------------------------------------------------"
        puts " module:      $mod($i,name)"
        puts " dir:         $mod($i,dir)"
        puts " files:       $mod($i,files)"
        puts " headers:     $mod($i,headers)"
        puts " version: $mod($i,version)"
    }
}

#--------------------------------------------------------------------
#   begintarget $name
#   04-Mar-00   floh    created
#--------------------------------------------------------------------
proc begintarget {name} {
    global num_tars
    global tar
    set tar($num_tars,name)                 $name
    set tar($num_tars,platform)             "all"
    set tar($num_tars,type)                 ""
    set tar($num_tars,mods)                 ""
    set tar($num_tars,depends)              ""
    set tar($num_tars,libs_win32)           ""
    set tar($num_tars,libs_win32_debug)     ""
    set tar($num_tars,libs_win32_release)   ""
    set tar($num_tars,libs_xbox_debug)      ""
    set tar($num_tars,libs_xbox_release)    ""
    set tar($num_tars,libs_unix)            ""
    set tar($num_tars,libs_xbox)            ""
    set tar($num_tars,rsrc_win32)           ""
    set tar($num_tars,doxycfg)              ""
    set tar($num_tars,doxytitle)            ""
}

#--------------------------------------------------------------------
#   setplatform $platform
#   09-Mar-00   floh    created
#--------------------------------------------------------------------
proc setplatform {platform} {
    global num_tars
    global tar
    set tar($num_tars,platform) $platform
}

#--------------------------------------------------------------------
#   settype $type
#   04-Mar-00   floh    created
#--------------------------------------------------------------------
proc settype {type} {
    global num_tars
    global tar
    set tar($num_tars,type) $type
}

#--------------------------------------------------------------------
#   setmods $modules
#   04-Mar-00   floh    created
#--------------------------------------------------------------------
proc setmods {mod_list} {
    global num_tars
    global tar
    set tar($num_tars,mods) ""
    for {set i 0} {$i < [llength $mod_list]} {incr i} {
        lappend tar($num_tars,mods) [lindex $mod_list $i] 
    }
}

#--------------------------------------------------------------------
#   setclusters $clusters
#--------------------------------------------------------------------
proc setclusters {cluster_list} {
    global home
    global num_tars
    global tar
    global header_post
    global source_post
    
    for {set i 0} {$i < [llength $cluster_list]} {incr i} {
        set cluster [lindex $cluster_list $i]
        
        # Get list of header files
        set full_headers [glob -nocomplain -directory ./$cluster *$header_post]
        set headers ""
        for {set j 0} {$j < [llength $full_headers]} {incr j} {
            lappend headers [file rootname [file tail [lindex $full_headers $j]]]
        }

        # Get list of source files
        set full_sources [glob -nocomplain -directory ./$cluster *$source_post]
        set sources ""
        for {set j 0} {$j < [llength $full_sources]} {incr j} {
            lappend sources [file rootname [file tail [lindex $full_sources $j]]]
        }
        
        # Make module from cluster
        beginmodule $cluster
            setmodtype cpplib
            setdir $cluster
            setfiles $sources
            setheaders $headers
        endmodule
    }
    
    # Set modules
    setmods $cluster_list
}

#--------------------------------------------------------------------
#   setdepends $depends
#   04-Mar-00   floh    created
#--------------------------------------------------------------------
proc setdepends {depends} {
    global num_tars
    global tar
    set tar($num_tars,depends) $depends
}

#--------------------------------------------------------------------
#   setdoxycfg $filename
#   01-Feb-01   floh    created
#--------------------------------------------------------------------
proc setdoxycfg {filename} {
    global num_tars
    global tar
    set tar($num_tars,doxycfg)   $filename
}

#--------------------------------------------------------------------
#   setlibs_win32 $libs
#   04-Mar-00   floh    created
#--------------------------------------------------------------------
proc setlibs_win32 {libs} {
    global num_tars
    global tar
    addtolist tar($num_tars,libs_win32) $libs
}

#--------------------------------------------------------------------
#   setlibs_win32_debug $libs
#   31-Jan-03   floh    created
#--------------------------------------------------------------------
proc setlibs_win32_debug {libs} {
    global num_tars
    global tar
    addtolist tar($num_tars,libs_win32_debug) $libs
}

#--------------------------------------------------------------------
#   setlibs_win32_release $libs
#   31-Jan-03   floh    created
#--------------------------------------------------------------------
proc setlibs_win32_release {libs} {
    global num_tars
    global tar
    addtolist tar($num_tars,libs_win32_release) $libs
}

#--------------------------------------------------------------------
#   setlibs_unix $libs
#   04-Mar-00   floh    created
#--------------------------------------------------------------------
proc setlibs_unix {libs} {
    global num_tars
    global tar
    addtolist tar($num_tars,libs_unix) $libs
}

#--------------------------------------------------------------------
#   setlibs_xbox $libs
#   18-Dec-02   floh    created
#--------------------------------------------------------------------
proc setlibs_xbox {libs} {
    global num_tars
    global tar
    addtolist tar($num_tars,libs_xbox) $libs
}

#--------------------------------------------------------------------
#   setlibs_xbox_debug $libs
#   31-Jan-03   floh    created
#--------------------------------------------------------------------
proc setlibs_xbox_debug {libs} {
    global num_tars
    global tar
    addtolist tar($num_tars,libs_xbox_debug) $libs
}

#--------------------------------------------------------------------
#   setlibs_xbox_release $libs
#   31-Jan-03   floh    created
#--------------------------------------------------------------------
proc setlibs_xbox_release {libs} {
    global num_tars
    global tar
    addtolist tar($num_tars,libs_xbox_release) $libs
}

#--------------------------------------------------------------------
#   setrsrc_win32 $libs
#   04-Mar-00   floh    created
#--------------------------------------------------------------------
proc setrsrc_win32 {file_list} {
    global num_tars
    global tar
    addtolist tar($num_tars,rsrc_win32) $file_list
}

#--------------------------------------------------------------------
#   endtarget
#   04-Mar-00   floh    created
#--------------------------------------------------------------------
proc endtarget { } {
    global num_tars
    incr num_tars
}

#--------------------------------------------------------------------
#   dumptargets
#   Print data of all defined targets.
#   04-Mar-00   floh    created
#--------------------------------------------------------------------
proc dumptargets { } {
    global tar
    global num_tars
    puts "\n\n======================================================="
    puts " TARGETS"
    puts " "
    for {set i 0} {$i < $num_tars} {incr i} {
        puts "-------------------------------------------------------"
        puts " target:      $tar($i,name)"
        puts " type:        $tar($i,type)"
        puts " mods:        $tar($i,mods)"
        puts " depends:     $tar($i,depends)"
        puts " libs_win32:  $tar($i,libs_win32)"
        puts " libs_unix:   $tar($i,libs_unix)"
        puts " rsrc_win32:  $tar($i,rsrc_win32)"
    }
}

#====================================================================
#   PACKAGE HELPER FUNCTIONS
#====================================================================

#--------------------------------------------------------------------
#   gentocfile 
#
#   Generate the global Nebula classes.toc file from all
#   package targets. Expects the toc file's complete path
#   as argument (i.e. $env(RL_HOME)/bin/win32/classes.toc)
#   
#   09-Oct-02   floh    complete rewrite
#--------------------------------------------------------------------
proc gentocfile {filename} {
    global mod
    global tar
    global num_tars
    global home

    puts "*** generating toc file"

    # open toc file
    set cid [open $filename w]
    puts $cid "# ---"
    puts $cid "# machine generated, don't edit"
    puts $cid "# [clock format [clock seconds]]"
    puts $cid "# ---"

    # for each target...
    for {set i 0} {$i < $num_tars} {incr i} {
        if {$tar($i,type) == "package"} {
            
            # write package name
            puts $cid "\$$tar($i,name)"

            # write class names
            for {set j 0} {$j < [llength $tar($i,mods)]} {incr j} {

                set curMod [lindex $tar($i,mods) $j]
                set modIndex [findmodbyname $curMod]
                if {!(($mod($modIndex,type) == "cpplib") || ($mod($modIndex,type) == "clib"))} {
                    puts $cid "%$curMod"
                }
            }
        }
    }
    close $cid
}

#--------------------------------------------------------------------
#   genpacksrc
#   Generate package.cc file for a package target.
#
#   06-Mar-00   floh    created
#   24-Jul-02   floh    + added support for module types
#--------------------------------------------------------------------
proc genpacksrc {tarname} {

    global tar
    global mod

    set i [findtargetbyname $tarname]

    puts "-> generating packages/pkg_$tarname.cc"
    set cid [open "packages/pkg_$tarname.cc" w]
    puts $cid "//----------------------------------------------------------"
    puts $cid "// pkg_$tarname.cc"
    puts $cid "// MACHINE GENERATED, DON'T EDIT!"
    puts $cid "//----------------------------------------------------------"
    puts $cid {#include "kernel/ntypes.h"}
    puts $cid {#include "kernel/nkernelserver.h"}
    for {set j 0} {$j < [llength $tar($i,mods)]} {incr j} {
        set curMod [lindex $tar($i,mods) $j]
        set modIndex [findmodbyname $curMod]

        # don't create a package entry for lib modules
        if {!(($mod($modIndex,type) == "cpplib") || ($mod($modIndex,type) == "clib"))} {    
            set initproc "n_init_$curMod"
            set finiproc "n_fini_$curMod"
            set newproc  "n_new_$curMod"
            set versproc "n_version_$curMod"
            puts $cid "extern \"C\" bool $initproc (nClass *, nKernelServer *);"
            puts $cid "extern \"C\" void $finiproc (void);"
            puts $cid "extern \"C\" void *$newproc (void);"
            puts $cid "extern \"C\" char *$versproc (void);"
        }
    }
    puts $cid "extern \"C\" void N_EXPORT n_addmodules(nKernelServer *);"
    puts $cid "extern \"C\" void N_EXPORT n_remmodules(nKernelServer *);"
    puts $cid "void N_EXPORT n_addmodules(nKernelServer *ks)"
    puts $cid "\{"
    for {set j 0} {$j < [llength $tar($i,mods)]} {incr j} {
        set curMod [lindex $tar($i,mods) $j]
        set modIndex [findmodbyname $curMod]
        
        # don't create a package entry for modules containing the main function
        if {!(($mod($modIndex,type) == "cpplib") || ($mod($modIndex,type) == "clib"))} {    
            set initproc "n_init_$curMod"
            set finiproc "n_fini_$curMod"
            set newproc  "n_new_$curMod"
            set versproc "n_version_$curMod"
            puts $cid "    ks->AddModule(\"$curMod\",$initproc,$finiproc,$newproc,$versproc);"
        }
    }
    puts $cid "\}"
    puts $cid "void N_EXPORT n_remmodules(nKernelServer *)"
    puts $cid "\{"
    puts $cid "\}"
    puts $cid "//----------------------------------------------------------"
    puts $cid "// EOF"
    puts $cid "//----------------------------------------------------------"
    close $cid
}

#--------------------------------------------------------------------
#   gen_package_source
#   Generate one package src file from all targets, this is useful
#   for statically linked Nebula applications.
#
#   19-Nov-02   floh    created
#--------------------------------------------------------------------
proc gen_package_source {filename list_of_targets} {
global tar
global mod

    # for each package
    puts "-> generating packages/pkg_$filename.cc"

    set cid [open "packages/pkg_$filename.cc" w]
    puts $cid "//----------------------------------------------------------"
    puts $cid "// pkg_$filename.cc"
    puts $cid "// MACHINE GENERATED, DON'T EDIT!"
    puts $cid "//----------------------------------------------------------"
    puts $cid {#include "kernel/ntypes.h"}
    puts $cid {#include "kernel/nkernelserver.h"}
    puts $cid {#ifdef __XBxX__}
    puts $cid {#undef __WIN32__}
    puts $cid {#endif}
    
    puts $cid "extern \"C\" void N_EXPORT n_addmodules(nKernelServer *);"
    puts $cid "extern \"C\" void N_EXPORT n_remmodules(nKernelServer *);"
    for {set i 0} {$i < [llength $list_of_targets]} {incr i} {
        set t [findtargetbyname [lindex $list_of_targets $i]]
        
        # start ifdef block if platform specific target
        set insideIfdef 0
        if {$tar($t,platform)=="win32"} {
            puts $cid "\#ifdef __WIN32__"
            set insideIfdef 1
        } elseif {$tar($t,platform)=="xbox"} {
            puts $cid "\#ifdef __XBxX__"
            set insideIfdef 1
        } elseif {$tar($t,platform)=="unix"} {
            puts $cid "\#ifdef __LINUX__"
            set insideIfdef 1
        } elseif {$tar($t,platform)=="macosx"} {
            puts $cid "\#ifdef __MACOSX__"
            set insideIfdef 1
        }
        
        # write module procedure prototypes
        for {set j 0} {$j < [llength $tar($t,mods)]} {incr j} {
            set curMod [lindex $tar($t,mods) $j]
            set modIndex [findmodbyname $curMod]
    
            # don't create a package entry for lib modules
            if {!(($mod($modIndex,type) == "cpplib") || ($mod($modIndex,type) == "clib"))} {    
                set initproc "n_init_$curMod"
                set finiproc "n_fini_$curMod"
                set newproc  "n_new_$curMod"
                set versproc "n_version_$curMod"
                puts $cid "extern \"C\" bool $initproc (nClass *, nKernelServer *);"
                puts $cid "extern \"C\" void $finiproc (void);"
                puts $cid "extern \"C\" void *$newproc (void);"
                puts $cid "extern \"C\" char *$versproc (void);"
            }
        }
        
        # finish platform specific ifdef block
        if {$insideIfdef} {
            puts $cid "\#endif"
        }
    }

    puts $cid "void N_EXPORT n_addmodules(nKernelServer *ks)"
    puts $cid "\{"
    for {set i 0} {$i < [llength $list_of_targets]} {incr i} {

        set t [findtargetbyname [lindex $list_of_targets $i]]
        
        # start ifdef block if platform specific target
        set insideIfdef 0
        if {$tar($t,platform)=="win32"} {
            puts $cid "\#ifdef __WIN32__"
            set insideIfdef 1
        } elseif {$tar($t,platform)=="xbox"} {
            puts $cid "\#ifdef __XBxX__"
            set insideIfdef 1
        } elseif {$tar($t,platform)=="unix"} {
            puts $cid "\#ifdef __LINUX__"
            set insideIfdef 1
        } elseif {$tar($t,platform)=="macosx"} {
            puts $cid "\#ifdef __MACOSX__"
            set insideIfdef 1
        }

        # write code to add target modules to Nebula kernel        
        for {set j 0} {$j < [llength $tar($t,mods)]} {incr j} {
            set curMod [lindex $tar($t,mods) $j]
            set modIndex [findmodbyname $curMod]
            
            # don't create a package entry for modules containing the main function
            if {!(($mod($modIndex,type) == "cpplib") || ($mod($modIndex,type) == "clib"))} {    
                set initproc "n_init_$curMod"
                set finiproc "n_fini_$curMod"
                set newproc  "n_new_$curMod"
                set versproc "n_version_$curMod"
                puts $cid "    ks->AddModule(\"$curMod\",$initproc,$finiproc,$newproc,$versproc);"
            }
        }

        # finish platform specific ifdef block
        if {$insideIfdef} {
            puts $cid "\#endif"
        }        
    }
    puts $cid "\}"
    puts $cid "void N_EXPORT n_remmodules(nKernelServer *)"
    puts $cid "\{"
    puts $cid "\}"
    puts $cid "//----------------------------------------------------------"
    puts $cid "// EOF"
    puts $cid "//----------------------------------------------------------"
    close $cid
}

#--------------------------------------------------------------------
#   gen_component_list
#   Generate a list of dependent components from a target dependency list.
#   The target itself will be included at the head of the list.
#--------------------------------------------------------------------
proc gen_component_list {target} {
global tar
global num_tars

    set component_list $target
    set t [findtargetbyname $target]
    for {set i 0} {$i < [llength $tar($t,depends)]} {incr i} {
        lappend component_list [lindex $tar($t,depends) $i]
    }
    return $component_list
}

#====================================================================
#   Include generator functions for GNU Make and Visual Studio
#   Workspace files.
#====================================================================
#vc7
source $env(RL_HOME)/bin/makesln.tcl
#unix
source $env(RL_HOME)/bin/makeunix.tcl
#vc6
source $env(RL_HOME)/bin/makedsw.tcl

#TODO:
#macosx and other build system script includes


#--------------------------------------------------------------------
#   EOF
#--------------------------------------------------------------------
