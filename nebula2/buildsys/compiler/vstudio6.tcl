#-----------------------------------------------------------------------------------------
#    makedsw.tcl
#
#  FIXME:  This needs severly cleaned up and finished to use all relevant
#          bits as an example for other compiler generator authors.
#  FIXME:  Make sure the functions are properly commented on.
#
#   Cobbled up from the original buildsys courtesy of Radon Labs and 
#   Leaf Garland's Nebula2 VC6 project files.
#   Copyright (c) 2003 Radon Labs
#
#   This file is licenesed under the terms of the Nebula License
#-----------------------------------------------------------------------------------------

#-----------------------------------------------------------------------------------------
#    Globals
#-----------------------------------------------------------------------------------------

set release_lib_flags "/nologo /GB /MD /W3 /GX- /Ot /Og /Oi /Oy /Ob2 /Gy /I \"..\\inc\" /D \"N_STATIC\" /D \"__WIN32__\" /D \"WIN32\" /D \"NDEBUG\" /D \"NT_PLUGIN\" /GF /TP /c /FD"

set debug_cpp_flags   "/nologo /MDd /W3 /ZI /Od /I \"..\\inc\" /D \"N_STATIC\" /D \"__WIN32__\" /D \"WIN32\" /D \"_DEBUG\" /D \"NT_PLUGIN\" /c /GX /FD"
set release_cpp_flags  "/nologo /G6 /GB /MD /W3 /Ot /Og /Oi /Oy /Ob2 /Gy /I \"..\\inc\" /D \"N_STATIC\" /D \"__WIN32__\" /D \"WIN32\" /D \"NDEBUG\" /D \"NT_PLUGIN\" /GF /c /GX /FD"
set neb_libpath_win32 "./code/nebula2/lib/win32_vc_i386"


#-----------------------------------------------------------------------------------------
# emit_files $name $cid
#   Spits out the 'group' segments of a .dsp file
#-----------------------------------------------------------------------------------------
proc emit_files {name cid} {
    global platform

    puts $cid "# Name \"$name - Win32 Debug\""
    puts $cid "# Name \"$name - Win32 Release\""
    puts $cid "# Begin Group \"$name\""
    puts $cid ""
    puts $cid "# PROP Default_Filter \"cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;cc\""

    #Prep work
    set smod_list [get_tarmods $name]

    foreach module $smod_list {

        puts $cid "# Begin Group \"$module\""
        puts $cid "";
        puts $cid "# PROP Default_Filter \"cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;cc\""

        if {[test_modplatform $module $platform]} {
            set filenames [get_modsources_dressed $module]

            foreach filename $filenames { 
                #build a new source entry

                set more_syms "/D N_INIT=n_init_$module /D N_FINI=n_fini_$module /D N_NEW=n_new_$module /D N_VERSION=n_version_$module /D N_INITCMDS=n_initcmds_$module"

                puts $cid "# Begin Source File"
                puts $cid ""

                puts $cid "SOURCE=[pathto [getfilenamewithextension $filename cc] ]"

                set cpp "/TP"
                if { [get_modtype $module] == "c" } { set cpp "/TC" }

                #build the flag set
                puts $cid ""
                puts $cid "!IF \"\$(CFG)\" == \"$name - Win32 Debug\""
                puts $cid ""
                puts $cid "# ADD CPP $more_syms $cpp /GZ"
                puts $cid ""
                puts $cid "!ELSEIF  \"\$(CFG)\" == \"$name - Win32 Release\""
                puts $cid ""
                puts $cid "# ADD CPP $more_syms $cpp"
                puts $cid ""
                puts $cid "!ENDIF "
                puts $cid ""
                puts $cid "# End Source File"

            }

            # add header files 
            set headers [get_modheaders_dressed $module]
            foreach header $headers {
                puts $cid "# Begin Source File"
                puts $cid "SOURCE=[pathto $header.h]"
                puts $cid "# End Source File"
            }

        puts $cid "# End Group"
        }
    }
    # resource file group
    if {[get_tartype $name] == "exe"} {
        puts $cid "# Begin Group \"resources\""
        puts $cid ""
        puts $cid "# PROP Default_Filter \"cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;cc\""
        puts $cid "# Begin Source File"
        puts $cid ""
        puts $cid "SOURCE=pkg/res_$name.rc"
        puts $cid "# End Source File"
        puts $cid "# End Group"
    }
    puts $cid "# End Group"
    puts $cid "# End Target"
    puts $cid "# End Project"
}

#-----------------------------------------------------------------------------------------
#  emit_dsp_header $name $cid type
#-----------------------------------------------------------------------------------------
proc emit_dsp_header {name cid type} {
    if {$type == "lib"} {
        set tagline "Static Library"
        set vernum  "0x0104"
    } elseif {$type == "dll"} {
        set tagline "Dynamic-Link Library"
        set vernum  "0x0102"
    } else {
        set tagline "Application"
        set vernum  "0x0101"
    }
 
    puts $cid "# Microsoft Developer Studio Project File - Name=\"$name\" - Package Owner=<4>"
    puts $cid "# Microsoft Developer Studio Generated Build File, Format Version 6.00"
    puts $cid "# ** DO NOT EDIT **"
    puts $cid ""
    puts $cid "# TARGTYPE \"Win32 (x86) $tagline\" $vernum"
    puts $cid ""
    puts $cid "CFG=$name - Win32 Debug"
    puts $cid "!MESSAGE This is not a valid makefile. To build this project using NMAKE,"
    puts $cid "!MESSAGE use the Export Makefile command and run"
    puts $cid "!MESSAGE "
    puts $cid "!MESSAGE NMAKE /f \"$name\"."
    puts $cid "!MESSAGE "
    puts $cid "!MESSAGE You can specify a configuration when running NMAKE"
    puts $cid "!MESSAGE by defining the macro CFG on the command line. For example:"
    puts $cid "!MESSAGE "
    puts $cid "!MESSAGE NMAKE /f \"$name\" CFG=\"$name - Win32 Debug\""
    puts $cid "!MESSAGE "
    puts $cid "!MESSAGE Possible choices for configuration are:"
    puts $cid "!MESSAGE "
    puts $cid "!MESSAGE \"$name - Win32 Release\" (based on \"Win32 (x86) $tagline\")"
    puts $cid "!MESSAGE \"$name - Win32 Debug\" (based on \"Win32 (x86) $tagline\")"
    puts $cid "!MESSAGE "
    puts $cid ""
    puts $cid "# Begin Project"
    puts $cid "# PROP AllowPerConfigDependencies 0"
    puts $cid "# PROP Scc_ProjName \"\""
    puts $cid "# PROP Scc_LocalPath \"\""
    puts $cid "CPP=cl.exe"
    puts $cid "MTL=midl.exe"
    puts $cid "RSC=rc.exe"
}

#-----------------------------------------------------------------------------------------
#  emit_dsp_settings $name $cid $use_debug
#-----------------------------------------------------------------------------------------
proc emit_dsp_settings {name cid use_debug} {
    variable debug_cpp_flags
    variable release_cpp_flags
    variable release_lib_flags
    global cur_workspacepath
    variable neb_libpath_win32

    #generate cpp_flags odir idir
    set inc_list "/I \"[join [get_incsearchdirs] "\" /I \""]\""
    set def_list ""
    foreach def [get_tardefs $name] {
        if {[llength [lindex $def 0]] && [llength [lindex $def 1]]} {
            append def_list "/D [lindex $def 0]=[lindex $def 1] "
        }
    }
    

    puts $cid ""
    
    if {$use_debug == 1} {
        puts $cid "!IF  \"\$(CFG)\" == \"$name - Win32 Debug\""    
        set idir [path_wspacetointer]/win32d
        set odir [path_wspacetooutput]/win32d
        set win32_libs [get_win32libs_debug $name]        
        if {[get_tartype $name] == "lib"} {
            set cpp_flags $debug_cpp_flags
        } else {
            set cpp_flags $debug_cpp_flags
        }
        set lib_path "/libpath:Debug"
    } else {
        puts $cid "!ELSEIF  \"\$(CFG)\" == \"$name - Win32 Release\""
        set idir [path_wspacetointer]/win32
        set odir [path_wspacetooutput]/win32
         set win32_libs [get_win32libs_release $name]        
        if {[get_tartype $name] == "lib"} {
            set cpp_flags $release_lib_flags
        } else {
            set cpp_flags $release_cpp_flags
        }
        set lib_path "/libpath:Release"
    }

    # add support for rtti and exceptions if desired
    if { [get_rtti $name] == "true" } {
        set cpp_flags "$cpp_flags /GR" 
    }
    if { [get_exceptions $name] == "true" } {
        set cpp_flags "$cpp_flags /EHsc"
    }
    
    puts $cid ""
    puts $cid "# PROP BASE Use_MFC 0"
    puts $cid "# PROP BASE Use_Debug_Libraries $use_debug"
    puts $cid "# PROP BASE Output_Dir \"$odir\""
    puts $cid "# PROP BASE Intermediate_Dir \"$idir/$name\""
    puts $cid "# PROP BASE Target_Dir \"\""
    puts $cid "# PROP Use_MFC 0"
    puts $cid "# PROP Use_Debug_Libraries $use_debug"
    puts $cid "# PROP Output_Dir \"$odir\""
    puts $cid "# PROP Intermediate_Dir \"$idir/$name\""
    puts $cid "# PROP Target_Dir \"\""
    puts $cid "# ADD BASE CPP $cpp_flags"
    puts $cid "# ADD CPP $cpp_flags $inc_list $def_list"
    
    if {[get_tartype $name] == "lib"} {
        puts $cid "MTL=midl.exe"
    } elseif {[get_tartype $name] == "dll"} {
        puts $cid "MTL=midl.exe"
    }
    
    puts $cid "# ADD BASE MTL /nologo /win32"
    puts $cid "# ADD MTL /nologo /win32"
    
    if {$use_debug == 1} {
        puts $cid "# ADD BASE RSC /l 0x409 /d \"_DEBUG\""
        puts $cid "# ADD RSC /l 0x409 /d \"_DEBUG\""
    } else {
        puts $cid "# ADD BASE RSC /l 0x409 /d \"NDEBUG\""
        puts $cid "# ADD RSC /l 0x409 /d \"NDEBUG\""
    }
    
    puts $cid "BSC32=bscmake.exe"
    puts $cid "# ADD BASE BSC32 /nologo"
    puts $cid "# ADD BSC32 /nologo"

    if {[get_tartype $name] == "lib" } {
        puts $cid "LIB32=link.exe -lib"
        puts $cid "# ADD BASE LIB32 $win32_libs /nologo $lib_path"
        puts $cid "# ADD LIB32 $win32_libs /nologo $lib_path /libpath:[findrelpath $cur_workspacepath $neb_libpath_win32]"
    } elseif {[get_tartype $name] == "dll"} {
        puts $cid "LINK32=link.exe"
        puts $cid "# ADD BASE LINK32 $win32_libs /nologo /dll /machine:I386 $lib_path"
        puts $cid "# ADD LINK32 $win32_libs /nologo /dll /machine:I386 $lib_path /libpath:[findrelpath $cur_workspacepath $neb_libpath_win32]"
    } else {
        set linkdbg ""
        if {$use_debug == 1} {
            set linkdbg "/debug "
        }
        puts $cid "LINK32=link.exe"
        puts $cid "# ADD BASE LINK32 user32.lib gdi32.lib advapi32.lib $win32_libs /nologo $linkdbg/machine:IX86 /pdbtype:sept $lib_path"
        puts $cid "# SUBTRACT BASE LINK32 /pdb:none"
        puts $cid "# ADD LINK32 user32.lib gdi32.lib advapi32.lib $win32_libs /nologo $linkdbg/machine:IX86 /pdbtype:sept $lib_path /libpath:[findrelpath $cur_workspacepath $neb_libpath_win32]"
        puts $cid "# SUBTRACT LINK32 /pdb:none"
    }
    
    if {$use_debug == 0} {
        puts $cid ""
        puts $cid "!ENDIF "
        puts $cid ""
        puts $cid "# Begin Target"
        puts $cid ""
    }
}

#-----------------------------------------------------------------------------------------
#   gen_lib_dsp $wspace_idx $tarname
#   Creates a .dsp file for static lib targets (console or window)
#-----------------------------------------------------------------------------------------
proc gen_lib_dsp {name} {
    global home
    global cur_workspacepath
    
    puts "Generate lib target: $name"

    # write .dsp file
    set cid [open [cleanpath $home/$cur_workspacepath/$name.dsp] w]

    emit_dsp_header $name $cid lib
    emit_dsp_settings $name $cid 1
    emit_dsp_settings $name $cid 0
    emit_files $name $cid
    
    close $cid
}

#-----------------------------------------------------------------------------------------
#   gen_dll_dsp $wspace_idx $tarname
#   Creates a .dsp file for dynamic lib targets (console or window)
#-----------------------------------------------------------------------------------------
proc gen_dll_dsp {name} {
    global home
    global cur_workspacepath
   
    puts "Generate dll target: $name"

    # write .dsp file
    set cid [open [cleanpath $home/$cur_workspacepath/$name.dsp] w]

    emit_dsp_header $name $cid dll
    emit_dsp_settings $name $cid 1
    emit_dsp_settings $name $cid 0
    emit_files $name $cid
   
    close $cid
}

#-----------------------------------------------------------------------------------------
#   gen_exe_dsp $name
#   Creates a .dsp file for app targets (console or window)
#-----------------------------------------------------------------------------------------
proc gen_exe_dsp {name} {
    global home
    global cur_workspacepath

    puts "Generate exe target: $name"
    
    # write .dsp file
    set cid [open [cleanpath $home/$cur_workspacepath/$name.dsp] w]

    emit_dsp_header $name $cid exe
    emit_dsp_settings $name $cid 1
    emit_dsp_settings $name $cid 0
    emit_files $name $cid

    close $cid
}


#-----------------------------------------------------------------------------------------
#   gen_dsw $wspace_idx
#   Creates a .dsw file
#-----------------------------------------------------------------------------------------
proc gen_dsw { name } {
    global platform
    global home
    global cur_workspacepath
    
    # write .dsw file
    set cid [open [cleanpath $home/$cur_workspacepath/$name.dsw] w]

    puts $cid "Microsoft Developer Studio Workspace File, Format Version 6.00"
    puts $cid "# WARNUNG: DIESE ARBEITSBEREICHSDATEI DARF NICHT BEARBEITET ODER GELÖSCHT WERDEN!"
    puts $cid ""

    #for each project in the list
    foreach target [get_targets] {

        # ignore any non-win32-packages - this was use above to build the projects
        if {![test_tarplatform $target $platform]} {
            puts $cid "###############################################################################"
            puts $cid ""
            puts $cid "Project: \"$target\"=.\\$target.dsp - Package Owner=<4>"
            puts $cid ""
            puts $cid "Package=<5>"
            puts $cid "\{\{\{"
            puts $cid "\}\}\}"
            puts $cid ""
            puts $cid "Package=<4>"
            puts $cid "\{\{\{"

            #for each project in the list - again
            foreach tardep [get_tardeps $target] {
                if {![test_tarplatform $tardep $platform]} {
                    puts $cid "    Begin Project Dependency"
                    puts $cid "    Project_Dep_Name $tardep"
                    puts $cid "    End Project Dependency"
                }
            }
            puts $cid "\}\}\}"
            puts $cid ""
        }
    }
    puts $cid "###############################################################################"
    puts $cid ""
    puts $cid "Global:"
    puts $cid ""
    puts $cid "Package=<5>"
    puts $cid "\{\{\{"
    puts $cid "\}\}\}"
    puts $cid ""
    puts $cid "Package=<3>"
    puts $cid "\{\{\{"
    puts $cid "\}\}\}"
    puts $cid ""
    puts $cid "###############################################################################"

    close $cid
}

#-----------------------------------------------------------------------------------------
#   gen_workspace 
#   Generates the .dsp and .dsw file for all workspaces
#-----------------------------------------------------------------------------------------
proc generate {} {
    
    set vstudiopath ./build/vstudio6
    set outputpath  ./bin
    set interpath   $vstudiopath/inter
    
    foreach workspace [get_workspaces]  {    
        puts "Generating vstudio workspace file $workspace.dsw..."

        # let the buildsys know which workspace we are currently working
        # with and what the default directories for that workspace should
        # be - default directories may be overridden - this particular
        # call also writes the pkg_XXX.cc files for the workspace out.
        use_workspace $workspace $vstudiopath $outputpath $interpath
        
        gen_dsw $workspace
        
        #create the project files
        foreach target [get_targets] {
            set t [get_tartype $target]
            
            # no dll target just yet
            if {$t == "lib"} {
                gen_lib_dsp $target
            } elseif {$t == "exe" } {
                gen_exe_dsp $target
            } elseif {$t == "dll" } {
                gen_dll_dsp $target
            } else {
                puts "ERROR: Unknown target type $t for target $target"
                exit
            }
        }
    }
}

#-----------------------------------------------------------------------------------------
#EOF
#-----------------------------------------------------------------------------------------
