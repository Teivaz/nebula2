#--------------------------------------------------------------------
#   makesln.tcl
#   ===========
#   Functions to generate Visual Studio .NET solution files
#   (with Xbox configurations removed).
#
#   Is included into makemake.tcl.
#
#   UUIDGEN.exe must be in PATH!
#   (\Program Files\Microsoft Visual Studio .NET\Common7\Tools\Bin)
#
#   29-Jan-2002 stki	created
#	20-Feb-2002 stki	+ added fix for missing _ver files
#
#--------------------------------------------------------------------
set headerPrefix "..\\inc"
set sourcePrefix "..\\src"
set vstudioPrefix "../vstudio"
set binPrefix "..\\..\\..\\bin\\win32"

set release_preprocessor_defs "WIN32;NDEBUG;NT_PLUGIN;_HAS_EXCEPTIONS=0"
set debug_preprocessor_defs "WIN32;_DEBUG;NT_PLUGIN;_HAS_EXCEPTIONS=0"
set libpath "$binPrefix,..\\lib\\win32_vc_i386;..\\..\\bin\\win32"

#--------------------------------------------------------------------
#   get_dirmod
#   Return list of module indices, sorted by the module's directory.
#   09-Mar-00
#--------------------------------------------------------------------
proc get_dirmod {name} {
    global tar
    global num_tars
    global mod
    global num_mods

    set t [findtargetbyname $name]

    set dirmod_list ""
    for {set i 0} {$i < [llength $tar($t,mods)]} {incr i} {
        set m [findmodbyname [lindex $tar($t,mods) $i]]
        set dm "$mod($m,dir) $m"
        lappend dirmod_list $dm
    }

    # puts "Before sort: $dirmod_list"
    set dirmod_list [lsort -index 0 $dirmod_list]
    # puts "After sort: $dirmod_list"

    set mod_list ""
    for {set i 0} {$i < [llength $dirmod_list]} {incr i} {
        lappend mod_list [lindex [lindex $dirmod_list $i] 1]
    }
    # puts "Result: $mod_list"
    return $mod_list
}

#--------------------------------------------------------------------
#   extract_libs
#   Extract the link libs for a given target.
#--------------------------------------------------------------------
proc extract_libs {targetName libType} {
    global tar
    set i [findtargetbyname $targetName]
    set libList ""
    for {set j 0} {$j < [llength $tar($i,$libType)]} {incr j} {
        append libList "[lindex $tar($i,$libType) $j] "
    }
    return $libList
}

#--------------------------------------------------------------------
#   gen_dll_vcproj
#   Generate vcproj file for a dll or mll target definition.
#   29-Jan-02	stki    created
#--------------------------------------------------------------------
proc gen_dll_vcproj {name extension nodirgroups} {
    global tar
    global num_tars
    global mod
    global num_mods
    global nomads_home
    global release_preprocessor_defs
    global debug_preprocessor_defs
    global libpath
    global headerPrefix
    global sourcePrefix
    global vstudioPrefix
    global binPrefix

    puts "-> gen_dll_vcproj $name"

    # create a local pkg_ file with Nebula module declarations
    set component_list [gen_component_list $name]
    gen_package_source $name $component_list

    set i [findtargetbyname $name]
    set platform $tar($i,platform)
    set preBuild $tar($i,prebuild_win32);
 
    # sort modules by directory
    set smod_list [get_dirmod $name]

    # create list of link libs (win32 libs)
    set win32_libs [extract_libs $name libs_win32]
    set debug_win32_libs [extract_libs $name libs_win32_debug]
    set release_win32_libs [extract_libs $name libs_win32_release]

    # write .vcproj file
    set cid [open $vstudioPrefix/$name.vcproj w]
    
    puts $cid "<?xml version=\"1.0\" encoding = \"Windows-1252\"?>"
    puts $cid "<VisualStudioProject"
    puts $cid "	ProjectType=\"Visual C++\""
    puts $cid "	Version=\"7.00\""
    puts $cid "	Name=\"$name\""
    puts $cid "	SccProjectName=\"\""
    puts $cid "	SccLocalPath=\"\">"
    puts $cid "	<Platforms>"
    puts $cid "		<Platform"
    puts $cid "			Name=\"Win32\"/>"
    puts $cid "	</Platforms>"
    
    puts $cid "	<Configurations>"
    
    #--- Win32 Debug Configuration
    puts $cid "		<Configuration"
    puts $cid "			Name=\"Debug|Win32\""
    puts $cid "			OutputDirectory=\"$binPrefix\""
    puts $cid "			IntermediateDirectory=\".\\Debug\\$name\""
    puts $cid "			ConfigurationType=\"2\""
    puts $cid "			UseOfMFC=\"0\""
    puts $cid "			ATLMinimizesCRunTimeLibraryUsage=\"FALSE\">"
    puts $cid "			<Tool"
    puts $cid "				Name=\"VCCLCompilerTool\""
    puts $cid "				AdditionalOptions=\"\""
    puts $cid "				Optimization=\"0\""
    puts $cid "				AdditionalIncludeDirectories=\"$headerPrefix\""
    puts $cid "				PreprocessorDefinitions=\"N_STATIC;__WIN32__;$debug_preprocessor_defs\""
    puts $cid "				BasicRuntimeChecks=\"0\""
    puts $cid "				RuntimeLibrary=\"1\""
    puts $cid "				UsePrecompiledHeader=\"0\""
    puts $cid "				AssemblerListingLocation=\".\\Debug\\$name/\""
    puts $cid "				ObjectFile=\".\\Debug\\$name/\""
    puts $cid "             ExceptionHandling=\"FALSE\""    
    puts $cid "				ProgramDataBaseFileName=\".\\Debug\\$name/\""
    puts $cid "				WarningLevel=\"3\""
    puts $cid "             BufferSecurityCheck=\"TRUE\""    
    puts $cid "				SuppressStartupBanner=\"TRUE\""
    puts $cid "				DebugInformationFormat=\"4\""
    puts $cid "				CompileAs=\"2\"/>"
    puts $cid "			<Tool"
    puts $cid "				Name=\"VCCustomBuildTool\"/>"
    puts $cid "			<Tool"
    puts $cid "				Name=\"VCLinkerTool\""
    puts $cid "				AdditionalOptions=\"/MACHINE:IX86\""
    puts $cid "				AdditionalDependencies=\"$win32_libs $debug_win32_libs\""
    puts $cid "				OutputFile=\"$binPrefix\\$name.$extension\""
    puts $cid "				LinkIncremental=\"2\""
    puts $cid "				SuppressStartupBanner=\"TRUE\""
    puts $cid "				AdditionalLibraryDirectories=\"Debug,$libpath\""
    puts $cid "				GenerateDebugInformation=\"TRUE\""
    puts $cid "				ProgramDatabaseFile=\"$binPrefix\\$name.pdb\""
    puts $cid "				ImportLibrary=\"$binPrefix\\$name.lib\"/>"
    puts $cid "			<Tool"
    puts $cid "				Name=\"VCResourceCompilerTool\""
    puts $cid "				PreprocessorDefinitions=\"_DEBUG\""
    puts $cid "				Culture=\"1033\"/>"
    if {$preBuild != ""} {
        puts $cid "         <Tool"
        puts $cid "             Name=\"VCPreBuildEventTool\""
        puts $cid "             CommandLine=\"$preBuild\"/>"
    }
    puts $cid "		</Configuration>"

    #--- Win32 Release Configuration ---
    puts $cid "		<Configuration"
    puts $cid "			Name=\"Release|Win32\""
    puts $cid "			OutputDirectory=\"$binPrefix\""
    puts $cid "			IntermediateDirectory=\".\\Release\\$name\""
    puts $cid "			ConfigurationType=\"2\""
    puts $cid "			UseOfMFC=\"0\""
    puts $cid "			ATLMinimizesCRunTimeLibraryUsage=\"FALSE\">"
    puts $cid "			<Tool"
    puts $cid "				Name=\"VCCLCompilerTool\""
    puts $cid "				AdditionalOptions=\"\""
    puts $cid "				Optimization=\"4\""
    puts $cid "				GlobalOptimizations=\"TRUE\""
    puts $cid "				InlineFunctionExpansion=\"2\""
    puts $cid "				EnableIntrinsicFunctions=\"TRUE\""
    puts $cid "				FavorSizeOrSpeed=\"1\""
    puts $cid "				OmitFramePointers=\"TRUE\""
    puts $cid "				OptimizeForProcessor=\"2\""
    puts $cid "				AdditionalIncludeDirectories=\"$headerPrefix\""
    puts $cid "				PreprocessorDefinitions=\"N_STATIC;__WIN32__;$release_preprocessor_defs\""
    puts $cid "				StringPooling=\"TRUE\""
    puts $cid "				RuntimeLibrary=\"0\""
    puts $cid "             BufferSecurityCheck=\"FALSE\""
    puts $cid "				EnableFunctionLevelLinking=\"TRUE\""
    puts $cid "             ExceptionHandling=\"FALSE\""    
    puts $cid "				UsePrecompiledHeader=\"0\""
    puts $cid "				AssemblerListingLocation=\".\\Release\\$name/\""
    puts $cid "				ObjectFile=\".\\Release\\$name/\""
    puts $cid "				ProgramDataBaseFileName=\".\\Release\\$name/\""
    puts $cid "				WarningLevel=\"3\""
    puts $cid "				SuppressStartupBanner=\"TRUE\""
    puts $cid "				CompileAs=\"2\"/>"
    puts $cid "			<Tool"
    puts $cid "				Name=\"VCLinkerTool\""
    puts $cid "				AdditionalOptions=\"/MACHINE:IX86\""
    puts $cid "				AdditionalDependencies=\"$win32_libs $release_win32_libs\""
    puts $cid "				OutputFile=\"$binPrefix\\$name.$extension\""
    puts $cid "				LinkIncremental=\"1\""
    puts $cid "				SuppressStartupBanner=\"TRUE\""
    puts $cid "				AdditionalLibraryDirectories=\".\\Release,$libpath\""
    puts $cid "				ProgramDatabaseFile=\"$binPrefix\\$name.pdb\""
    puts $cid "				ImportLibrary=\"$binPrefix\\$name.lib\"/>"
    puts $cid "			<Tool"
    puts $cid "				Name=\"VCResourceCompilerTool\""
    puts $cid "				PreprocessorDefinitions=\"NDEBUG\""
    puts $cid "				Culture=\"1033\"/>"
    if {$preBuild != ""} {
        puts $cid "         <Tool"
        puts $cid "             Name=\"VCPreBuildEventTool\""
        puts $cid "             CommandLine=\"$preBuild\"/>"
    }
    puts $cid "		</Configuration>"
    puts $cid "	</Configurations>"

    puts $cid "	<Files>"
    puts $cid "		<Filter"
    puts $cid "			Name=\"Source Files\""
    puts $cid "			Filter=\"cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;cc\">"
    for {set k 0} {$k < [llength $tar($i,mods)]} {incr k} {
        set m [findmodbyname [lindex $tar($i,mods) $k]]

        # add regular sources
        for {set j 0} {$j < [llength $mod($m,srcs)]} {incr j} {
            puts $cid "			<File"
            puts $cid "				RelativePath=\"$sourcePrefix\\$mod($m,dir)\\[lindex $mod($m,files) $j].cc\">"

            #--- file configuration for Win32 Debug ---
            if {($platform != "all") && ($platform != "win32")} {
                # exclude this file from build
                puts $cid "					<FileConfiguration Name=\"Debug|Win32\" ExcludedFromBuild=\"TRUE\">"
            } else {        
                puts $cid "					<FileConfiguration Name=\"Debug|Win32\">"
            }
            puts $cid "					</FileConfiguration>"
            
            #--- file configuration for Win32 Release ---
            if {($platform != "all") && ($platform != "win32")} {
                # exclude this file from build
                puts $cid "					<FileConfiguration Name=\"Release|Win32\" ExcludedFromBuild=\"TRUE\">"
            } else {        
                puts $cid "					<FileConfiguration Name=\"Release|Win32\">"
            }
            puts $cid "					</FileConfiguration>"

            puts $cid "			</File>"
        }
        # add header files
        for {set j 0} {$j < [llength $mod($m,hdrs)]} {incr j} {
            puts $cid "			<File"
            puts $cid "				RelativePath=\"$headerPrefix\\$mod($m,dir)\\[lindex $mod($m,headers) $j].h\">"
            puts $cid "			</File>"
        }
    }
    puts $cid "		</Filter>"

    # End Of File
    puts $cid "	</Files>"
    puts $cid "	<Globals>"
    puts $cid "	</Globals>"
    puts $cid "</VisualStudioProject>"

    close $cid
}

#--------------------------------------------------------------------
#   gen_lib_vcproj
#   Generate vcproj file for a static link lib.
#   29-Jan-02	stki    created
#   31-Jan-02   floh    modify name of output lib based on 
#                       debug/release mode (debug libs get a d_ prefix)
#--------------------------------------------------------------------
proc gen_lib_vcproj {name nodirgroups} {
    global tar
    global num_tars
    global mod
    global num_mods
    global nomads_home
    global release_preprocessor_defs
    global debug_preprocessor_defs
    global libpath
    global headerPrefix
    global sourcePrefix
    global vstudioPrefix
    global binPrefix

    puts "-> gen_lib_vcproj $name"

    set i [findtargetbyname $name]
    set platform $tar($i,platform)

    # sort modules by directory
    set smod_list [get_dirmod $name]

    # write .vcproj file
    set cid [open $vstudioPrefix/$name.vcproj w]
    
    puts $cid "<?xml version=\"1.0\" encoding = \"Windows-1252\"?>"
    puts $cid "<VisualStudioProject"
    puts $cid "	ProjectType=\"Visual C++\""
    puts $cid "	Version=\"7.00\""
    puts $cid "	Name=\"$name\""
    puts $cid "	SccProjectName=\"\""
    puts $cid "	SccLocalPath=\"\">"
    puts $cid "	<Platforms>"
    puts $cid "		<Platform"
    puts $cid "			Name=\"Win32\"/>"
    puts $cid "	</Platforms>"
    
    puts $cid "	<Configurations>"
 
    # --- Win32 Debug Configuration ---
    puts $cid "		<Configuration"
    puts $cid "			Name=\"Debug|Win32\""
    puts $cid "			OutputDirectory=\".\\Debug\""
    puts $cid "			IntermediateDirectory=\".\\Debug\\$name\""
    puts $cid "			ConfigurationType=\"4\""
    puts $cid "			UseOfMFC=\"0\""
    puts $cid "			ATLMinimizesCRunTimeLibraryUsage=\"FALSE\">"
    puts $cid "			<Tool"
    puts $cid "				Name=\"VCCLCompilerTool\""
    puts $cid "				AdditionalOptions=\"\""
    puts $cid "				Optimization=\"0\""
    puts $cid "				AdditionalIncludeDirectories=\"$headerPrefix\""
    puts $cid "				PreprocessorDefinitions=\"N_STATIC;__WIN32__;$debug_preprocessor_defs\""
    puts $cid "             ExceptionHandling=\"FALSE\""
    puts $cid "				BasicRuntimeChecks=\"0\""
    puts $cid "				RuntimeLibrary=\"1\""
    puts $cid "				UsePrecompiledHeader=\"0\""
    puts $cid "				AssemblerListingLocation=\".\\Debug\\$name/\""
    puts $cid "				ObjectFile=\".\\Debug\\$name/\""
    puts $cid "             BufferSecurityCheck=\"TRUE\""    
    puts $cid "				ProgramDataBaseFileName=\".\\Debug\\$name/\""
    puts $cid "				WarningLevel=\"3\""
    puts $cid "				SuppressStartupBanner=\"TRUE\""
    puts $cid "				DebugInformationFormat=\"4\""
    puts $cid "				CompileAs=\"2\"/>"
    puts $cid "			<Tool"
    puts $cid "				Name=\"VCCustomBuildTool\"/>"
    puts $cid "			<Tool"
    puts $cid "				Name=\"VCLibrarianTool\""
    puts $cid "				OutputFile=\".\\Debug\\d_$name.lib\"/>"
    puts $cid "			<Tool"
    puts $cid "				Name=\"VCResourceCompilerTool\""
    puts $cid "				PreprocessorDefinitions=\"_DEBUG\""
    puts $cid "				Culture=\"1033\"/>"
    puts $cid "		</Configuration>"

    # --- Win32 Release Configuration ---
    puts $cid "		<Configuration"
    puts $cid "			Name=\"Release|Win32\""
    puts $cid "			OutputDirectory=\".\\Release\""
    puts $cid "			IntermediateDirectory=\".\\Release\\$name\""
    puts $cid "			ConfigurationType=\"4\""
    puts $cid "			UseOfMFC=\"0\""
    puts $cid "			ATLMinimizesCRunTimeLibraryUsage=\"FALSE\""
    puts $cid "         WholeProgramOptimization=\"TRUE\">"    
    puts $cid "			<Tool"
    puts $cid "				Name=\"VCCLCompilerTool\""
    puts $cid "				AdditionalOptions=\"\""
    puts $cid "				Optimization=\"4\""
    puts $cid "				GlobalOptimizations=\"TRUE\""
    puts $cid "				InlineFunctionExpansion=\"2\""
    puts $cid "				EnableIntrinsicFunctions=\"TRUE\""
    puts $cid "				FavorSizeOrSpeed=\"1\""
    puts $cid "				OmitFramePointers=\"TRUE\""
    puts $cid "				OptimizeForProcessor=\"2\""
    puts $cid "				AdditionalIncludeDirectories=\"$headerPrefix\""
    puts $cid "				PreprocessorDefinitions=\"N_STATIC;__WIN32__;$release_preprocessor_defs\""
    puts $cid "				StringPooling=\"TRUE\""
    puts $cid "             ExceptionHandling=\"FALSE\""
    puts $cid "             BufferSecurityCheck=\"FALSE\""
    puts $cid "				RuntimeLibrary=\"0\""
    puts $cid "				EnableFunctionLevelLinking=\"TRUE\""
    puts $cid "				UsePrecompiledHeader=\"0\""
    puts $cid "				AssemblerListingLocation=\".\\Release\\$name/\""
    puts $cid "				ObjectFile=\".\\Release\\$name/\""
    puts $cid "				ProgramDataBaseFileName=\".\\Release\\$name/\""
    puts $cid "				WarningLevel=\"3\""
    puts $cid "				SuppressStartupBanner=\"TRUE\""
    puts $cid "				CompileAs=\"2\"/>"
    puts $cid "			<Tool"
    puts $cid "				Name=\"VCCustomBuildTool\"/>"
    puts $cid "			<Tool"
    puts $cid "				Name=\"VCLibrarianTool\""
    puts $cid "				OutputFile=\".\\Release\\$name.lib\"/>"
    puts $cid "			<Tool"
    puts $cid "				Name=\"VCResourceCompilerTool\""
    puts $cid "				PreprocessorDefinitions=\"NDEBUG\""
    puts $cid "				Culture=\"1033\"/>"
    puts $cid "		</Configuration>"
    puts $cid "	</Configurations>"

    puts $cid "	<Files>"

    # generate a group for each module directory, and below it a group for each
    # module in that directory
    set cur_dir "none"
    for {set k 0} {$k < [llength $smod_list]} {incr k} {
        set m  [lindex $smod_list $k]

        # check if we need to start a new directory group
        if {!$nodirgroups && ($mod($m,dir) != $cur_dir)} {
            # end current directory group
            if {$cur_dir != "none"} {
                puts $cid "		</Filter>"
            }
            # start a new group
            puts $cid "		<Filter"
            puts $cid "			Name=\"$mod($m,dir)\""
            puts $cid "			Filter=\"cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;cc\">"
            set cur_dir $mod($m,dir)
        }

        # create a sub group for each module and add files
        puts $cid "			<Filter"
        puts $cid "				Name=\"$mod($m,name)\""
        puts $cid "				Filter=\"cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;cc\">"
        for {set j 0} {$j < [llength $mod($m,srcs)]} {incr j} {
            set mod_name $mod($m,name)
            set more_syms "N_INIT=n_init_$mod_name;N_FINI=n_fini_$mod_name;N_NEW=n_new_$mod_name;N_INITCMDS=n_initcmds_$mod_name"
            puts $cid "				<File"
            puts $cid "					RelativePath=\"$sourcePrefix\\$mod($m,dir)\\[lindex $mod($m,files) $j].cc\">"

            #--- file configuration for Win32 Debug ---
            if {($platform != "all") && ($platform != "win32")} {
                # exclude this file from build
                puts $cid "					<FileConfiguration Name=\"Debug|Win32\" ExcludedFromBuild=\"TRUE\">"
            } else {        
                puts $cid "					<FileConfiguration Name=\"Debug|Win32\">"
            }
            puts $cid "						<Tool"
            puts $cid "							Name=\"VCCLCompilerTool\""
            if {$mod($m,type) == "clib"} {
                puts $cid "                         CompileAs=\"1\""
            }
            puts $cid "							PreprocessorDefinitions=\"$more_syms\"/>"
            puts $cid "					</FileConfiguration>"
            
            #--- file configuration for Win32 Release ---
            if {($platform != "all") && ($platform != "win32")} {
                # exclude this file from build
                puts $cid "					<FileConfiguration Name=\"Release|Win32\" ExcludedFromBuild=\"TRUE\">"
            } else {        
                puts $cid "					<FileConfiguration Name=\"Release|Win32\">"
            }
            puts $cid "						<Tool"
            puts $cid "							Name=\"VCCLCompilerTool\""
            if {$mod($m,type) == "clib"} {
                puts $cid "                         CompileAs=\"1\""
            }
            puts $cid "							PreprocessorDefinitions=\"$more_syms\"/>"
            puts $cid "					</FileConfiguration>"

            puts $cid "				</File>"
        }

        # add header files
        for {set j 0} {$j < [llength $mod($m,hdrs)]} {incr j} {
            puts $cid "				<File"
            puts $cid "					RelativePath=\"$headerPrefix\\$mod($m,dir)\\[lindex $mod($m,headers) $j].h\">"
            puts $cid "				</File>"
        }

     	puts $cid "			</Filter>"
    }
    if {!$nodirgroups} {
        puts $cid "		</Filter>"
    }

    # add dummy.cc file, for specific platform libs which would end up being empty
    puts $cid "				<File"
    puts $cid "					RelativePath=\"$sourcePrefix\\dummy.cc\">"
    puts $cid "				</File>"
        
    # End Of File
    puts $cid "	</Files>"
    puts $cid "	<Globals>"
    puts $cid "	</Globals>"
    puts $cid "</VisualStudioProject>"

    close $cid
}


#--------------------------------------------------------------------
#   gen_exe_vcproj
#   Generate vcproj file for an executable target definition.
#   29-Jan-02	stki    created
#--------------------------------------------------------------------
proc gen_exe_vcproj {name} {
    global tar
    global num_tars
    global mod
    global num_mods
    global nomads_home
    global release_preprocessor_defs
    global debug_preprocessor_defs
    global libpath
    global headerPrefix
    global sourcePrefix
    global vstudioPrefix
    global binPrefix

    puts "-> gen_exe_vcproj $name"

    # create a local pkg_ file with Nebula module declarations
    set component_list [gen_component_list $name]
    gen_package_source $name $component_list

    set i [findtargetbyname $name]
    set platform $tar($i,platform)
    set preBuild $tar($i,prebuild_win32);
     
    # create list of link libs (win32 libs)
    set win32_libs [extract_libs $name libs_win32]
    set debug_win32_libs [extract_libs $name libs_win32_debug]
    set release_win32_libs [extract_libs $name libs_win32_release]

    # write .vcproj file
    set cid [open $vstudioPrefix/$name.vcproj w]
    
    puts $cid "<?xml version=\"1.0\" encoding = \"Windows-1252\"?>"
    puts $cid "<VisualStudioProject"
    puts $cid "	ProjectType=\"Visual C++\""
    puts $cid "	Version=\"7.00\""
    puts $cid "	Name=\"$name\""
    puts $cid "	SccProjectName=\"\""
    puts $cid "	SccLocalPath=\"\">"
    puts $cid "	<Platforms>"
    puts $cid "		<Platform"
    puts $cid "			Name=\"Win32\"/>"
    puts $cid "	</Platforms>"

    puts $cid "	<Configurations>"

    #--- Win32 Release Configuration ---
    puts $cid "		<Configuration"
    puts $cid "			Name=\"Release|Win32\""
    puts $cid "			OutputDirectory=\"$binPrefix\""
    puts $cid "			IntermediateDirectory=\".\\Release\\$name\""
    puts $cid "			ConfigurationType=\"1\""
    puts $cid "			UseOfMFC=\"0\""
    puts $cid "			ATLMinimizesCRunTimeLibraryUsage=\"FALSE\""
    puts $cid "         WholeProgramOptimization=\"TRUE\">"    
    puts $cid "			<Tool"
    puts $cid "				Name=\"VCCLCompilerTool\""
    puts $cid "				AdditionalOptions=\"\""
    puts $cid "				Optimization=\"4\""
    puts $cid "				GlobalOptimizations=\"TRUE\""
    puts $cid "				InlineFunctionExpansion=\"2\""
    puts $cid "				EnableIntrinsicFunctions=\"TRUE\""
    puts $cid "				FavorSizeOrSpeed=\"1\""
    puts $cid "				OmitFramePointers=\"TRUE\""
    puts $cid "				OptimizeForProcessor=\"2\""
    puts $cid "				AdditionalIncludeDirectories=\"$headerPrefix\""
    puts $cid "				PreprocessorDefinitions=\"N_STATIC;__WIN32__;$release_preprocessor_defs\""
    puts $cid "				StringPooling=\"TRUE\""
    puts $cid "				RuntimeLibrary=\"0\""
    puts $cid "				EnableFunctionLevelLinking=\"TRUE\""
    puts $cid "             ExceptionHandling=\"FALSE\""
    puts $cid "             BufferSecurityCheck=\"FALSE\""
    puts $cid "				UsePrecompiledHeader=\"0\""
    puts $cid "				AssemblerListingLocation=\".\\Release\\$name/\""
    puts $cid "				ObjectFile=\".\\Release\\$name/\""
    puts $cid "				ProgramDataBaseFileName=\".\\Release\\$name/\""
    puts $cid "				WarningLevel=\"3\""
    puts $cid "				SuppressStartupBanner=\"TRUE\""
    puts $cid "				CompileAs=\"2\"/>"
    puts $cid "			<Tool"
    puts $cid "				Name=\"VCLinkerTool\""
    puts $cid "				AdditionalDependencies=\"$win32_libs $release_win32_libs\""
    puts $cid "				OutputFile=\"$binPrefix\\$name.exe\""
    puts $cid "				LinkIncremental=\"0\""
    puts $cid "				SuppressStartupBanner=\"TRUE\""
    puts $cid "				AdditionalLibraryDirectories=\".\\Release,$libpath\""
    puts $cid "				ProgramDatabaseFile=\"$binPrefix\\$name.pdb\"/>"
    puts $cid "			<Tool"
    puts $cid "				Name=\"VCResourceCompilerTool\""
    puts $cid "				PreprocessorDefinitions=\"NDEBUG\""
    puts $cid "				Culture=\"1033\"/>"
    if {$preBuild != ""} {
        puts $cid "         <Tool"
        puts $cid "             Name=\"VCPreBuildEventTool\""
        puts $cid "             CommandLine=\"$preBuild\"/>"
    }
    puts $cid "		</Configuration>"

    #--- Win32 Debug Configuration ---
    puts $cid "		<Configuration"
    puts $cid "			Name=\"Debug|Win32\""
    puts $cid "			OutputDirectory=\"$binPrefix\""
    puts $cid "			IntermediateDirectory=\".\\Debug\\$name\""
    puts $cid "			ConfigurationType=\"1\""
    puts $cid "			UseOfMFC=\"0\""
    puts $cid "			ATLMinimizesCRunTimeLibraryUsage=\"FALSE\">"
    puts $cid "			<Tool"
    puts $cid "				Name=\"VCCLCompilerTool\""
    puts $cid "				AdditionalOptions=\"\""
    puts $cid "				Optimization=\"0\""
    puts $cid "				AdditionalIncludeDirectories=\"$headerPrefix\""
    puts $cid "				PreprocessorDefinitions=\"N_STATIC;__WIN32__;$debug_preprocessor_defs\""
    puts $cid "				BasicRuntimeChecks=\"0\""
    puts $cid "				RuntimeLibrary=\"1\""
    puts $cid "				UsePrecompiledHeader=\"0\""
    puts $cid "             BufferSecurityCheck=\"TRUE\""    
    puts $cid "				AssemblerListingLocation=\".\\Debug\\$name/\""
    puts $cid "				ObjectFile=\".\\Debug\\$name/\""
    puts $cid "             ExceptionHandling=\"FALSE\""
    puts $cid "				ProgramDataBaseFileName=\".\\Debug\\$name/\""
    puts $cid "				WarningLevel=\"3\""
    puts $cid "				SuppressStartupBanner=\"TRUE\""
    puts $cid "				DebugInformationFormat=\"4\""
    puts $cid "				CompileAs=\"2\"/>"
    puts $cid "			<Tool"
    puts $cid "				Name=\"VCCustomBuildTool\"/>"
    puts $cid "			<Tool"
    puts $cid "				Name=\"VCLinkerTool\""
    puts $cid "				AdditionalOptions=\"/MACHINE:IX86\""
    puts $cid "				AdditionalDependencies=\"$win32_libs $debug_win32_libs\""
    puts $cid "				OutputFile=\"$binPrefix\\$name.exe\""
    puts $cid "				LinkIncremental=\"2\""
    puts $cid "				SuppressStartupBanner=\"TRUE\""
    puts $cid "				AdditionalLibraryDirectories=\".\\Debug,$libpath\""
    puts $cid "				GenerateDebugInformation=\"TRUE\""
    puts $cid "				ProgramDatabaseFile=\"$binPrefix\\$name.pdb\"/>"
    puts $cid "			<Tool"
    puts $cid "				Name=\"VCResourceCompilerTool\""
    puts $cid "				PreprocessorDefinitions=\"_DEBUG\""
    puts $cid "				Culture=\"1033\"/>"
    if {$preBuild != ""} {
        puts $cid "         <Tool"
        puts $cid "             Name=\"VCPreBuildEventTool\""
        puts $cid "             CommandLine=\"$preBuild\"/>"
    }
    puts $cid "		</Configuration>"
    puts $cid "	</Configurations>"


    # Source code group
    puts $cid "	<Files>"
    puts $cid "		<Filter"
    puts $cid "			Name=\"Source Files\""
    puts $cid "			Filter=\"cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;cc\">"
    for {set k 0} {$k < [llength $tar($i,mods)]} {incr k} {
        set m [findmodbyname [lindex $tar($i,mods) $k]]

        # add regular sources
        for {set j 0} {$j < [llength $mod($m,srcs)]} {incr j} {
            puts $cid "			<File"
            puts $cid "				RelativePath=\"$sourcePrefix\\$mod($m,dir)\\[lindex $mod($m,files) $j].cc\">"

            #--- file configuration for Win32 Debug ---
            if {($platform != "all") && ($platform != "win32")} {
                # exclude this file from build
                puts $cid "					<FileConfiguration Name=\"Debug|Win32\" ExcludedFromBuild=\"TRUE\">"
            } else {        
                puts $cid "					<FileConfiguration Name=\"Debug|Win32\">"
            }
            puts $cid "					</FileConfiguration>"
            
            #--- file configuration for Win32 Release ---
            if {($platform != "all") && ($platform != "win32")} {
                # exclude this file from build
                puts $cid "					<FileConfiguration Name=\"Release|Win32\" ExcludedFromBuild=\"TRUE\">"
            } else {        
                puts $cid "					<FileConfiguration Name=\"Release|Win32\">"
            }
            puts $cid "					</FileConfiguration>"

            puts $cid "			</File>"
        }
        # add header files
        for {set j 0} {$j < [llength $mod($m,hdrs)]} {incr j} {
            puts $cid "			<File"
            puts $cid "				RelativePath=\"$headerPrefix\\$mod($m,dir)\\[lindex $mod($m,headers) $j].h\">"
            puts $cid "			</File>"
        }
    }
    puts $cid "		</Filter>"
    
    # resource file group
    if {[llength $tar($i,rsrc_win32)] > 0} {
        puts $cid "		<Filter"
        puts $cid "            Name=\"Resource Files\""
        puts $cid "           Filter=\"ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe\">"
        for {set j 0} {$j < [llength $tar($i,rsrc_win32)]} {incr j} {
            puts $cid "            <File"
            puts $cid "                RelativePath=\"$sourcePrefix\\[lindex $tar($i,rsrc_win32) $j].rc\">"
            # treat the resource file in a special way
            if [string match "*.rc" [lindex $tar($i,rsrc_win32) $j]] {
    		    puts $cid "		         <FileConfiguration"
    			puts $cid "		         		Name=\"Release|Win32\">"
    			puts $cid "		         		<Tool"
    			puts $cid "		         			Name=\"VCResourceCompilerTool\"/>"
    			puts $cid "		         	</FileConfiguration>"
    			puts $cid "		         	<FileConfiguration"
    			puts $cid "		         		Name=\"Debug|Win32\">"
    			puts $cid "		         		<Tool"
			    puts $cid "		         			Name=\"VCResourceCompilerTool\"/>"
		    	puts $cid "		         	</FileConfiguration>"
            }
            puts $cid "            </File>"
        }
        puts $cid "        </Filter>"
    }
    
    # End Of File
    puts $cid "	</Files>"
    puts $cid "	<Globals>"
    puts $cid "	</Globals>"
    puts $cid "</VisualStudioProject>"

    close $cid
}

#--------------------------------------------------------------------
#   gen_solution
#   29-Jan-02	stki    created
#   29-Mar-03   cubejk  added the handling for 'workspace' target.
#--------------------------------------------------------------------
proc gen_solution {main_target nodirgroups} {
    global tar
    global num_tars
    global nomads_home
    global vstudioPrefix
    global binPrefix
    
    puts "Generating Visual Studio .NET solution file $main_target.sln..."

    # find the main target, and generate the complete target list
    # from its dependencies
    set component_list [gen_component_list $main_target]
    
    puts "*** component list: $component_list"

    # generate package source file
    gen_package_source $main_target $component_list

    # for each target, generate a vcproj file, and if its a package target,
    # a .toc and package.cc file
    for {set i 0} {$i < [llength $component_list]} {incr i} {
        set t [findtargetbyname [lindex $component_list $i]]

        if {$tar($t,type) == "package"} {
            gen_package_vcproj $tar($t,name)
        } elseif {$tar($t,type) == "dll"} {
            gen_dll_vcproj $tar($t,name) dll $nodirgroups
        } elseif {$tar($t,type) == "exe"} {
            gen_exe_vcproj $tar($t,name)
        } elseif {$tar($t,type) == "mll"} {
            gen_dll_vcproj $tar($t,name) mll $nodirgroups
        } elseif {$tar($t,type) == "lib"} {
            gen_lib_vcproj $tar($t,name) $nodirgroups
        } elseif {$tar($t,type) == "workspace"} {
            gen_lib_vcproj $tar($t,name) $nodirgroups
        } else {
            puts "ERROR: UNKNOWN TARGET TYPE '$tar($t,type)'"
            exit
        }
        if {$tar($t,type) == "package"} {
            genpacksrc $tar($t,name)
        }
    }

    # write .sln file
    set cid [open $vstudioPrefix/$main_target.sln w]

    puts $cid "Microsoft Visual Studio Solution File, Format Version 7.00"

    # project list
    for {set i 0} {$i < [llength $component_list]} {incr i} {
        set t [findtargetbyname [lindex $component_list $i]]

    	# generate UUID for project
    	set tar($t,uuid) [string toupper [exec uuidgen]]    	
    	puts $cid "Project(\"{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}\") = \"$tar($t,name)\", \"$tar($t,name).vcproj\", \"{$tar($t,uuid)}\""
    	puts $cid "EndProject"
    }
    
    puts $cid "Global"
    puts $cid "	GlobalSection(SolutionConfiguration) = preSolution"
    puts $cid "		ConfigName.0 = Debug_Win32"
    puts $cid "		ConfigName.1 = Release_Win32"
    puts $cid "	EndGlobalSection"
    puts $cid "	GlobalSection(ProjectDependencies) = postSolution    "
    
    # project dependencies
    for {set i 0} {$i < [llength $component_list]} {incr i} {
        set t [findtargetbyname [lindex $component_list $i]]
        set depcount 0
        for {set k 0} {$k < [llength $tar($t,depends)]} {incr k} {
            set dtar [lindex $tar($t,depends) $k]
            set d [findtargetbyname $dtar]
            puts $cid "		{$tar($t,uuid)}.$depcount = {$tar($d,uuid)}"
            incr depcount
        }
    }
    
    puts $cid "	EndGlobalSection"
    puts $cid "	GlobalSection(ProjectConfiguration) = postSolution"
    for {set i 0} {$i < [llength $component_list]} {incr i} {
        set t [findtargetbyname [lindex $component_list $i]]
        puts $cid "		{$tar($t,uuid)}.Debug_Win32.ActiveCfg = Debug|Win32"
        puts $cid "		{$tar($t,uuid)}.Debug_Win32.Build.0 = Debug|Win32"
        puts $cid "		{$tar($t,uuid)}.Release_Win32.ActiveCfg = Release|Win32"
        puts $cid "		{$tar($t,uuid)}.Release_Win32.Build.0 = Release|Win32"
    }
    
    puts $cid "	EndGlobalSection"
    puts $cid "	GlobalSection(ExtensibilityGlobals) = postSolution"
    puts $cid "	EndGlobalSection"
    puts $cid "	GlobalSection(ExtensibilityAddIns) = postSolution"
    puts $cid "	EndGlobalSection"
    puts $cid "EndGlobal"

    close $cid

    puts "...done generating $main_target.sln."
}

#--------------------------------------------------------------------
#   EOF
#--------------------------------------------------------------------
