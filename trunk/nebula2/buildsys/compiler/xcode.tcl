#-------------------------------------------------------------------------------
#    xcode.tcl
#
#   Cobbled up from the original buildsys courtesy of Radon Labs and 
#   Leaf Garland's Nebula2 VC7 project files.
#   Copyright (c) 2003 Radon Labs
#
#   This file is licenesed under the terms of the Nebula License
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
#   Globals
#-------------------------------------------------------------------------------

set global_counter 0
set targetids(0) 0
set filerefids(0) 0
set workspaceID 0

#-------------------------------------------------------------------------------
#   nextuuid
#-------------------------------------------------------------------------------

proc nextuuid {} {
    variable global_counter
    incr global_counter
    return [format "%024d" $global_counter]
}

#-------------------------------------------------------------------------------
#   emit_PBXContainerItemProxy $cid $targetID
#-------------------------------------------------------------------------------

proc emit_PBXContainerItemProxy { cid targetID } {
    set proxyID [nextuuid]
    variable workspaceID
    
    puts $cid "\t\t$proxyID = {"
    puts $cid "\t\t\tcontainerPortal = $workspaceID;"
    puts $cid "\t\t\tisa = PBXContainerItemProxy;"
    puts $cid "\t\t\tproxyType = 1;"
    puts $cid "\t\t\tremoteGlobalIDString = $targetID;"
    #puts $cid "\t\t\tremoteInfo = $targetName;"
    puts $cid "\t\t};"
    
    return $proxyID
}

#-------------------------------------------------------------------------------
#   emit_PBXTargetDependency $cid $targetID
#-------------------------------------------------------------------------------

proc emit_PBXTargetDependency { cid targetID } {
    set targetDependecyID [nextuuid]
    set targetProxy [emit_PBXContainerItemProxy $cid $targetID]
    
    puts $cid "\t\t$targetDependecyID = {"
    puts $cid "\t\t\tisa = PBXTargetDependency;"
    puts $cid "\t\t\ttarget = $targetID;"
    puts $cid "\t\t\ttargetProxy = $targetProxy;"
    puts $cid "\t\t};"
    
    return $targetDependecyID
}

#-------------------------------------------------------------------------------
#   emit_PBXBuildFile $cid $fileRef $settingsList
#-------------------------------------------------------------------------------

proc emit_PBXBuildFile { cid fileRef settingsList } {
    set buildFileID [nextuuid]
    array set settings $settingsList
    
    puts $cid "\t\t$buildFileID = {"
    puts $cid "\t\t\tisa = PBXBuildFile;"
    puts $cid "\t\t\tfileRef = $fileRef;"
    puts $cid "\t\t\tsettings = {"
    
    if { [array size settings] > 0 } {
        foreach setting [array names settings] {
            puts $cid "\t\t\t\t$setting = \"$settings($setting)\";"
        }
    }
    
    puts $cid "\t\t\t};"
    puts $cid "\t\t};"
    
    return $buildFileID
}

#-------------------------------------------------------------------------------
#   emit_PBXSourcesBuildPhase $cid $files
#-------------------------------------------------------------------------------

proc emit_PBXSourcesBuildPhase { cid files } {
    set buildPhaseID [nextuuid]
    puts $cid "\t\t$buildPhaseID = {"
    puts $cid "\t\t\tbuildActionMask = 2147483647;"
    puts $cid "\t\t\tfiles = ("
    
    if { [llength $files] > 0 } {
        foreach fileID $files {
            puts $cid "\t\t\t\t$fileID,"
        }
    }
    
    puts $cid "\t\t\t);"
    puts $cid "\t\t\tisa = PBXSourcesBuildPhase;"
    puts $cid "\t\t\trunOnlyForDeploymentPostprocessing = 0;"
    puts $cid "\t\t};"
    return $buildPhaseID
}

#-------------------------------------------------------------------------------
#   emit_PBXHeadersBuildPhase $cid $files
#-------------------------------------------------------------------------------

proc emit_PBXHeadersBuildPhase { cid files } {
    set buildPhaseID [nextuuid]
    puts $cid "\t\t$buildPhaseID = {"
    puts $cid "\t\t\tbuildActionMask = 2147483647;"
    puts $cid "\t\t\tfiles = ("
    
    if { [llength $files] > 0 } {
        foreach fileID $files {
            puts $cid "\t\t\t\t$fileID,"
        }
    }
    
    puts $cid "\t\t\t);"
    puts $cid "\t\t\tisa = PBXHeadersBuildPhase;"
    puts $cid "\t\t\trunOnlyForDeploymentPostprocessing = 0;"
    puts $cid "\t\t};"
    return $buildPhaseID
}

#-------------------------------------------------------------------------------
#   emit_PBXGroup $cid $name $path $refType $children
#-------------------------------------------------------------------------------

proc emit_PBXGroup { cid name path refType children } {
    variable filerefids
    set filerefids($path) [nextuuid]
    
    puts $cid "\t\t$filerefids($path) = {"
    puts $cid "\t\t\tchildren = ("
    if { [llength children] > 0 } {
        foreach child $children {
            puts $cid "\t\t\t\t$child,"
        }
    }
    puts $cid "\t\t\t);"
    puts $cid "\t\t\tisa = PBXGroup;"
    if { $name != "" } {
        puts $cid "\t\t\tname = $name;"
    }
    if { $path != "" } {
        puts $cid "\t\t\tpath = $path;"
    }
    puts $cid "\t\t\trefType = $refType;"
    if { $refType == 0 } {
        puts $cid "\t\t\tsourceTree = \"<absolute>\";"
    }
    if { $refType == 2 } {
        puts $cid "\t\t\tsourceTree = SOURCE_ROOT;"
    }
    if { $refType == 3 } {
        puts $cid "\t\t\tsourceTree = BUILT_PRODUCTS_DIR;"
    }
    if { $refType == 4 } {
        puts $cid "\t\t\tsourceTree = \"<group>\";"
    }
    puts $cid "\t\t};"
    
    return $filerefids($path)
}

#-------------------------------------------------------------------------------
#   emit_PBXFileReference $cid $name $path $refType
#-------------------------------------------------------------------------------

proc emit_PBXFileReference { cid name path refType } {
    variable filerefids
    set filerefids($path) [nextuuid]
    
    puts $cid "\t\t$filerefids($path) = {"
    puts $cid "\t\t\tisa = PBXFileReference;"
    if { $name != "" } {
        puts $cid "\t\t\tname = $name;"
    }
    if { $path != "" } {
        puts $cid "\t\t\tpath = $path;"
    }
    puts $cid "\t\t\trefType = $refType;"
    if { $refType == 0 } {
        puts $cid "\t\t\tsourceTree = \"<absolute>\";"
    }
    if { $refType == 2 } {
        puts $cid "\t\t\tsourceTree = SOURCE_ROOT;"
    }
    if { $refType == 3 } {
        puts $cid "\t\t\tsourceTree = BUILT_PRODUCTS_DIR;"
    }
    if { $refType == 4 } {
        puts $cid "\t\t\tsourceTree = \"<group>\";"
    }
    puts $cid "\t\t};"
    
    return $filerefids($path)
}

#-------------------------------------------------------------------------------
#   emit_PBXNativeTarget $cid $target
#-------------------------------------------------------------------------------

proc emit_PBXNativeTarget { cid target } {
    global platform
    variable targetids
    set fileIDs [list]
    set buildFileIDs [list]
    set dependencies [list]
    
    if {[lsearch [array names targetids] $target] < 0} {
        set targetids($target) [nextuuid]
    }

    set tardeps [get_tardeps $target]
    foreach tardep $tardeps {
        if {[lsearch [array names targetids] $tardep] < 0} {
            set targetids($tardep) [nextuuid]
        }
        lappend dependencies [emit_PBXTargetDependency $cid $targetids($tardep)]
    }

    set targetType [get_tartype $target]
    
    foreach module [get_tarmods $target] {
        if { [test_modplatform $module $platform ] } {
            set modType [get_modtype $module]
            set includePath [get_modpath $module inc]
            foreach file [get_modsources_dressed $module] {
                set name [join [list [lindex [split $file /] end] .cc] ""]
                set path [join [list $file .cc] ""]
                set fileID [emit_PBXFileReference $cid $name $path 2]
                set settings(COMPILER_FLAGS) ""
                if { $modType == "c" } {
                    append settings(COMPILER_FLAGS) "-x c "
                }
                if { $modType == "cpp" } {
                    append settings(COMPILER_FLAGS) "-x c++ "
                }
                append settings(COMPILER_FLAGS) "-DN_INIT=n_init_$module " 
                append settings(COMPILER_FLAGS) "-DN_NEW=n_new_$module "
                append settings(COMPILER_FLAGS) "-DN_INITCMDS=n_initcmds_$module "
                append settings(COMPILER_FLAGS) "-I$includePath "
                if { $targetType == "lib" } {
                    append settings(COMPILER_FLAGS) "-DN_STATIC "
                }
                if { $targetType == "exe" } {
                    append settings(COMPILER_FLAGS) "-DN_STATIC "
                }
                set buildFileID [emit_PBXBuildFile $cid $fileID [array get settings]]
                lappend fileIDs $fileID
                lappend buildFileIDs $buildFileID
            }
        }
    }
    
    set sourceGroupID [emit_PBXGroup $cid "src" "" 4 $fileIDs]
    lappend buildPhaseIDs [emit_PBXSourcesBuildPhase $cid $buildFileIDs]
    
    set fileIDs [list]
    set buildFileIDs [list]
    foreach module [get_tarmods $target] {
        if { [test_modplatform $module $platform ] } {
            foreach file [get_modheaders_dressed $module] {
                set name [join [list [lindex [split $file /] end] .h] ""]
                set path [join [list $file .h] ""]
                set fileID [emit_PBXFileReference $cid $name $path 2]
                set buildFileID [emit_PBXBuildFile $cid $fileID [list]]
                lappend fileIDs $fileID
                lappend buildFileIDs $buildFileID
            }
        }
    }
    
    set headerGroupID [emit_PBXGroup $cid "inc" "" 4 $fileIDs]
    lappend buildPhaseIDs [emit_PBXHeadersBuildPhase $cid $buildFileIDs]
    set groupID [emit_PBXGroup $cid $target "" 4 [list $sourceGroupID $headerGroupID]]
    
    if { $targetType == "lib" } {
        set productType "com.apple.product-type.library.static"
    }
    if { $targetType == "exe" } {
        set productType "com.apple.product-type.tool"
    }
    if { $targetType == "dll" } {
        set productType "com.apple.product-type.library.dynamic"
    }
    
    puts $cid "\t\t$targetids($target) = {"
    puts $cid "\t\t\tbuildPhases = ("
    
    if { [llength $buildPhaseIDs] > 0 } {
        foreach buildPhase $buildPhaseIDs {
            puts $cid "\t\t\t\t$buildPhase,"
        }
    }
    
    puts $cid "\t\t\t);"
    puts $cid "\t\t\tbuildSettings = {"
    puts $cid "\t\t\t\tGCC_PREPROCESSOR_DEFINITIONS = __MACOSX__;"
    puts $cid "\t\t\t};"
    puts $cid "\t\t\tbuildRules = ("
    
    puts $cid "\t\t\t);"
    puts $cid "\t\t\tdependencies = ("
    
    if { [llength $dependencies] > 0 } {
        foreach dep $dependencies {
            puts $cid "\t\t\t\t$dep,"
        }
    }
    
    puts $cid "\t\t\t);"
    puts $cid "\t\t\tisa = PBXNativeTarget;"
    puts $cid "\t\t\tname = $target;"
    puts $cid "\t\t\tproductName = $target;"
    #puts $cid "\t\t\tproductReference = ;"
    puts $cid "\t\t\tproductType = \"$productType\";"
    puts $cid "\t\t};"

    return [list $targetids($target) $groupID]
}

#-------------------------------------------------------------------------------
#   emit_PBXBuildStyle $cid $name
#-------------------------------------------------------------------------------

proc emit_PBXBuildStyle { cid name } {
    set uuid [nextuuid]
    
    puts $cid "\t\t$uuid = {"
    puts $cid "\t\t\tbuildSettings = {"
    if { $name == "Development" } {
        puts $cid "\t\t\t\tCOPY_PHASE_STRIP = NO;"
    }
    if { $name == "Deployment" } {
        puts $cid "\t\t\t\tCOPY_PHASE_STRIP = YES;"
    }
    puts $cid "\t\t\t};"
    puts $cid "\t\t\tbuildRules = ("
    puts $cid "\t\t\t);"
    puts $cid "\t\t\tisa = PBXBuildStyle;"
    puts $cid "\t\t\tname = $name;"
    puts $cid "\t\t};"
    
    return $uuid
}

#-------------------------------------------------------------------------------
#   emit_PBXProject $cid $workspace
#
#   this proc will call emit_PBXBuildStyle, emit_PBXGroup, emit_PBXNativeTarget
#-------------------------------------------------------------------------------

proc emit_PBXProject { cid workspace } {
    set groupIDs [list]
    set targetIDs [list]
    variable workspaceID

    set workspaceID [nextuuid]
    lappend buildStyleIDs [emit_PBXBuildStyle $cid "Development"] 
    lappend buildStyleIDs [emit_PBXBuildStyle $cid "Deployment"]
    
    foreach target [get_targets] {
        set temp [emit_PBXNativeTarget $cid $target]
        lappend targetIDs [lindex $temp 0]
        lappend groupIDs [lindex $temp 1]
    }
    
    set mainGroupID [emit_PBXGroup $cid $workspace "" 4 $groupIDs]
    
    puts $cid "\t\t$workspaceID = {"
    puts $cid "\t\t\tbuildSettings = {"
    puts $cid "\t\t\t};"
    puts $cid "\t\t\tbuildStyles = ("
    
    foreach buildStyle $buildStyleIDs {
        puts $cid "\t\t\t\t$buildStyle,"
    }
    
    puts $cid "\t\t\t);"
    puts $cid "\t\t\thasScannedForEncodings = 1;"
    puts $cid "\t\t\tisa = PBXProject;"
    #puts $cid "\t\t\tproductRefGroup = ;"
    puts $cid "\t\t\tprojectDirPath = \"../../\";"
    puts $cid "\t\t\ttargets = ("
    
    foreach target $targetIDs {
        puts $cid "\t\t\t\t$target,"
    }
    
    puts $cid "\t\t\t);"
    puts $cid "\t\t\tmainGroup = $mainGroupID;"
    puts $cid "\t\t};"
    
    return $workspaceID
}

#-------------------------------------------------------------------------------
#   gen_xcode $workspace
#-------------------------------------------------------------------------------

proc gen_xcode { workspace } {
    global platform
    global home
    global cur_workspacepath
    
    ::log::log info "Generating XCode file for project: $workspace"
    check_makedir $home/$cur_workspacepath/$workspace.xcode
    set cid [open [cleanpath $home/$cur_workspacepath/$workspace.xcode/project.pbxproj] w]
    
    puts $cid "// !$*UTF8*$!"
    puts $cid "{"
    puts $cid "\tarchiveVersion = 1;"
    puts $cid "\tclasses = {"
    puts $cid "\t};"
    puts $cid "\tobjectVersion = 39;"
    puts $cid "\tobjects = {"
    
    set rootObject [emit_PBXProject $cid $workspace]
    
    puts $cid "\t};"
    puts $cid "\trootObject = $rootObject;"
    puts $cid "}"
    
    close $cid
}

#-------------------------------------------------------------------------------
#   generate
#   Generates the project files for the specified workspaces
#-------------------------------------------------------------------------------

proc generate { wslist } {
    global platform
    
    if { $platform == "macosx" }  {

        ::log::log debug "Generate xcode files"

        set xcodepath   ./build/xcode
        set outpath     ./bin
        set interpath   $xcodepath/inter

        foreach workspace [get_workspaces $wslist]  {
            use_workspace $workspace $xcodepath $outpath $interpath
            gen_xcode $workspace
        }
    }
}

#-------------------------------------------------------------------------------
#   description
#   Output a description of this generator.                                     
#-------------------------------------------------------------------------------
proc description { } {
    puts "Support for Apple's XCode IDE."
}

#-------------------------------------------------------------------------------
#EOF
#-------------------------------------------------------------------------------
