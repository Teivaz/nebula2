#-------------------------------------------------------------------------------
#    vstudio7.tcl
#
#  FIXME:  This needs severly cleaned up and finished to use all relevant
#          bits as an example for other compiler generator authors.
#  FIXME:  Make sure the functions are properly commented on.
#
#   Cobbled up from the original buildsys courtesy of Radon Labs and
#   Leaf Garland's Nebula2 VC7 project files.
#   Copyright (c) 2003 Radon Labs
#
#   This file is licenesed under the terms of the Nebula License
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
#    Globals
#-------------------------------------------------------------------------------

set release_preprocessor_defs "WIN32;NDEBUG;NT_PLUGIN"
set debug_preprocessor_defs "WIN32;_DEBUG;NT_PLUGIN"
set neb_libpath_win32 "./code/nebula2/lib/win32_vc_i386"

#-------------------------------------------------------------------------------
#  emit_vcproj_header $name $cid
#-------------------------------------------------------------------------------
proc emit_vcproj_header {name cid} {
    puts $cid "<?xml version=\"1.0\" encoding = \"Windows-1252\"?>"
    puts $cid "<VisualStudioProject"
    puts $cid "\tProjectType=\"Visual C++\""
    puts $cid "\tVersion=\"7.00\""
    puts $cid "\tName=\"$name\""
    puts $cid "\tSccProjectName=\"\""
    puts $cid "\tSccLocalPath=\"\">"
    puts $cid "\t<Platforms>"
    puts $cid "\t\t<Platform Name=\"Win32\"/>"
    puts $cid "\t</Platforms>"
}

#-------------------------------------------------------------------------------
#   emit_vcproj_config $name $cid $debug
#-------------------------------------------------------------------------------
proc emit_vcproj_config {name cid debug inc_list lib_list} {
    variable debug_preprocessor_defs
    variable release_preprocessor_defs
    global cur_workspacepath
    variable neb_libpath_win32

    set tartype [get_tartype $name]
    set def_list ""

    set moddeffile [get_moddeffile $name]
        if { $moddeffile != "" } {
            set moddeffile [findrelpath $cur_workspacepath $moddeffile]
    }

    set typenumber 0
    set prefix ""
    switch -exact $tartype {
        "lib" {
            set typenumber 4
            set extension "lib"
            if { $debug == 1 } {
                set prefix "d_"
            }
            append def_list "N_STATIC;"
        }
        "dll" {
            set typenumber 2
            set extension [get_dllextension $name]
        }
        "exe" {
            set typenumber 1
            set extension "exe"
            append def_list "N_STATIC;"
        }
    }

    foreach def [get_tardefs $name] {
        if { [llength [lindex $def 0]] && [llength [lindex $def 1]] } {
            append def_list "[lindex $def 0]=[lindex $def 1];"
        } elseif { [llength [lindex $def 0]] } {
            append def_list "[lindex $def 0];"
        }
    }

    set lib_path [findrelpath $cur_workspacepath $neb_libpath_win32]

    if { $debug == 1 } {
        set confname "Debug|Win32"
        set idir ".\\Debug"
        #[path_wspacetointer]/win32d
        set odir [path_wspacetooutput]/win32d
        set win32_libs [get_win32libs_debug $name]
        append def_list $debug_preprocessor_defs
    } else {
        set confname "Release|Win32"
        set idir ".\\Release"
        #[path_wspacetointer]/win32
        set odir [path_wspacetooutput]/win32
        set win32_libs [get_win32libs_release $name]
        append def_list $release_preprocessor_defs
    }

    puts $cid "\t\t<Configuration"
    puts $cid "\t\t\tName=\"$confname\""
    puts $cid "\t\t\tOutputDirectory=\"$odir\""
    puts $cid "\t\t\tIntermediateDirectory=\"$idir\""
    puts $cid "\t\t\tConfigurationType=\"$typenumber\""
    puts $cid "\t\t\tUseOfMFC=\"0\""
    puts $cid "\t\t\tATLMinimizesCRunTimeLibraryUsage=\"FALSE\">"
    puts $cid "\t\t\t<Tool"
    puts $cid "\t\t\t\tName=\"VCCLCompilerTool\""
    puts $cid "\t\t\t\tAdditionalOptions=\"\""
    puts $cid "\t\t\t\tOptimization=\"0\""
    puts $cid "\t\t\t\tAdditionalIncludeDirectories=\"$inc_list\""
    puts $cid "\t\t\t\tPreprocessorDefinitions=\"__WIN32__;$def_list\""
    puts $cid "\t\t\t\tExceptionHandling=\"[get_exceptions $name]\""
    puts $cid "\t\t\t\tBasicRuntimeChecks=\"0\""
    puts $cid "\t\t\t\tRuntimeLibrary=\"1\""
    puts $cid "\t\t\t\tRuntimeTypeInfo=\"[get_rtti $name]\""
    puts $cid "\t\t\t\tUsePrecompiledHeader=\"0\""
    puts $cid "\t\t\t\tAssemblerListingLocation=\"$idir\\$name\""
    puts $cid "\t\t\t\tObjectFile=\"$idir\\$name\\\""
    puts $cid "\t\t\t\tProgramDataBaseFileName=\"$idir\\$name\""
    puts $cid "\t\t\t\tWarningLevel=\"3\""
    puts $cid "\t\t\t\tSuppressStartupBanner=\"TRUE\""
    if {$debug == 1} {
        puts $cid "\t\t\t\tDebugInformationFormat=\"4\""
    }
    puts $cid "\t\t\t\tCompileAs=\"2\"/>"

    # Now either the linker or the librarian
    if {$tartype == "lib" } {
        puts $cid "\t\t\t<Tool"
        puts $cid "\t\t\t\tName=\"VCLibrarianTool\""
        puts $cid "\t\t\t\tOutputFile=\"$idir\\$prefix$name.$extension\"/>"
    } else {
        puts $cid "\t\t\t<Tool"
        puts $cid "\t\t\t\tName=\"VCLinkerTool\""
        puts $cid "\t\t\t\tAdditionalDependencies=\"$win32_libs\""
        puts $cid "\t\t\t\tOutputFile=\"$odir\\$name.$extension\""
        if {$debug == 1} {
            puts $cid "\t\t\t\tGenerateDebugInformation=\"TRUE\""
            puts $cid "\t\t\t\tLinkIncremental=\"2\""
        } else {
            puts $cid "\t\t\t\tLinkIncremental=\"1\""
        }
        puts $cid "\t\t\t\tSuppressStartupBanner=\"TRUE\""
        puts $cid "\t\t\t\tAdditionalLibraryDirectories=\"$lib_path;$idir\""
        puts $cid "\t\t\t\tModuleDefinitionFile=\"$moddeffile\""
        puts $cid "\t\t\t\tProgramDatabaseFile=\"$idir\\$name.pdb\"/>"
    }

    if {$tartype == "exe" } {
        puts $cid "\t\t\t<Tool"
        puts $cid "\t\t\t\tName=\"VCResourceCompilerTool\""
        puts $cid "\t\t\t\tPreprocessorDefinitions=\"_DEBUG\""
        puts $cid "\t\t\t\tCulture=\"1033\" />"
    }

    puts $cid "\t\t</Configuration>"
}

#-------------------------------------------------------------------------------
#   emit_vcproj_files $name $cid
#-------------------------------------------------------------------------------
proc emit_vcproj_files {name cid} {
    global platform

    puts $cid "\t<Files>"

    foreach module [get_tarmods $name] {
        switch -exact [get_modtype $module] {
            "c" { set compileas 1 }
            "cpp" { set compileas 2 }
            default { set compileas 0 }
        }

        set more_syms "N_INIT=n_init_$module;N_NEW=n_new_$module;N_INITCMDS=n_initcmds_$module"
        puts $cid "\t\t<Filter Name=\"$module\" Filter=\"cpp;c;cxx;cc;h;hxx;hcc\" >"

        # Source files
        foreach sourcefile [get_modsources_dressed $module] {
            regsub -all "/" [pathto [getfilenamewithextension $sourcefile cc] ] "\\" filename
            puts $cid "\t\t\t<File"
            puts $cid "\t\t\t\tRelativePath=\"$filename\" >"
            puts $cid "\t\t\t\t<FileConfiguration Name=\"Debug|Win32\">"
            puts $cid "\t\t\t\t\t<Tool Name=\"VCCLCompilerTool\" PreprocessorDefinitions=\"$more_syms\" CompileAs=\"$compileas\" />"
            puts $cid "\t\t\t\t</FileConfiguration>"
            puts $cid "\t\t\t\t<FileConfiguration Name=\"Release|Win32\">"
            puts $cid "\t\t\t\t\t<Tool Name=\"VCCLCompilerTool\" PreprocessorDefinitions=\"$more_syms\" CompileAs=\"$compileas\" />"
            puts $cid "\t\t\t\t</FileConfiguration>"
            puts $cid "\t\t\t</File>"
        }

        # Header files
        foreach headerfile [get_modheaders_dressed $module] {
            regsub -all "/" [pathto $headerfile.h] "\\" filename
            puts $cid "\t\t\t<File RelativePath=\"$filename\" />"
        }

        puts $cid "\t\t</Filter>"
    }

    #if the module definition file is set and target is dll then add it
    set moddeffile [get_moddeffile $name]
    if { $moddeffile != "" && [get_tartype $name] == "dll" } {
        global cur_workspacepath
        set moddeffile [findrelpath $cur_workspacepath $moddeffile]
        regsub -all "/" $moddeffile "\\" filename
        puts $cid "\t\t<Filter Name=\"Module Definition\" Filter=\"def\" >"
        puts $cid "\t\t\t<File RelativePath=\"$filename\" />"
        puts $cid "\t\t</Filter>"
    }

    # Resource files
    set tartype [get_tartype $name]
    if {$tartype == "exe" || $tartype == "dll"} {
        puts $cid "\t\t<Filter Name=\"Resource Files\" Filter=\"rc\" >"

        # add standard nebula rsrc to exe
        if {$tartype == "exe"} {
            puts $cid "\t\t\t<File RelativePath=\"..\\pkg\\res_$name.rc\" />"
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
            regsub -all "/" $resfile "\\" filename
            puts $cid "\t\t\t<File RelativePath=\"$filename\" />"
        }
        puts $cid "\t\t</Filter>"
    }

    puts $cid "\t</Files>"
}

#-------------------------------------------------------------------------------
#   emit_vcproj_tail $cid
#-------------------------------------------------------------------------------
proc emit_vcproj_tail {cid} {
    puts $cid "\t<Globals>"
    puts $cid "\t</Globals>"
    puts $cid "</VisualStudioProject>"
}

#-------------------------------------------------------------------------------
#   gen_vcproj $name
#   Creates a .vcproj file
#-------------------------------------------------------------------------------
proc gen_vcproj {name inc_list lib_list} {
    global home
    global cur_workspacepath

    ::log::log debug "Generate vcproj target: $name"

    # write .vcproj file
    set cid [open [cleanpath $home/$cur_workspacepath/$name.vcproj] w]

    emit_vcproj_header $name $cid
    puts $cid "\t<Configurations>"
    emit_vcproj_config $name $cid 1 $inc_list $lib_list
    emit_vcproj_config $name $cid 0 $inc_list $lib_list
    puts $cid "\t</Configurations>"

    emit_vcproj_files $name $cid

    emit_vcproj_tail $cid

    close $cid
}

#-------------------------------------------------------------------------------
#   gen_sln $wspace_idx
#   Creates a .sln file
#-------------------------------------------------------------------------------
proc gen_sln { name } {
    global platform
    global home
    global cur_workspacepath
    set targetuuids(0) 0

    ::log::log info "Generating Visual Studio .NET 2002 solution file $name.sln..."

    # write .sln file
    set cid [open [cleanpath $home/$cur_workspacepath/$name.sln] w]

    puts $cid "Microsoft Visual Studio Solution File, Format Version 7.00"

    #for each project in the list
    foreach target [get_targets] {
        if { [test_tarplatform $target $platform ] } {
            continue
        }

        set targetuuids($target) [string toupper [exec uuidgen]]

        puts $cid "Project(\"{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}\") = \"$target\", \"$target.vcproj\", \"{$targetuuids($target)}\""
        puts $cid "EndProject"
    }

    puts $cid "Global"
    puts $cid "\tGlobalSection(SolutionConfiguration) = preSolution"
    puts $cid "\t\tConfigName.0 = Debug_Win32"
    puts $cid "\t\tConfigName.1 = Release_Win32"
    puts $cid "\tEndGlobalSection"

    # And now, for each project, its dependencies
    puts $cid "\tGlobalSection(ProjectDependencies) = postSolution    "
    foreach target [get_targets] {
        set depcount 0
        foreach dep [get_tardeps $target] {
            if {[test_tarplatform $dep $platform]} {
                continue
            }

            puts $cid "\t\t{$targetuuids($target)}.$depcount = {$targetuuids($dep)}"
            incr depcount
        }
    }
    puts $cid "\tEndGlobalSection"

    # Configurations
    puts $cid "\tGlobalSection(ProjectConfiguration) = postSolution"
    foreach target [get_targets] {
        if {[test_tarplatform $target $platform]} {
            continue
        }
        puts $cid "\t\t{$targetuuids($target)}.Debug_Win32.ActiveCfg = Debug|Win32"
        puts $cid "\t\t{$targetuuids($target)}.Debug_Win32.Build.0 = Debug|Win32"
        puts $cid "\t\t{$targetuuids($target)}.Release_Win32.ActiveCfg = Release|Win32"
        puts $cid "\t\t{$targetuuids($target)}.Release_Win32.Build.0 = Release|Win32"
    }

    puts $cid "\tEndGlobalSection"
    puts $cid "\tGlobalSection(ExtensibilityGlobals) = postSolution"
    puts $cid "\tEndGlobalSection"
    puts $cid "\tGlobalSection(ExtensibilityAddIns) = postSolution"
    puts $cid "\tEndGlobalSection"
    puts $cid "EndGlobal"

    close $cid
}

#-------------------------------------------------------------------------------
#   generate
#   Generates the .sln and .vcproj file for the specified workspaces
#-------------------------------------------------------------------------------
proc generate { wslist } {
    ::log::log debug "Looking for uuidgen...."
    if { [catch { exec uuidgen }] } {
        ::log::log error "uuidgen.exe not found skipping Visual Studio Solutions."
        return
    } else {
        ::log::log debug "uuidgen.exe found"
    }

    set vstudiopath ./build/vstudio7
    set outputpath  ./bin
    set interpath   $vstudiopath/inter

    # list of required targets - project files
    set targets ""

    foreach workspace [get_workspaces $wslist]  {
        # let the buildsys know which workspace we are currently working
        # with and what the default directories for that workspace should
        # be - default directories may be overridden - this particular
        # call also writes the pkg_XXX.cc files for the workspace out.

        use_workspace $workspace $vstudiopath $outputpath $interpath

        gen_sln $workspace
		
		# collect targetlist from workspace
        foreach target [get_targets] {
			lappend targets [list $target $workspace]
		}
    }

    #remove double entrys - ignore workspace numbers
    set targets [lsort -unique -index 0 $targets]

    foreach target $targets {
		use_workspace [lindex $target 1] $vstudiopath $outputpath $interpath

        set inc_list [join [get_incsearchdirs] ";"]
        set lib_list [join [get_libsearchdirs] "/win32_vc_i386;"]

		gen_vcproj [lindex $target 0] $inc_list $lib_list
	}
}

#-------------------------------------------------------------------------------
#   description
#   Return a description of this generator.
#-------------------------------------------------------------------------------
proc description { } {
    return "Support for Microsoft Visual Studio 7. This is also known as Visual Studio .NET."
}

#-------------------------------------------------------------------------------
#EOF
#-------------------------------------------------------------------------------
