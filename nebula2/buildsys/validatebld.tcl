#============================================================================
#  Data Validation
#
#  This file contains all the validation routines not explicitly 
#  required to generate the data for the compiler generator api.
#  All heavy and serious validation should go here.
#
#  This file requires:
#    'global home' be set to the nebula 2 root dir
#
#   Cobbled up from the original buildsys courtesy of Radon Labs
#   Copyright (c) 2003 Radon Labs
#
#   This file is licenesed under the terms of the Nebula License
#============================================================================

#----------------------------------------------------------------------------
# procs:
#
#    dump_data $filename
#    dump_api_Data $filename
#----------------------------------------------------------------------------

#----------------------------------------------------------------------------
#  dump_data
#
#  dumps raw data from the loadbld.tcl structures to file for debugging
#  and verification.  Does not belong in validateblddata.
#----------------------------------------------------------------------------
proc dump_data { filename } {
    global home
    global wspace
    global num_wspaces
    global tar
    global num_tars
    global bundle
    global num_bundles
    global mod
    global num_mods
    global mod_sorted
    
    
    set cid [open [cleanpath $home/buildsys/$filename.log] w]
    
    puts $cid "Build Data dump"
    puts $cid ""
    
    puts $cid "--------------------------------------------------------"
    puts $cid " Workspaces"
    puts $cid "--------------------------------------------------------"
    puts $cid ""
    
    for {set i 0} {$i < $num_wspaces} {incr i} {
        puts $cid "Name: $wspace($i,name)"
        puts $cid "Bldfile: $wspace($i,bldfile)"

        puts $cid "  Project file dir:  $wspace($i,dir)"
        puts $cid "  Binary output dir: $wspace($i,binarydir)"
        puts $cid "  Scratch build dir: $wspace($i,libdir)"
        puts $cid "  Annotation:"
        
        puts $cid ""
        puts $cid $wspace($i,annotate)        
        puts $cid ""
        
        puts $cid "  Global preprocessor defines:"
        foreach def $wspace($i,globaldefs) {
            puts $cid "      [lindex $def 0] = [lindex $def 1]"
        }
        
        puts $cid "  Target specific preprocessor defines:"
        foreach def $wspace($i,targetdefs) {
            puts $cid "      [lindex $def 0]: [lindex $def 1] = [lindex $def 2]"
        }
        
        puts $cid "  Targets to build:"
        foreach target $wspace($i,targets) {
            puts $cid "        $target"
        }
        puts $cid ""
        puts $cid "--------------------------------------------------------"
        puts $cid ""
    }
    
    puts $cid ""
    puts $cid ""
    puts $cid ""
    puts $cid "--------------------------------------------------------"
    puts $cid " Targets"
    puts $cid "--------------------------------------------------------"
    puts $cid ""
    
    for {set i 0} {$i < $num_tars} {incr i} {
        puts $cid "Name: $tar($i,name)"
        puts $cid "Bldfile: $tar($i,bldfile)"

        puts $cid "  Type:     $tar($i,type)"
        puts $cid "  Platform: $tar($i,platform)"
        
        puts $cid "  Annotation:"
        
        puts $cid ""
        puts $cid $tar($i,annotate)        
        puts $cid ""
        
        puts $cid "  Moduledefinition File: $tar($i,moddeffile)"
        puts $cid "  DLL Extension: $tar($i,dllextension)"
        puts $cid "  Enabled RTTI: $tar($i,rtti)"
        puts $cid "  Enabled Exceptions: $tar($i,exceptions)"

        puts $cid "  Modules:"
        foreach module $tar($i,modules) {
            puts $cid "      $module"
        }
        
        puts $cid "  Bundles:"
        foreach bndl $tar($i,bundles) {
            puts $cid "      $bndl"
        }
        
        puts $cid "  Target Dependencies:"
        foreach tardep $tar($i,targetdeps) {
            puts $cid "      $tardep"
        }
        
        puts $cid "  Generated Items - generatebld.tcl:"
        puts $cid ""
        
        puts $cid "  Merged modules:"
        foreach mmod $tar($i,mergedmods) {
            puts $cid "      $mmod"
        }
        
        puts $cid "  Merged win32 libs:"
        foreach lib $tar($i,libs_win32) {
            puts $cid "      $lib"
        }
        
        puts $cid "  Merged win32 debug libs:"
        foreach lib $tar($i,libs_win32_release) {
            puts $cid "      $lib"
        }
        
        puts $cid "  Merged win32 release libs:"
        foreach lib $tar($i,libs_win32_debug) {
            puts $cid "      $lib"
        }
        
        puts $cid "  Merged linux libs:"
        foreach lib $tar($i,libs_linux) {
            puts $cid "      $lib"
        }
        
        puts $cid "  Merged macosx libs:"
        foreach lib $tar($i,libs_macosx) {
            puts $cid "      $lib"
        }
        
        puts $cid ""
        puts $cid "--------------------------------------------------------"
        puts $cid ""
    }
    
    puts $cid ""
    puts $cid ""
    puts $cid ""
    puts $cid "--------------------------------------------------------"
    puts $cid " Bundles"
    puts $cid "--------------------------------------------------------"
    puts $cid ""
    
    for {set i 0} {$i < $num_bundles} {incr i} {
        puts $cid "Name: $bundle($i,name)"
        puts $cid "Bldfile: $bundle($i,bldfile)"
        
        puts $cid "  Annotation:"
        
        puts $cid ""
        puts $cid $bundle($i,annotate)        
        puts $cid ""
        
        puts $cid "  Modules:"
        foreach module $bundle($i,modules) {
            puts $cid "      $module"
        }
        
        puts $cid "  Targets:"
        foreach target $bundle($i,targets) {
            puts $cid "      $target"
        }

        puts $cid ""
        puts $cid "--------------------------------------------------------"
        puts $cid ""
    }

    puts $cid ""
    puts $cid ""
    puts $cid ""
    puts $cid "--------------------------------------------------------"
    puts $cid " Modules"
    puts $cid "--------------------------------------------------------"
    puts $cid ""

    for {set i 0} {$i < $num_mods} {incr i} {
        puts $cid "Name: $mod($i,name)"
        puts $cid "Bldfile: $mod($i,bldfile)"
        puts $cid "  Type:         $mod($i,type)"
        puts $cid "  Dir:          $mod($i,dir)"
        puts $cid "  Platforms:    $mod($i,platform)"
        puts $cid "  Force No Pak: $mod($i,forcenopkg)"
        puts $cid "  Trunkdir:     $mod($i,trunkdir)"
        puts $cid "  ModDefFile:   $mod($i,moddeffile)"
        
        puts $cid "  Files:"
        foreach file $mod($i,files) {
            puts $cid "      $file"
        }
        
        puts $cid "  Headers:"
        foreach header $mod($i,headers) {
            puts $cid "      $header"
        }
        
        puts $cid "  win32 libs:"
        foreach lib $mod($i,libs_win32) {
            puts $cid "      $lib"
        }

        puts $cid "  win32 libs release:"
        foreach lib $mod($i,libs_win32_release) {
            puts $cid "      $lib"
        }

        puts $cid "  win32 libs debug:"
        foreach lib $mod($i,libs_win32_debug) {
            puts $cid "      $lib"
        }

        puts $cid "  linux libs:"
        foreach lib $mod($i,libs_linux) {
            puts $cid "      $lib"
        }

        puts $cid "  macosx libs:"
        foreach lib $mod($i,libs_macosx) {
            puts $cid "      $lib"
        }
        
        puts $cid "  Module dependencies:"
        foreach module $mod($i,moduledeps) {
            puts $cid "      $module"
        }
        
        puts $cid "  Generated Items - generatebld.tcl:"
        puts $cid ""
        
        puts $cid "  Module has kernel:   $mod($i,kernel)"
        puts $cid "  Module is not NOH:   $mod($i,autonopak)"
        puts $cid "  Module derived from: $mod($i,ancestor)"
        
        puts $cid "  Fixed Sources:"
        foreach src $mod($i,srcs) {
            puts $cid "      $src"
        }
        
        puts $cid "  Fixed Headers:"
        foreach hdr $mod($i,hdrs) {
            puts $cid "      $hdr"
        }
        
        puts $cid ""
        puts $cid "--------------------------------------------------------"
        puts $cid ""
    }
    
    puts $cid ""
    puts $cid ""
    puts $cid "--------------------------------------------------------"
    puts $cid " EoF"
    puts $cid "--------------------------------------------------------"
    puts $cid ""

    close $cid
}

#----------------------------------------------------------------------------
#  dump_api_data
#
#  Dumps the data collected and collated for the compiler API
#----------------------------------------------------------------------------
proc dump_api_data { filename } {
    global home
    global wspace
    global num_wspaces
    global cur_workspace
    
    set cid [open [cleanpath $home/buildsys/$filename.log] w]
    
    puts $cid "Build Data dump"
    puts $cid ""
    
    puts $cid "--------------------------------------------------------"
    puts $cid " Workspaces"
    puts $cid "--------------------------------------------------------"
    puts $cid ""

    for {set i 0} {$i < $num_wspaces} {incr i} {
        set_currentworkspace $wspace($i,name)
        puts $cid "Name: $wspace($i,name)"

        puts $cid "  Project file dir:  $wspace($i,dir)"
        puts $cid "  Binary output dir: $wspace($i,binarydir)"
        puts $cid "  Scratch build dir: $wspace($i,libdir)"
        
        puts $cid "  Annotation:"
        
        puts $cid ""
        puts $cid $wspace($i,annotate)        
        puts $cid ""
        
       puts $cid "  Global preprocessor defines:"
        foreach def $wspace($i,globaldefs) {
            puts $cid "      [lindex $def 0] = [lindex $def 1]"
        }
        
        puts $cid "  Target specific preprocessor defines:"
        foreach def $wspace($i,targetdefs) {
            puts $cid "      [lindex $def 0]: [lindex $def 1] = [lindex $def 2]"
        }
        
        puts $cid "Targets for workspace $wspace($i,name)"
        puts $cid "--------------------------------------------------------"
        
       foreach target $wspace($i,targets) {
            puts $cid ""
            puts $cid "    Name:  $target"
            puts $cid "    Type:  [get_tartype $target]"
            puts $cid ""
            puts $cid "    RTTI:       [get_rtti $target]"
            puts $cid "    Exceptions: [get_exceptions $target]"
            puts $cid "    ModDefFile: [get_moddeffile $target]"
            puts $cid "    DLL Extension: [get_dllextension $target]"
            puts $cid ""
            puts $cid "    Optional preprocessor defines:"
            foreach define [get_tardefs $target] {
                puts $cid "        [lindex $define 0] = [lindex $define 1]"
            }

            puts $cid ""
            puts $cid "    Target Modules:"


            foreach module [sort_mods [get_tarmods $target]] {
                puts $cid "        $module"
            }


            puts $cid ""
            puts $cid "    Win32 Release Libs:"
            foreach lib [get_win32libs_release $target] {
                puts $cid "        $lib"
            }
            
            puts $cid ""
            puts $cid "    Win32 Debug Libs:"
            foreach lib [get_win32libs_debug $target] {
                puts $cid "        $lib"
            }
            
            puts $cid ""
            puts $cid "    Linux Libs:"
            foreach lib [get_linuxlibs $target] {
                puts $cid "        $lib"
            }
            
            puts $cid ""
            puts $cid "    Mac OSX Libs:"
            foreach lib [get_osxlibs $target] {
                puts $cid "        $lib"
            }
            
            puts $cid " pakmod modules (ancestor dep ordered)"
            foreach module $wspace($i,$target,pakmods) {
                puts $cid "        $module"
            }
            
            puts $cid "-------------------------------"
       }
            
    }    

    puts $cid ""
    puts $cid ""
    puts $cid "--------------------------------------------------------"
    puts $cid " EoF"
    puts $cid "--------------------------------------------------------"
    puts $cid ""

    close $cid
}

#----------------------------------------------------------------------------
# EOF
#----------------------------------------------------------------------------
