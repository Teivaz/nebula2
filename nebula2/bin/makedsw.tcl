#-----------------------------------------------------------------------------------------
#    makedsw.tcl
#    This was completely cobbled up based on the Neb1 makedsw.tcl
#    and Leaf Garland's vc6 projects.  Many thanks to Radon Labs and Leaf.
#
#    To prevent a problem with unneeded and missing headers from cluttering
#    the output window - do a batch clean before compiliing.  vc6 will
#    remember the missing files in the future and not display them.
#-----------------------------------------------------------------------------------------

#-----------------------------------------------------------------------------------------
#    Globals
#-----------------------------------------------------------------------------------------
set release_binPrefix "..\\..\\..\\bin\\win32"
set debug_binPrefix "..\\..\\..\\bin\\win32d"

set release_cpp_flags "/nologo /G6 /GB /MD /W3 /Ot /Og /Oi /Oy /Ob2 /Gy /I \"..\\inc\" /D \"N_STATIC\" /D \"__WIN32__\" /D \"WIN32\" /D \"NDEBUG\" /D \"NT_PLUGIN\" /GF /c /GX"
set debug_cpp_flags "/nologo /MDd /W3 /ZI /Od /I \"..\\inc\" /D \"N_STATIC\" /D \"__WIN32__\" /D \"WIN32\" /D \"_DEBUG\" /D \"NT_PLUGIN\" /c /GX"
set release_dswlibpath "/libpath:..\\lib\\win32_vc_i386 /libpath:Release"
set debug_dswlibpath "/libpath:..\\lib\\win32_vc_i386 /libpath:Debug"

#-----------------------------------------------------------------------------------------
#	gen_libs $name
#   preps the lib list for use
#-----------------------------------------------------------------------------------------
proc gen_libs { name } {
	global win32_libs
	global debug_win32_libs
	global release_win32_libs

    # create list of link libs (win32 libs)
    set win32_libs [extract_libs $name libs_win32]
    set debug_win32_libs [extract_libs $name libs_win32_debug]
    set release_win32_libs [extract_libs $name libs_win32_release]
}

#-----------------------------------------------------------------------------------------
#	spit_app_header $name $cid
#   Spits out a standard lib project .dsp file header
#-----------------------------------------------------------------------------------------
proc spit_lib_header { name cid } {
	puts $cid "# Microsoft Developer Studio Project File - Name=\"$name\" - Package Owner=<4>"
	puts $cid "# Microsoft Developer Studio Generated Build File, Format Version 6.00"
	puts $cid "# ** DO NOT EDIT **"
	puts $cid ""
	puts $cid "# TARGTYPE \"Win32 (x86) Static Library\" 0x0104"
	puts $cid ""
	puts $cid "CFG=$name - Win32 Debug"
	puts $cid "!MESSAGE This is not a valid makefile. To build this project using NMAKE,"
	puts $cid "!MESSAGE use the Export Makefile command and run"
	puts $cid "!MESSAGE "
	puts $cid "!MESSAGE NMAKE /f \"$name.mak\"."
	puts $cid "!MESSAGE "
	puts $cid "!MESSAGE You can specify a configuration when running NMAKE"
	puts $cid "!MESSAGE by defining the macro CFG on the command line. For example:"
	puts $cid "!MESSAGE "
	puts $cid "!MESSAGE NMAKE /f \"$name.mak\" CFG=\"$name - Win32 Debug\""
	puts $cid "!MESSAGE "
	puts $cid "!MESSAGE Possible choices for configuration are:"
	puts $cid "!MESSAGE "
	puts $cid "!MESSAGE \"$name - Win32 Debug\" (based on \"Win32 (x86) Static Library\")"
	puts $cid "!MESSAGE \"$name - Win32 Release\" (based on \"Win32 (x86) Static Library\")"
	puts $cid "!MESSAGE "
	puts $cid ""
	puts $cid "# Begin Project"
	puts $cid "# PROP AllowPerConfigDependencies 0"
	puts $cid "# PROP Scc_ProjName \"\""
	puts $cid "# PROP Scc_LocalPath \"\""
	puts $cid "CPP=cl.exe"
	puts $cid "RSC=rc.exe"
}

#-----------------------------------------------------------------------------------------
#	spit_app_header $name $cid
#   Spits out a standard app project .dsp file header
#-----------------------------------------------------------------------------------------
proc spit_app_header { name cid } {
	puts $cid "# Microsoft Developer Studio Project File - Name=\"$name\" - Package Owner=<4>"
	puts $cid "# Microsoft Developer Studio Generated Build File, Format Version 6.00"
	puts $cid "# ** DO NOT EDIT **"
	puts $cid ""
	puts $cid "# TARGTYPE \"Win32 (x86) Application\" 0x0101"
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
	puts $cid "!MESSAGE \"$name - Win32 Release\" (based on \"Win32 (x86) Application\")"
	puts $cid "!MESSAGE \"$name - Win32 Debug\" (based on \"Win32 (x86) Application\")"
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
#	spit_d_lib_cpp $name $cid
#   Spits out the debug project settings for a static lib
#-----------------------------------------------------------------------------------------
proc spit_d_lib_cpp { name cid } {
	global win32_libs
	global debug_win32_libs
	global release_win32_libs
	global debug_cpp_flags
	global debug_binPrefix
	global debug_dswlibpath

	puts $cid ""
	puts $cid "!IF  \"\$(CFG)\" == \"$name - Win32 Debug\""
	puts $cid ""
	puts $cid "# PROP BASE Use_MFC 0"
	puts $cid "# PROP BASE Use_Debug_Libraries 1"
	puts $cid "# PROP BASE Output_Dir \".\\Debug\""
	puts $cid "# PROP BASE Intermediate_Dir \".\\Debug\\$name\""
	puts $cid "# PROP BASE Target_Dir \"\""
	puts $cid "# PROP Use_MFC 0"
	puts $cid "# PROP Use_Debug_Libraries 1"
	puts $cid "# PROP Output_Dir \".\\Debug\""
	puts $cid "# PROP Intermediate_Dir \".\\Debug\\$name\""
	puts $cid "# PROP Target_Dir \"\""
	puts $cid "MTL=midl.exe"
	puts $cid "# ADD BASE MTL /nologo /win32"
	puts $cid "# ADD MTL /nologo /win32"
	#puts $cid "# ADD BASE CPP /nologo /MTd /W3 /GX /ZI /Od /I \"..\\inc\" /D \"N_STATIC\" /D \"__WIN32__\" /D \"WIN32\" /D \"_DEBUG\" /D \"NT_PLUGIN\" /TP /GZ /c"
	#puts $cid "# ADD CPP /nologo /MDd /W3 /GX- /ZI /Od /I \"..\\inc\" /D \"N_STATIC\" /D \"__WIN32__\" /D \"WIN32\" /D \"_DEBUG\" /D \"NT_PLUGIN\" /TP /GZ /c"
	puts $cid "# ADD BASE CPP $debug_cpp_flags"
	puts $cid "# ADD CPP $debug_cpp_flags"
	puts $cid "# ADD BASE RSC /l 0x409 /d \"_DEBUG\""
	puts $cid "# ADD RSC /l 0x409 /d \"_DEBUG\""
	puts $cid "BSC32=bscmake.exe"
	puts $cid "# ADD BASE BSC32 /nologo"
	puts $cid "# ADD BSC32 /nologo"
	puts $cid "LIB32=link.exe -lib"
	puts $cid "# ADD BASE LIB32 $win32_libs $debug_win32_libs /nologo /out:\".\\Debug\\d_$name.lib\" $debug_dswlibpath "
	puts $cid "# ADD LIB32 $win32_libs $debug_win32_libs /nologo /out:\".\\Debug\\d_$name.lib\" $debug_dswlibpath "
	puts $cid ""
}

#-----------------------------------------------------------------------------------------
#	spit_r_lib $name $cid
#   Spits out the release project settings for static libs
#-----------------------------------------------------------------------------------------
proc spit_r_lib_cpp { name cid } {
	global win32_libs
	global debug_win32_libs
	global release_win32_libs
	global release_cpp_flags
	global release_bin_Prefix
	global release_dswlibpath

	puts $cid "!ELSEIF  \"\$(CFG)\" == \"$name - Win32 Release\""
	puts $cid ""
	puts $cid "# PROP BASE Use_MFC 0"
	puts $cid "# PROP BASE Use_Debug_Libraries 0"
	puts $cid "# PROP BASE Output_Dir \".\\Release\""
	puts $cid "# PROP BASE Intermediate_Dir \".\\Release\\$name\""
	puts $cid "# PROP BASE Target_Dir \"\""
	puts $cid "# PROP Use_MFC 0"
	puts $cid "# PROP Use_Debug_Libraries 0"
	puts $cid "# PROP Output_Dir \".\\Release\""
	puts $cid "# PROP Intermediate_Dir \".\\Release\\$name\""
	puts $cid "# PROP Target_Dir \"\""
	puts $cid "MTL=midl.exe"
	puts $cid "# ADD BASE MTL /nologo /win32"
	puts $cid "# ADD MTL /nologo /win32"
	#puts $cid "# ADD BASE CPP /nologo /G6 /MT /W3 /GX /Ot /Og /Oi /Oy /Ob2 /Gy /I \"..\\inc\" /D \"N_STATIC\" /D \"__WIN32__\" /D \"WIN32\" /D \"NDEBUG\" /D \"NT_PLUGIN\" /GF /TP /c"
	#puts $cid "# ADD CPP /nologo /GB /MD /W3 /GX- /Ot /Og /Oi /Oy /Ob2 /Gy /I \"..\\inc\" /D \"N_STATIC\" /D \"__WIN32__\" /D \"WIN32\" /D \"NDEBUG\" /D \"NT_PLUGIN\" /GF /TP /c"
	puts $cid "# ADD BASE CPP $release_cpp_flags"
	puts $cid "# ADD CPP $release_cpp_flags"
	puts $cid "# ADD BASE RSC /l 0x409 /d \"NDEBUG\""
	puts $cid "# ADD RSC /l 0x409 /d \"NDEBUG\""
	puts $cid "BSC32=bscmake.exe"
	puts $cid "# ADD BASE BSC32 /nologo"
	puts $cid "# ADD BSC32 /nologo"
	puts $cid "LIB32=link.exe -lib"
	puts $cid "# ADD BASE LIB32 $win32_libs $release_win32_libs /nologo $release_dswlibpath"
	puts $cid "# ADD LIB32 $win32_libs $release_win32_libs /nologo $release_dswlibpath"
	puts $cid ""
	puts $cid "!ENDIF "
	puts $cid ""
	puts $cid "# Begin Target"
	puts $cid ""
}

#-----------------------------------------------------------------------------------------
#	spit_d_exe_cpp $name $cid
#   Spits out the debug app project settings segment
#-----------------------------------------------------------------------------------------
proc spit_d_exe_cpp { name cid } {
	global win32_libs
	global debug_win32_libs
	global release_win32_libs
	global debug_binPrefix
	global debug_cpp_flags
	global debug_dswlibpath

	puts $cid ""
	puts $cid "!IF  \"\$(CFG)\" == \"$name - Win32 Debug\""
	puts $cid ""
	puts $cid "# PROP BASE Use_MFC 0"
	puts $cid "# PROP BASE Use_Debug_Libraries 1"
	puts $cid "# PROP BASE Output_Dir \"$debug_binPrefix\""
	puts $cid "# PROP BASE Intermediate_Dir \".\\Debug\\$name\""
	puts $cid "# PROP BASE Target_Dir \"\""
	puts $cid "# PROP Use_MFC 0"
	puts $cid "# PROP Use_Debug_Libraries 1"
	puts $cid "# PROP Output_Dir \"$debug_binPrefix\""
	puts $cid "# PROP Intermediate_Dir \".\\Debug\\$name\""
	puts $cid "# PROP Target_Dir \"\""
	puts $cid "# ADD BASE CPP $debug_cpp_flags"
	puts $cid "# ADD CPP $debug_cpp_flags"
	puts $cid "# ADD BASE MTL /nologo /win32"
	puts $cid "# ADD MTL /nologo /win32"
	puts $cid "# ADD BASE RSC /l 0x409 /d \"_DEBUG\""
	puts $cid "# ADD RSC /l 0x409 /d \"_DEBUG\""
	puts $cid "BSC32=bscmake.exe"
	puts $cid "# ADD BASE BSC32 /nologo"
	puts $cid "# ADD BSC32 /nologo"
	puts $cid "LINK32=link.exe"
	puts $cid "# ADD BASE LINK32 user32.lib gdi32.lib advapi32.lib $win32_libs $debug_win32_libs /nologo /debug /machine:IX86 /pdbtype:sept $debug_dswlibpath "
	puts $cid "# SUBTRACT BASE LINK32 /pdb:none"
	puts $cid "# ADD LINK32 user32.lib gdi32.lib advapi32.lib $win32_libs $debug_win32_libs /nologo /debug /machine:IX86 /pdbtype:sept $debug_dswlibpath "
	puts $cid "# SUBTRACT LINK32 /pdb:none"
	puts $cid ""
}

#-----------------------------------------------------------------------------------------
#	spit_r_exe_cpp $name $cid
#   Spits out the release app project settings segment
#-----------------------------------------------------------------------------------------
proc spit_r_exe_cpp { name cid } {
	global win32_libs
	global debug_win32_libs
	global release_win32_libs
	global release_binPrefix
	global release_cpp_flags
	global release_dswlibpath

	puts $cid "!ELSEIF  \"\$(CFG)\" == \"$name - Win32 Release\""
	puts $cid ""
	puts $cid "# PROP BASE Use_MFC 0"
	puts $cid "# PROP BASE Use_Debug_Libraries 0"
	puts $cid "# PROP BASE Output_Dir \"$release_binPrefix\""
	puts $cid "# PROP BASE Intermediate_Dir \".\\Release\\$name\""
	puts $cid "# PROP BASE Target_Dir \"\""
	puts $cid "# PROP Use_MFC 0"
	puts $cid "# PROP Use_Debug_Libraries 0"
	puts $cid "# PROP Output_Dir \"$release_binPrefix\""
	puts $cid "# PROP Intermediate_Dir \".\\Release\\$name\""
	puts $cid "# PROP Target_Dir \"\""
	puts $cid "# ADD BASE CPP $release_cpp_flags"
	puts $cid "# ADD CPP $release_cpp_flags"
	puts $cid "# ADD BASE MTL /nologo /win32"
	puts $cid "# ADD MTL /nologo /win32"
	puts $cid "# ADD BASE RSC /l 0x409 /d \"NDEBUG\""
	puts $cid "# ADD RSC /l 0x409 /d \"NDEBUG\""
	puts $cid "BSC32=bscmake.exe"
	puts $cid "# ADD BASE BSC32 /nologo"
	puts $cid "# ADD BSC32 /nologo"
	puts $cid "LINK32=link.exe"
	puts $cid "# ADD BASE LINK32 user32.lib gdi32.lib advapi32.lib $win32_libs $release_win32_libs /nologo /machine:IX86 /pdbtype:sept  $release_dswlibpath"
	puts $cid "# SUBTRACT BASE LINK32 /pdb:none"
	puts $cid "# ADD LINK32 user32.lib gdi32.lib advapi32.lib $win32_libs $release_win32_libs /nologo /machine:IX86 /pdbtype:sept  $release_dswlibpath"
	puts $cid "# SUBTRACT LINK32 /pdb:none"
	puts $cid ""
	puts $cid "!ENDIF "
	puts $cid ""
	puts $cid "# Begin Target"
	puts $cid ""
}

#-----------------------------------------------------------------------------------------
#	spit_dummy $cid
#   Spits out a dummy.cc segment to get the libs to compile
#-----------------------------------------------------------------------------------------
proc spit_dummy { cid } {
	puts $cid "# Begin Source File"
	puts $cid ""
	puts $cid "SOURCE=..\\src\\dummy.cc"
	puts $cid "DEP_CPP_DUMMY=\\"
	puts $cid "	\"..\\inc\\kernel\\ndebug.h\"\\"
	puts $cid "	\"..\\inc\\kernel\\ndefclass.h\"\\"
	puts $cid "	\"..\\inc\\kernel\\nsystem.h\"\\"
	puts $cid "	\"..\\inc\\kernel\\ntypes.h\"\\"
	puts $cid ""
	puts $cid "# ADD CPP /TP"
	puts $cid ""
	puts $cid "# End Source File"
}

#-----------------------------------------------------------------------------------------
#	spit_uber_group $name $cid $nodirgroups
#   Spits out the 'group' segments of a .dsp file
#-----------------------------------------------------------------------------------------
proc spit_uber_group { name cid nodirgroups} {
	global mod
	global tar
	global sourcePrefix
	global headerPrefix

	#Prep work

    set i [findtargetbyname $name]
    set platform $tar($i,platform)

    # sort modules by directory
    set smod_list [get_dirmod $name]

    # generate a group for each module directory, and below it a group for each
    # module in that directory
    set cur_dir "none"

	puts $cid "# Name \"$name - Win32 Debug\""
	puts $cid "# Name \"$name - Win32 Release\""
	puts $cid "# Begin Group \"$name\""
    puts $cid "";
    puts $cid "# PROP Default_Filter \"cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;cc\""

    for {set k 0} {$k < [llength $smod_list]} {incr k} {
        set m  [lindex $smod_list $k]
        if {$mod($m,name) != $cur_dir} {
            set cur_dir $mod($m,name)
            puts $cid "# Begin Group \"$cur_dir\""
            puts $cid "";
            puts $cid "# PROP Default_Filter \"cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;cc\""
			###############################################
			# Build the source and header inclusions here
			###############################################
	        for {set j 0} {$j < [llength $mod($m,srcs)]} {incr j} {
	            set mod_name $mod($m,name)
	            set more_syms "/D N_INIT=n_init_$mod_name /D N_FINI=n_fini_$mod_name /D N_NEW=n_new_$mod_name /D N_VERSION=n_version_$mod_name /D N_INITCMDS=n_initcmds_$mod_name"
	            #build a new source entry
	            if {($platform == "all") || ($platform == "win32")} {
		            puts $cid "# Begin Source File"
		            puts $cid ""
		            set filename [lindex $mod($m,files) $j]
		            puts $cid "SOURCE=$sourcePrefix\\$mod($m,dir)\\$filename.cc"

		            set cpp "/TP"
		            if { $mod($m,type) == "clib" } { set cpp "/TC" }

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
	        }

	        # add header files
	        for {set j 0} {$j < [llength $mod($m,hdrs)]} {incr j} {
	            puts $cid "# Begin Source File"
	            puts $cid "SOURCE=$headerPrefix\\$mod($m,dir)\\[lindex $mod($m,headers) $j].h"
	            puts $cid "# End Source File"
	        }
        	puts $cid "# End Group";
        }

	}
	puts $cid "# End Group"
}

#-----------------------------------------------------------------------------------------
#	gen_exe_dsp $name $nodirgroups
#   Creates a .dsp file for app targets (console or window)
#-----------------------------------------------------------------------------------------
proc gen_exe_dsp {name nodirgroups} {
	global tar
	global vstudioPrefix

    puts "Generate exe target: $name"

    # write .dsp file
    set cid [open $vstudioPrefix/$name.dsp w]

 	spit_app_header $name $cid
    gen_libs $name
	spit_d_exe_cpp $name $cid
	spit_r_exe_cpp $name $cid
	spit_uber_group $name $cid $nodirgroups

	puts $cid "# End Target"
	puts $cid "# End Project"
	close $cid
}

#-----------------------------------------------------------------------------------------
#	gen_workspace_dsp $name $nodirgroups
#   Dummy .dsp file that depends on all other projects
#-----------------------------------------------------------------------------------------
proc gen_workspace_dsp {name nodirgroups} {
	global tar
	global vstudioPrefix

    puts "Generate lib target: $name"

    # write .dsp file
    set cid [open $vstudioPrefix/$name.dsp w]

    spit_lib_header $name $cid
    gen_libs $name
	spit_d_lib_cpp $name $cid
	spit_r_lib_cpp $name $cid
	spit_uber_group $name $cid $nodirgroups

	puts $cid "# End Target"
	puts $cid "# End Project"
	close $cid
}

#-----------------------------------------------------------------------------------------
#	gen_lib_dsp $name $nodirgroups
#   Creates a .dsp file for static lib targets (console or window)
#-----------------------------------------------------------------------------------------
proc gen_lib_dsp {name nodirgroups} {
	global tar
	global vstudioPrefix

    puts "Generate lib target: $name"

    # write .dsp file
    set cid [open $vstudioPrefix/$name.dsp w]

    spit_lib_header $name $cid
    gen_libs $name
	spit_d_lib_cpp $name $cid
	spit_r_lib_cpp $name $cid
	spit_uber_group $name $cid $nodirgroups

	spit_dummy $cid
	puts $cid "# End Target"
	puts $cid "# End Project"
	close $cid
}



#-----------------------------------------------------------------------------------------
#	gen_dsw $main_target $nodirgroups
#   Creates a .dsw file for the main target
#-----------------------------------------------------------------------------------------
proc gen_dsw { main_target component_list} {
	global tar
    global vstudioPrefix

    # write .dsw file
    set cid [open $vstudioPrefix/$main_target.dsw w]

    puts $cid "Microsoft Developer Studio Workspace File, Format Version 6.00"
    puts $cid "# WARNUNG: DIESE ARBEITSBEREICHSDATEI DARF NICHT BEARBEITET ODER GELÖSCHT WERDEN!"
    puts $cid ""

	#for each project in the list
    for {set j 0} {$j < [llength $component_list]} {incr j} {
    	#get the project index
        set t [findtargetbyname [lindex $component_list $j]]
        # ignore any non-win32-packages - this was use above to build the projects
        if {(($tar($t,platform)=="all") || ($tar($t,platform)=="win32"))} {
            puts $cid "###############################################################################"
            puts $cid ""
            puts $cid "Project: \"$tar($t,name)\"=.\\$tar($t,name).dsp - Package Owner=<4>"
            puts $cid ""
            puts $cid "Package=<5>"
            puts $cid "\{\{\{"
            puts $cid "\}\}\}"
            puts $cid ""
            puts $cid "Package=<4>"
            puts $cid "\{\{\{"
            #for each project in the list - again
           	#for {set i 0} {$i < [llength $component_list]} {incr i} {
           		#set l [findtargetbyname [lindex $component_list $i]]
           		# each of its dependencies
	            for {set k 0} {$k < [llength $tar($t,depends)]} {incr k} {
	            	#write out the dependency
	                set dtar [lindex $tar($t,depends) $k]
	                # check if depend target exists as win32 target
	                set d [findtargetbyname $dtar]
	                if {(($tar($d,platform)=="win32") || ($tar($d,platform)=="all"))} {
	                    puts $cid "    Begin Project Dependency"
	                    puts $cid "    Project_Dep_Name $dtar"
	                    puts $cid "    End Project Dependency"
	                }
	            }
           	#}
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
#	gen_workspace $main_target $nodirgroups
#   Generates the .dsp and .dsw file for this workspace
#-----------------------------------------------------------------------------------------
proc gen_workspace {main_target nodirgroups} {
    global tar
    global num_tars
    global nomads_home
    global vstudioPrefix
    global release_binPrefix
    global debug_binPrefix

    puts "Generating vstudio workspace file $main_target.dsw..."

    #per the nebula2 makesln.tcl
    #generate the complete target list from its dependencies
    set component_list [gen_component_list $main_target]

    #per nebula2/makesln.tcl
    #generate package source file
    gen_package_source $main_target $component_list

   # for each target - generate a dsp file
    for {set i 0} {$i < [llength $component_list]} {incr i} {

        set t [findtargetbyname [lindex $component_list $i]]
        # ignore any non-win32-packages
        if {(($tar($t,platform)=="all") || ($tar($t,platform)=="win32"))} {
            if {$tar($t,type) == "package"} {
            	puts "ERROR: package targets not supported."
				#this is missing as in the vs.net version
                #gen_package_dsp $tar($t,name)
            } elseif {$tar($t,type) == "dll"} {
            	puts "ERROR: dll targets not supported"
                #gen_dll_dsp $tar($t,name) $nodirgroups
            } elseif {$tar($t,type) == "exe"} {
                gen_exe_dsp $tar($t,name) $nodirgroups
            } elseif {$tar($t,type) == "mll"} {
            	puts "ERROR: dll targets not supported"
                #gen_dll_dsp $tar($t,name) $nodirgroups
            } elseif {$tar($t,type) == "lib"} {
            	gen_lib_dsp $tar($t,name) $nodirgroups
            } elseif {$tar($t,type) == "workspace"} {
            	gen_workspace_dsp $tar($t,name) $nodirgroups
            } else {
                puts "ERROR: UNKNOWN TARGET TYPE '$tar($t,type)'"
                exit
            }
            if {$tar($t,type) == "package"} {
                genpacksrc $tar($t,name)
            }
        }
    	gen_dsw $main_target $component_list
    }
}

#-----------------------------------------------------------------------------------------
#EOF
#-----------------------------------------------------------------------------------------
