#--------------------------------------------------------------------
#   makefile.tcl
#   ============
#   Functions to generate GNU Make files.
#   (C) 2000 A.Weissflog
#
#   01-Feb-2001     floh      added Doxygen support
#   07-Jan-2005     jurquhart initial conversion to nebula2 build sys
#   10-Jan-2005     jurquhart more or less compiles on linux now
#--------------------------------------------------------------------

#--------------------------------------------------------------------
#   Description of system:
#   One main Makefile is generated, which contains the base targets -
#   the "workspaces" of nebula. Each base target includes a set of
#   target (target.*.mak) files, which in themselves define the
#   underlying targets.
#--------------------------------------------------------------------
#
#--------------------------------------------------------------------
#   gen_module_make
#   Write the statements to compile the files that define a module
#   into the makefile.
#--------------------------------------------------------------------
proc gen_module_make {name cid} {
    set more_syms "\$(SYM_OPT)N_INIT=n_init_$name \$(SYM_OPT)N_NEW=n_new_$name \$(SYM_OPT)N_INITCMDS=n_initcmds_$name"
    
    switch -exact [get_modtype $name] {
        "c" { set compileas 1 }
        "cpp" { set compileas 2 }
        default { set compileas 0 }
    }
        
    # Plonk in build lines for each source
    foreach sourcefile [get_modsources_dressed $name] {
        set basename [join [list [lindex [split $sourcefile /] end] ""] ""]
        puts -nonewline $cid "\$(N_INTERDIR)$basename\$(OBJ): [pathto [getfilenamewithextension $sourcefile cc] ]"
        # NOTE: doesn't seem like we need to plonk in the headers...
        #foreach headerfile [get_modheaders_dressed $name] {
        #        puts -nonewline $cid "../../$headerfile.h "
        #}
            
        if { $compileas <= 1 } {
            puts $cid "\n\t\$(DO_COMPILE_C) $more_syms"
        } elseif { $compileas == 2 } {
            puts $cid "\n\t\$(DO_COMPILE_CC) $more_syms"
        }
    }
}

#--------------------------------------------------------------------
#   get_compiledfile
#   Returns a list of object files for the specified module
#--------------------------------------------------------------------
proc get_compiledfiles {module} {
    set obj_list ""
    foreach sourcefile [get_modsources $module] {
        addtolist obj_list "\$(N_INTERDIR)$sourcefile\$(OBJ)"
    }
    return $obj_list
}

#--------------------------------------------------------------------
#   gen_target_make
#   Generate *.mak file for given target.
#
#   TODO:
#   - Finish .res support
#   - Fix messy compiler statements
#--------------------------------------------------------------------
proc gen_target_make {name cid} {
    global home
    global cur_workspacepath
    global platform
    
    log::log debug "Generating target line for $name"

    # handle platform specific targets (unless non-specified)
    set needend 0
    if {![test_tarplatform $name "all"]} {
        if {[test_tarplatform $name "win32"]} {
            puts $cid "ifeq (\$(N_PLATFORM),__WIN32__)"
        } elseif {[test_tarplatform $name "linux"]} {
            puts $cid "ifeq (\$(N_PLATFORM),__LINUX__)"
        } elseif {[test_tarplatform $name "macosx"]} {
            puts $cid "ifeq (\$(N_PLATFORM),__MACOSX__)"
        }
        set needend 1
    }
    
    set tartype [get_tartype $name]

    set files_win32  ""
    set files_linux  ""
    set files_macosx ""
    # Add modules we need to the files line
    foreach module [get_tarmods $name] {
        if {[llength [get_modsources $module]] > 0} {
            # Module has sources, onto the platform...
            if {![test_modplatform $module "all"]} {
                if {[test_modplatform $module "win32"]} {
                    addtolist files_win32 [get_compiledfiles $module]
                } elseif {[test_modplatform $module "linux"]} {
                    addtolist files_linux [get_compiledfiles $module]
                } elseif {[test_modplatform $module "macosx"]} {
                    addtolist files_macosx [get_compiledfiles $module]
                } else {
                    log::log error "Module \"$module\" not found on any regular platform! RUN!!!"
                }
            } else {
                addtolist files_win32  [get_compiledfiles $module]
                addtolist files_linux  [get_compiledfiles $module]
                addtolist files_macosx [get_compiledfiles $module]
            }
        }
    }

    # handle resource files
    set rsrc_files ""
    if {$tartype == "exe" || $tartype == "dll"} {
        # add standard nebula rsrc to exe
        if {$tartype == "exe"} {
            puts $cid "ifeq (\$(N_PLATFORM),__WIN32__)"
            puts $cid  "./pkg/build/pkg/res_$name.res\: ../../build/pkg/res_$name.rc"
            puts $cid "endif"
            addtolist rsrc_files "./pkg/res_$name.res"
        }

        # add any custom rsrc files
        set resfile [get_win32resource $name]
        if { $resfile != "" } {
            global cur_workspacepath
            global home
            global mod

            set startpath [string trim $home '/']
            set i [findmodbyname $name]
            set resfile [findrelpath $cur_workspacepath $startpath/code/$mod($i,trunkdir)/res/$resfile.rc]
            # addtolist rsrc_files "../build/pkg/res_$name.res"
            log::log debug "TODO: extra resource $name"
        }
    }

    # Get a list of libs to plonk on command
    set libs_win32 ""
    set libs_linux  ""
    set libs_macosx ""
    foreach lib [get_win32libs_release $name] {
        addtolist libs_win32 "\$(LIB_OPT)$lib"
    }
    foreach lib [get_linuxlibs $name] {
        addtolist libs_linux "\$(LIB_OPT)$lib"
    }
    foreach lib [get_osxlibs $name] {
        addtolist libs_macosx "\$(LIB_OPT)$lib"
    }
    
    # generate list of target dependencies for that target
    set dep_win32 ""
    set dep_linux  ""
    set dep_macosx ""
    foreach dep [get_tardeps $name] {
        if {[get_tartype $dep] == "dll"} {
            set targname "\$(LIB_OPT)$dep"
        } elseif {[get_tartype $dep] == "lib"} {
            set targname "\$(LIB_OPT)$dep"
        } else {
            log::log error "Target depends on an executable ($name, $dep) ?"
            set targname ""
        }
        if {![test_tarplatform $dep "all"]} {
            if {[test_tarplatform $dep "win32"]} {
                addtolist dep_win32 $dep
                addtolist libs_win32 $targname
            } elseif {[test_tarplatform $dep "linux"]} {
                addtolist dep_linux $dep
                addtolist libs_linux $targname
            } elseif {[test_tarplatform $dep "macosx"]} {
                addtolist dep_macosx $dep
                addtolist libs_macosx $targname
            } else {
                log::log error "Dependancy \"$dep\" not found on any regular platform! RUN!!!"
            }
        } else {
            addtolist dep_win32  $dep
            addtolist libs_win32 $targname
            addtolist dep_linux  $dep
            addtolist libs_linux $targname
            addtolist dep_macosx $dep
            addtolist libs_macosx $targname
        } 
    }

    # compile target itself
    # TODO: fix erroneous "only __VC__ must be win32" assumption
    if {$tartype == "dll"} {
        set t "\$(DLL_PRE)$name\$(DLL_POST)"
        set tarpath "\$(N_TARGETDIR)$t"
        puts $cid "ifeq (\$(N_COMPILER),__VC__)"
        puts $cid "$name : $dep_win32 $tarpath"
        puts $cid "$tarpath : $files_win32 $rsrc_files"
        puts $cid "\t\$(LD) \$^ /OUT:\$@ \$(LFLAGS) \$(LIBDIR) \$(LIBS) $libs_win32 /DLL"
        puts $cid "else"
        puts $cid "ifeq (\$(N_PLATFORM),__MACOSX__)"
        set t2 "\$(DLL_PRE)$name.dylib"
        set tarpath2 "\$(N_TARGETDIR)$t2"
        puts $cid "$name : $dep_macosx $tarpath2"
        puts $cid "$tarpath2 : $files_macosx"
        puts $cid "\t\$(CXX) -dynamiclib -compatibility_version 0.1 -current_version 0.1.0 \$^ -o \$@ \$(CFLAGS) \$(LIBDIR) \$(LIBS) $libs_macosx"
        puts $cid "else"
        puts $cid "$name : $dep_linux $tarpath"
        puts $cid "$tarpath : $files_linux"
        puts $cid "\t\$(CXX) \$^ -o \$@ \$(CFLAGS) \$(LIBDIR) \$(LIBS) $libs_linux -shared"
        puts $cid "endif"
        puts $cid "endif"
    } elseif {$tartype == "exe"} {
        set t "$name\$(EXE)"
        set tarpath "\$(N_TARGETDIR)$t"
        puts $cid "ifeq (\$(N_COMPILER),__VC__)"
        puts $cid "$name : $dep_win32 $tarpath"
        puts $cid "$tarpath : $files_linux $rsrc_files"
        puts $cid "\t\$(LD) \$^ /OUT:\$@ \$(LFLAGS) \$(LIBDIR) \$(LIBS) $libs_win32"
        puts $cid "else"
        puts $cid "ifeq (\$(N_PLATFORM),__MACOSX__)"
        puts $cid "$name : $dep_macosx $tarpath"
        puts $cid "$tarpath : $files_macosx"
        puts $cid "\t\$(CXX) \$^ -o \$@ \$(LIBDIR) \$(LIBS) $libs_macosx -flat_namespace"
        puts $cid "else"
        puts $cid "$name : $dep_linux $tarpath"
        puts $cid "$tarpath : $files_linux"
        puts $cid "\t\$(CXX) \$^ -o \$@ \$(LIBDIR) \$(LIBS) $libs_linux"
        puts $cid "endif"
        puts $cid "endif"
    } elseif {$tartype == "lib"} {
        # NOTE: previously known as a "package"
        # TODO: need to fix the MACOSX options...
        set t "\$(LIB_PRE)$name\$(LIB_POST)"
        set tarpath "\$(N_TARGETDIR)$t"
        puts $cid "ifeq (\$(N_COMPILER),__VC__)"
        puts $cid "$name : $dep_win32 $tarpath"
        puts $cid "$tarpath : $files_win32 $rsrc_files"
        puts $cid "\t\$(AR) \$^ /OUT:\$@ \$(LFLAGS) \$(LIBDIR) \$(LIBS) $libs_win32"
        puts $cid "else"
        puts $cid "ifeq (\$(N_PLATFORM),__MACOSX__)"
        puts $cid "$name : $dep_macosx $tarpath"
        puts $cid "$tarpath : $files_macosx"
        puts $cid "\t\$(CXX) -dynamiclib -compatibility_version 0.1 -current_version 0.1.0 \$^ -o \$@ \$(CFLAGS) \$(LIBDIR) \$(LIBS) $libs_macosx"
        puts $cid "$tarpath : packages/${name}_bundle\$(OBJ)"
        puts $cid "\t\$(CXX) \$^ -o \$@ \$(CFLAGS) \$(LIBDIR) \$(LIBS) \$(LIB_OPT)$name $libs_macosx -bundle -flat_namespace"
        puts $cid "else"
        puts $cid "$name : $dep_linux $tarpath"
        puts $cid "$tarpath : $files_linux"
        puts $cid "\t\$(AR) -cr \$@ \$^"
        puts $cid "endif"
        puts $cid "endif"
    } elseif {$tartype == "mll"} {
        set t "\$(DLL_PRE)$name.mll"
        set tarpath "\$(N_TARGETDIR)$t"
        puts $cid "ifeq (\$(N_COMPILER),__VC__)"
        puts $cid "$name : $dep_win32 $tarpath"
        puts $cid "$tarpath : $files_linux $rsrc_files"
        puts $cid "\t\$(LD) \$^ /OUT:\$@ \$(LFLAGS) \$(LIBDIR) \$(LIBS) $libs_win32 /DLL /export:initializePlugin /export:uninitializePlugin"
        puts $cid "endif"
    }

    # handle platform specific targets
    if {$needend == 1} {
        puts $cid "endif\n"
    } else {
        puts $cid ""
    }
}

#--------------------------------------------------------------------
#   gen_makefile
#   Generate a Makefile for a list of targets.
#--------------------------------------------------------------------
proc gen_makefile {name list_of_targets} {
    global home
    global cur_workspacepath
    global platform

    # Generate a platform specific global dependency list
    set tarlist_win32   ""
    set tarlist_linux   ""
    set tarlist_macosx  ""
    set list_of_modules ""
    foreach target [$list_of_targets] {
        if {![test_tarplatform $target "all"]} {
            if {[test_tarplatform $target "win32"]} {
                addtolist tarlist_win32 $target
            } elseif {[test_tarplatform $target "linux"]} {
                addtolist tarlist_linux $target
            } elseif {[test_tarplatform $target "macosx"]} {
                addtolist tarlist_macosx $target
            } else {
                log::log error "Target \"$target\" not found on any regular platform! PANIC!!!"
            }
        } else {
            addtolist tarlist_win32  $target
            addtolist tarlist_linux  $target
            addtolist tarlist_macosx $target
        }
        
        # Add this target's modules to a temp list (if it has source files)
        foreach module [get_tarmods $target] {
            # Only add if we have source files to compile
            if {[llength [get_modsources $module]] > 0} {
                lappend list_of_modules $module
            }
        }
    }
    
    set list_of_modules [lsort -unique $list_of_modules]

    set cid [open $home/$cur_workspacepath/$name.mak w]

    puts $cid "#--------------------------------------------------------------------"
    puts $cid "#    $name.mak"
    puts $cid "#    MACHINE GENERATED, DON'T EDIT!"
    puts $cid "#--------------------------------------------------------------------"
    puts $cid "include ../../buildsys/config.mak"
    puts $cid "BASECFLAGS += \$(EXTRAS)"
    puts $cid ""
    puts $cid "ifeq (\$(N_PLATFORM),__WIN32__)"
    puts $cid "$name: $tarlist_win32"
    puts $cid "endif"
    puts $cid "ifeq (\$(N_PLATFORM),__LINUX__)"
    puts $cid "$name: $tarlist_linux"
    puts $cid "endif"
    puts $cid "ifeq (\$(N_PLATFORM),__MACOSX__)"
    puts $cid "$name: $tarlist_macosx"
    puts $cid "endif"
    puts $cid ""
    
    # Important compiling statements
    puts $cid "DO_COMPILE_C  = \$(CC) \$< \$(OBJ_OPT)\$@ \$(NOLINK_OPT) \$(CFLAGS)"
    puts $cid "DO_COMPILE_CC = \$(CXX) \$< \$(OBJ_OPT)\$@ \$(NOLINK_OPT) \$(CXXFLAGS)"
    
    # First, we need to print out all the possible modules
    foreach module $list_of_modules {
        gen_module_make $module $cid
    }
    
    # Now we can do the targets 
    foreach target [$list_of_targets] {
        gen_target_make $target $cid
    }
    puts $cid ""
}

#-------------------------------------------------------------------------------
#   generate
#   Generates the main Makefile and adds targets for each workspace
#-------------------------------------------------------------------------------
proc generate { wslist } {

    set makepath ./build/makefile
    set outputpath  ./bin
    set interpath   $makepath/inter

    # list of required targets - makefile's
    set targets ""
    set cid [open [cleanpath $makepath/Makefile] w]
    
    puts $cid "# Makefile for nebula"
    puts $cid "include ../../buildsys/config.mak\n"
    
    foreach workspace [get_workspaces $wslist]  {
        # let the buildsys know which workspace we are currently working
        # with and what the default directories for that workspace should
        # be - default directories may be overridden - this particular
        # call also writes the pkg_XXX.cc files for the workspace out.
        use_workspace $workspace $makepath $outputpath $interpath

        # Calculate these once for the workspace
        set inc_list [get_incsearchdirs]
        set lib_list [get_libsearchdirs]
        
        ::log::log info "Generating workspace makefile $workspace.mak..."
        
        # Add in default target
        puts $cid "$workspace: "
        
        # Add inc_list and lib_list to an EXTRAS variable, passing onto the workspace makefile
        puts -nonewline $cid "\t@$\(MAKE) -f ./$workspace.mak EXTRAS=\""
        foreach include $inc_list {
            puts -nonewline $cid "$\(IPATH_OPT)$include "
        }
        foreach libpath $lib_list {
            puts -nonewline $cid "$\(LPATH_OPT)$libpath "
        }
        puts $cid "\"\n"
        
        # Ggenerate a .mak for this workspace
        gen_makefile $workspace get_targets
    }
    
    puts -nonewline $cid "all: "
    foreach workspace [get_workspaces $wslist]  {
        puts -nonewline $cid "$workspace "
    }
    puts $cid "\n"
    
    puts $cid "clean: "
    puts $cid "\t\$(RM) \$(N_TARGETDIR)*"
    puts $cid "\t\$(RM) \$(N_INTERDIR)*"
    
    puts $cid "default: all"
    
    # Add appropriate .PHONY targets
    puts -nonewline $cid  ".PHONY: all "
    foreach workspace [get_workspaces $wslist]  {
        puts -nonewline $cid "$workspace "
    }
    puts $cid "\n"
    
    close $cid
}

#-------------------------------------------------------------------------------
#   description
#   Return a description of this generator.
#-------------------------------------------------------------------------------
proc description { } {
    return "Support for Makefile using GNU make."
}

#--------------------------------------------------------------------
#   EOF
#--------------------------------------------------------------------

