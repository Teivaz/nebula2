#--------------------------------------------------------------------
#   makeunix.tcl
#   ===========
#   Functions to generate Unix Makefiles
#
#   The makefile is generated for 3 buildtypes release/default, debug
#   and profile. The objects, libs and exes are named with the buildtype,
#   so 'make debug' creates for example 'nsh-d', profile '-p' and release ''.
#   The objectfiles should go to 'code/make/',so the source directorys stay
#   clean. You can build the diffent buildtypes without cleanup the last one,
#   because of the specific naming.
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
#   - handle the doxgenation
#   - support for win32 and macosx makefiles
#   - 
#
#   22-Mar-2002 cubejk  created
#
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
    global e_pre e_post l_pre l_post o_pre o_post
    
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
    puts $cid "$e_pre$target$e_post: \
                  [make_list $depend_list $l_pre $l_post]\
                  [make_list $obj_list $o_pre $o_post]"
    
    puts $cid "\t\$(CXX) \$(CFLAGS$bt_post) \$(INCDIR) \$(LIBDIR) \$(LIBS) \
                [make_list $depend_list \$(LIB_OPT)$l_pre $l_post] \
                \$(SYM_OPT)$bt_def \
                \$(OUT_OPT) $e_pre$target$e_post\
                [make_list $obj_list $o_pre $o_post]"

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
    set dir $mod($m,dir)
    
    #collect header files
    set header_list { }
    set header_dep_list " "
    foreach header $mod($m,headers) {
        lappend header_list $header
        set filename "$inc_dir_real$dir/$header.h"
        puts "-> dependencies for $filename"
        set header_dep_list [get_depends $filename $inc_dir_real $header_dep_list]
    }
    
    #collect source files
    set source_list { }
    foreach src $mod($m,files) {
        lappend source_list $src
    }
    
    if { $mod($m,type) == "clib" } {
        set compiler "\$(CC)"
    } else {
        set compiler "\$(CXX)"
    }
    
    #write the single sources entrys thats not already done
    global done_objs
    foreach fsrc $source_list {
        if { (-1 == [lsearch done_objs fsrc]) } {
                    lappend done_objs $fsrc
        set filename "$dir/$fsrc.cc"
        puts "-> dependencies for $filename"
        set dep_list [get_depends $filename $inc_dir_real ""]
        puts $cid "$o_pre$fsrc$o_post: $filename [make_pre_list $dep_list $inc_dir]"
        
        puts $cid "\t$compiler \$(CFLAGS$bt_post) \$(INCDIR) \
                    \$(SYM_OPT)$bt_def\
                    \$(NOLINK_OPT) \$(OUT_OPT) $o_pre$fsrc$o_post $dir/$fsrc.cc"
        puts $cid ""
        }
    }
}

#--------------------------------------------------------------------
#generate a makefile for the 3 build types release, debug and profile
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
            } else {
                puts "WARNING: FIXME: target: $tar($x,name) type: $tar($x,type) Not handled !!!"
            }
        }
    }
    
    puts "Generate Makefile..."
    
    set cid [open Makefile w]

    puts $cid "#----------------------------------------------------------"
    puts $cid "# Makefile"
    puts $cid "# MACHINE GENERATED, DON'T EDIT!"
    puts $cid "#----------------------------------------------------------"
    
    puts $cid "include config.mak"
    
    puts $cid "all: release"
    
    puts $cid "clean: \n\trm \$(N_OBJECTDIR)/*\$(OBJ)\n"
    
    #generate all build types (release,debug,profile)
    for {set bt 0} {$bt < $num_buildtypes} { incr bt } {
        
        global bt_name bt_post bt_def 
        #build type
        set bt_name $btype($bt,name)
        set bt_post $btype($bt,post)
        set bt_def  $btype($bt,define)
        
        global e_pre e_post l_pre l_post o_pre o_post
        #exe
        set e_pre  "\$(N_TARGETDIR)/"
        set e_post "$bt_post\$(EXE)"
        
        #lib
        set l_pre  "\$(N_TARGETDIR)/\$(LIB_PRE)"
        set l_post "$bt_post\$(LIB_POST)"
        
        #obj
        set o_pre  "\$(N_OBJECTDIR)/"
        set o_post "$bt_post\$(OBJ)"
        
        puts "Generate $bt_name..."
        
        puts $cid "#--- begin of: $bt_name --------------------------------------"
        puts $cid "$bt_name: \
                [make_list $lib_target_list $l_pre $l_post] \
                [make_list $exe_target_list $e_pre $e_post]"
        
        puts $cid ""
        
        set mod_depend_list { }
        # generate all lib targets and collect all depended module
        foreach target $lib_target_list {
            puts "Generate lib target: $target$bt_post"
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
            puts "Generate exe target: $target$bt_post"
            set mod_dep_list [gen_exe_unix $target $cid]
            #collect all depending modules, uinque only
            foreach mod_dep $mod_dep_list {
                if { (-1 == [lsearch mod_depend_list depend]) } {
                    lappend mod_depend_list $mod_dep
                }
            }
        }
        
        #generate all depending modules
        foreach module $mod_depend_list {
            gen_obj_unix $module $cid
        }

        puts $cid "#--- end of: $bt_name --------------------------------------"
    }

    puts $cid "#----------------------------------------------------------"
    puts $cid "# nebula.mak"
    puts $cid "# EOF"
    puts $cid "#----------------------------------------------------------"
    close $cid
}

#--------------------------------------------------------------------
#   EOF
#--------------------------------------------------------------------
