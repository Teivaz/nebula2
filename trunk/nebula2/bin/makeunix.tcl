#--------------------------------------------------------------------
#   makeunix.tcl
#   ===========
#   Functions to generate Unix Makefiles
#
#   The makefile is generated for 3 buildtypes release/default, debug
#   and profile. The objects, libs and exes are named with the buildtype,
#   so 'make debug' creates for example 'nsh-d', profile '-p' and release ''.
#   The objectfiles should go to '$(N_OBJECTDIR)',so the source directorys stay
#   clean. You can build the diffent buildtypes without cleanup the last one,
#   because of the specific naming.
#   The make process is configured with config.mak. If needed change things there.
#
#   make == make release
#   make release: targetname+''     -D__RELEASE__
#   make debug  : targetname+'-d'   -D__DEBUG__
#   make profile: targetname+'-p'   -D__PROFILE__
#   
#   Is included into makemake.tcl.
#
#   TODO:
#   - handle target type: mll, dll, package
#   - handle the doxydoc genation
#   - support for win32 and macosx makefiles
#
#   22-Mar-2003 cubejk  created
#   30-Mar-2003 cubejk  updated
#--------------------------------------------------------------------
#Buildtype structs
set num_buildtypes 3
set btype(0,name) "release"
set btype(1,name) "debug"
set btype(2,name) "profile"

set btype(0,post) ""
set btype(1,post) "-d"
set btype(2,post) "-p"

set btype(0,define) "__RELEASE__"
set btype(1,define) "__DEBUG__"
set btype(2,define) "__PROFILE__"

#set btype(0, ) 
#set btype(1, ) 
#set btype(2, ) 

#--------------------------------------------------------------------
#helper functions to generate a string from a list of files and add $pre and $post
proc make_list {name_list pre post} {
    set ret ""
    foreach name $name_list {
        set ret "$ret $pre$name$post"
    }
    return $ret
}

#--------------------------------------------------------------------
proc make_post_list {name_list post} {
    set ret ""
    foreach name $name_list {
        set ret "$ret $name$post"
    }
    return $ret
}

#--------------------------------------------------------------------
proc make_pre_list {name_list pre} {
    set ret ""
    foreach name $name_list {
        set ret "$ret $pre$name"
    }
    return $ret
}

#--------------------------------------------------------------------
proc generate_obj_list { module } {
    
    set ret { }
    global mod
    
    set m [findmodbyname $module]
    
    foreach src $mod($m,files) {
        lappend ret $src
    }
    return $ret
}


#--------------------------------------------------------------------
#generate the makefile line for a static lib
#return   all depending modules
proc gen_lib_unix {target cid} {

    global tar
    global bt_name bt_post bt_def 
    global l_pre l_post o_pre o_post
    
    set t [findtargetbyname $target]
    
    #collect depend - lib
    set depend_list { }
    foreach depend $tar($t,depends) {
        lappend depend_list $depend
    }
    
    
    #collect modules and generate the obj files from this
    set module_list { }
    set obj_list { }
    foreach module $tar($t,mods) {
        lappend module_list $module
        #collect the object files form this module
        set temp_obj_list [generate_obj_list $module]
        foreach obj $temp_obj_list {
            if { (-1 == [lsearch obj_list obj]) } {
                    lappend obj_list $obj
            }
        }
    }
    
    #write entry
    puts $cid "$target$bt_post: $l_pre$target$l_post"
    puts $cid ""
    puts $cid "$l_pre$target$l_post: \
                  [make_list $depend_list $l_pre $l_post]\
                  [make_list $obj_list $o_pre $o_post]"
    
    puts $cid "\t\${AR} ru $l_pre$target$l_post\
              [make_list $obj_list $o_pre $o_post]"

    puts $cid ""
    
    #return all depending modules
    return $module_list
}


#--------------------------------------------------------------------
#generate the makefile line for a exe
#return   all depending modules
proc gen_exe_unix {target cid} {

    global tar
    global bt_name bt_post bt_def 
    global e_pre e_post l_pre l_post o_pre o_post
    
    set t [findtargetbyname $target]
    
    #collect depend
    set depend_list { }
    foreach depend $tar($t,depends) {
        lappend depend_list $depend
    }
    
    #collect unix libs
    set ulib_list { }
    foreach lib $tar($t,libs_unix) {
        lappend ulib_list $lib
    }
    
    #collect modules and generate the obj files from this
    set module_list { }
    set obj_list { }
    foreach module $tar($t,mods) {
        lappend module_list $module
        #collect the object files form this module
        set temp_obj_list [generate_obj_list $module]
        foreach obj $temp_obj_list {
            if { (-1 == [lsearch obj_list obj]) } {
                    lappend obj_list $obj
            }
        }
    }
    
    #write entry
    puts $cid "$target$bt_post: $e_pre$target$e_post"
    puts $cid ""
    puts $cid "$e_pre$target$e_post: [make_list $depend_list $l_pre $l_post] [make_list $obj_list $o_pre $o_post]"
    puts $cid "\t\$(CXX) \$(CXXFLAGS$bt_post) \$(INCDIR) \$(LIBDIR)\
                \$(SYM_OPT)$bt_def \
                \$(OUT_OPT) $e_pre$target$e_post\
                [make_list $obj_list $o_pre $o_post] \
                [make_pre_list $depend_list \$(LIB_OPT)] \
                \$(LIBS) [make_pre_list $ulib_list \$(LIB_OPT)]"

    puts $cid ""
    
    #return all depending modules
    return $module_list
}

#--------------------------------------------------------------------
#generate the makefile line for a obj
proc gen_obj_unix {module cid} {

    global mod
    global bt_name bt_post bt_def 
    global e_pre e_post l_pre l_post o_pre o_post
    global inc_dir
    
    set m [findmodbyname $module]
    
    set inc_dir "\$(INC_PATH)"
    set inc_dir_real "../inc/"
    set dir "$mod($m,dir)/"
    
    #set the export defines
    set init     "n_init_$module"
    set fini     "n_fini_$module"
    set new      "n_new_$module"
    set version  "n_version_$module"
    set initcmds "n_initcmds_$module"
    
    #collect header files
    set header_list { }
    set header_dep_list " "
    foreach header $mod($m,headers) {
        lappend header_list $header
        set filename "$inc_dir_real$dir$header.h"
        puts "-> dependencies for $filename"
        set header_dep_list [get_depends $filename $inc_dir_real $header_dep_list]
    }
    
    #collect source files
    set source_list { }
    foreach src $mod($m,files) {
        lappend source_list $src
    }
    
    #select the right compiler
    if { $mod($m,type) == "clib" } {
        set compiler "\$(CC)"
	set flags "\$(CFLAGS)"
     } else {
        set compiler "\$(CXX)"
	set flags "\$(CXXFLAGS)"
    }
    
    #write the single sources entrys thats not already done
    global done_objs
    foreach fsrc $source_list {
        if { (-1 == [lsearch done_objs fsrc]) } {
                    lappend done_objs $fsrc
            set filename "$dir$fsrc.cc"
        puts "-> dependencies for $filename"
        set dep_list [get_depends $filename $inc_dir_real ""]
        puts $cid "$o_pre$fsrc$o_post: $filename [make_pre_list $dep_list $inc_dir]"
        puts $cid "\t$compiler $flags$bt_post  \$(INCDIR) \
                    \$(SYM_OPT)$bt_def\
                    \$(SYM_OPT)N_INIT=$init \$(SYM_OPT)N_FINI=$fini \$(SYM_OPT)N_NEW=$new\
                    \$(SYM_OPT)N_VERSION=$version \$(SYM_OPT)N_INITCMDS=$initcmds\
                    \$(NOLINK_OPT) \$(OUT_OPT) $o_pre$fsrc$o_post $dir$fsrc.cc"
        puts $cid ""
        }
    }
}

#--------------------------------------------------------------------
#generate a makefile for the 3 build types release, debug and profile
#steps:
#   collect all targets by type
#   write the begin of the Makefile
#   loop: generate all targets and modules for each build type (release,debug,profile)
#      generate lib targets, collect depending modules
#      generate exe targets, collect depending modules
#      generate modules
#   write the end of the Makefile
#--------------------------------------------------------------------
proc gen_makefile { } {
    global RL_HOME
    
    global tar
    global num_tars
    global mod
    global num_mods
    
    global btype
    global num_buildtypes
    
    #collect targets, only lib and exe in the moment 
    set lib_target_list {}
    set exe_target_list {}
    for {set x 0} {$x < $num_tars} {incr x} {
        if { $tar($x,platform) == "all" || $tar($x,platform) == "unix"} {
            if { $tar($x,type) == "lib" } {
                lappend lib_target_list $tar($x,name)
            } elseif { $tar($x,type) == "exe" } {
                lappend exe_target_list $tar($x,name)
            } elseif { $tar($x,type) == "workspace" } {
                #empty - ignore workspace
            } else {
                puts stderr "ERROR: FIXME: target: $tar($x,name) type: $tar($x,type) Not handled !!!"
            }
        }
    }
    
    puts "Generate Makefile..."
    
    #write begin of the Makefile
    set cid [open Makefile w]

    puts $cid "#----------------------------------------------------------"
    puts $cid "# Makefile"
    puts $cid "# MACHINE GENERATED, DON'T EDIT!"
    puts $cid "#----------------------------------------------------------"
    
    puts $cid "include config.mak"
    
    puts $cid "all: release"
    
    puts $cid "dirs:"
    puts $cid "\ttest -d \$(N_OBJECTDIR) || mkdir \$(N_OBJECTDIR)"
    puts $cid "\ttest -d \$(N_TARGETDIR) || mkdir \$(N_TARGETDIR)"

    puts $cid "clean: \n\trm -f \$(N_OBJECTDIR)*\$(OBJ)\n\trm \$(N_TARGETDIR)*\$(LIB_POST)"
    
    #generate all targets and modules for each build type (release,debug,profile)
    for {set bt 0} {$bt < $num_buildtypes} { incr bt } {
        
        global bt_name bt_post bt_def 
        global e_pre e_post l_pre l_post o_pre o_post
        
        #setup the pre and post strings for the diffent targettypes
        set bt_name $btype($bt,name)
        set bt_post $btype($bt,post)
        set bt_def  $btype($bt,define)
        
        #exe
        set e_pre  "\$(N_TARGETDIR)"
        set e_post "$bt_post\$(EXE)"
        
        #lib
        set l_pre  "\$(N_TARGETDIR)\$(LIB_PRE)"
        set l_post "$bt_post\$(LIB_POST)"
        
        #obj
        set o_pre  "\$(N_OBJECTDIR)"
        set o_post "$bt_post\$(OBJ)"
        
        puts "-> Generate Buildtype:$bt_name..."
        
        puts $cid "#--- begin of: $bt_name --------------------------------------"
        puts $cid "$bt_name: dirs\
                [make_list $lib_target_list $l_pre $l_post] \
                [make_list $exe_target_list $e_pre $e_post]"
        
        puts $cid ""
        
        #clean the depending module list
        set mod_depend_list { }
        
        # generate all lib targets and collect all depended module
        foreach target $lib_target_list {
            puts "-> Generate lib target: $target$bt_post"
            set mod_dep_list [gen_lib_unix $target $cid]
            #collect all depending modules, uinque only
            foreach mod_dep $mod_dep_list {
                if { (-1 == [lsearch mod_depend_list depend]) } {
                    lappend mod_depend_list $mod_dep
                }
            }
        }
        
        # generate all exe targets and collect all depending modules
        foreach target $exe_target_list {
            puts "-> Generate exe target: $target$bt_post"
            set mod_dep_list [gen_exe_unix $target $cid]
            #collect all depending modules, uinque only
            foreach mod_dep $mod_dep_list {
                if { (-1 == [lsearch mod_depend_list depend]) } {
                    lappend mod_depend_list $mod_dep
                }
            }
        }
        
        #clean the list of already done objects
        global done_objs
        set done_objs { }
        
        #generate all depending modules
        foreach module $mod_depend_list {
            gen_obj_unix $module $cid
        }

        puts $cid "#--- end of: $bt_name --------------------------------------"
    }

    puts $cid "#----------------------------------------------------------"
    puts $cid "# Generate docs"
    puts $cid "#----------------------------------------------------------"
    puts $cid "doc:"
    puts $cid "\tpython \$(RL_HOME)/bin/autodoc.py"
    puts $cid "\tcd \$(RL_HOME)/code/nebula2/doxycfg; doxygen nebula2.cfg"

    #write end of Makefile
    puts $cid "#----------------------------------------------------------"
    puts $cid "# Makefile"
    puts $cid "# EOF"
    puts $cid "#----------------------------------------------------------"
    close $cid
}

#--------------------------------------------------------------------
#   EOF
#--------------------------------------------------------------------
