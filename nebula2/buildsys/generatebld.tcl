#============================================================================
#  Data prep
#
#  This file contains the routines that generate data and produce the
#  workspace entries as needed for the compiler generators.
#
#  Cobbled up from the original buildsys courtesy of Radon Labs
#  Copyright (c) 2003 Radon Labs
#
#  This file is licenesed under the terms of the Nebula License
#============================================================================

# for global module order sorting - this should be moved to validate
# data as needed.
#----------------------------------------------------------------------------
# procs:
#
#    generateblddata
#    gen_ancestor  $moduleindex
#    gen_filelists $moduleindex
#    fixmods
#    fixbundles
#    fixtargets
#    fixworkspaces
#----------------------------------------------------------------------------

global modulename_to_sortedindex

#============================================================================
#  Modules generation
#============================================================================

#----------------------------------------------------------------------------
#   gen_ancestor $module
#
#   Automagically fixes up autonopak and ancestor fields
#   of mod the index handed off
#
#   CAVEAT:  All these tests break if the relevant line is indented
#   CAVEAT:  This requires that all the filenames (hdr,src) have
#            been fixed for the proper path already.
#----------------------------------------------------------------------------
proc gen_ancestor {i} {
    global mod
    global home

    foreach filename $mod($i,srcs) {

        if {[catch { set cid [open [cleanpath $home/[getfilenamewithextension $filename cc]] r] } result]} {
            ::log::log error "ERROR in module $mod($i,name): $result"
            return
        }

        while {![eof $cid]} {
            set line [gets $cid]

            # remove any characters that may confuse list manipulation routines...
            set line [string map { \( " " \) " " "," " " \{ " " \} " " \" ""} $line]

            #valid Macro statements
            # nNebulaRootClass
            # nNebulaClass
            # nNebulaClassStaticInit
            # nNebulaScriptClass
            # nNebulaScriptClassStaticInit

            set idx [lindex $line 0]

            if {$idx == "nNebulaRootClass"} {
                set mod($i,ancestor)   ""
                set mod($i,autonopak)  false
                break
            }

            if {[string match -nocase "nNebula*Class*" $idx]} {
                set mod($i,ancestor)   [lindex $line 2]
                set mod($i,autonopak)  false
                break
            }

        }
        close $cid
    }
}

#----------------------------------------------------------------------------
#   gen_filelists $module
#
#   Prep the source, header path/names for use in the compiler generators.
#
#   CAVEAT:  No file extensions are added so that compiler generators may
#            add needed extensions as desired.
#----------------------------------------------------------------------------
proc gen_filelists {i hintdir} {
    global mod
    global home
    global num_mods
    global global_gendeps

    set moddir $mod($i,dir)
    set founddir ""
    set found false

    # get the trunk dir at ????/$dir/(inc/src)/$mod($i,dir)

    # First, check the hint dir
    if { ![string equal "invalid" $hintdir] } {
        if { [file exists $hintdir/inc/$moddir] && [file isdirectory $hintdir/inc/$moddir]} {
            set founddir $hintdir
            if { [string match "*contrib*" $hintdir] } {
                set trunkdir contrib/[lindex [file split $hintdir] end]
            } else {
                set trunkdir [lindex [file split $hintdir] end]
            }
            set found true
        }

        if { $found == false } {
            if { [file exists $hintdir/src/$moddir] && [file isdirectory $hintdir/src/$moddir]} {
                set founddir $hintdir
                if { [string match "*contrib*" $hintdir] } {
                    set trunkdir contrib/[lindex [file split $hintdir] end]
                } else {
                    set trunkdir [lindex [file split $hintdir] end]
                }
                set found true
            }
        }
    }
    set startpath [string trim $home '/']
    if { $found == false } {
        # The hint wasn't correct, so now search the core directories
        foreach ext [glob -nocomplain -directory $startpath/code */] {
            #ext will be the trunk path
            set ext [string trim $ext '/']

            # Skip CVS and contrib dirs in this pass
            if { [string equal contrib [lindex [file split $ext] end]] } {
                continue
            }
            if { [string equal CVS [lindex [file split $ext] end]] } {
                continue
            }

            #need to search for $ext/inc/$dir or $ext/src/dir directly
            if { [file exists $ext/inc/$moddir] && [file isdirectory $ext/inc/$moddir]} {
                set founddir $ext
                set trunkdir [lindex [file split $ext] end]
                set found true
                break
            }

            if { [file exists $ext/src/$moddir] && [file isdirectory $ext/src/$moddir]} {
                set founddir $ext
                set trunkdir [lindex [file split $ext] end]
                set found true
                break
            }
        }
    }

    if { $found == false } {
        # We didn't find it, so now search contrib...
        foreach ext [glob -nocomplain -directory $startpath/code/contrib */] {
            #ext will be the trunk path
            set ext [string trim $ext '/']

            # Skip the CVS dir
            if { [string equal CVS [lindex [file split $ext] end]] } {
                continue
            }

            #need to search for $ext/inc/$dir or $ext/src/$dir directly
            if { [file exists $ext/inc/$moddir] && [file isdirectory $ext/inc/$moddir]} {
                set founddir $ext
                set trunkdir contrib/[lindex [file split $ext] end]
                set found true
                break
            }

            if { [file exists $ext/src/$moddir] && [file isdirectory $ext/src/$moddir]} {
                set founddir $ext
                set trunkdir contrib/[lindex [file split $ext] end]
                set found true
                break
            }
        }
    }

    set mod($i,trunkdir) $trunkdir

    # generate object file lists
    # XXX: Why do we set objs here?
    set mod($i,objs) {}
    set srcslist {}
    foreach file $mod($i,files) {
        set cur_file "./code/$trunkdir/src/$moddir/$file"
        lappend srcslist [cleanpath $cur_file]
    }
    set mod($i,srcs) $srcslist

    # append header files to 'hdrs'
    set hdrlist {}
    foreach header $mod($i,headers) {
        set cur_hdr "./code/$trunkdir/inc/$moddir/$header"
        lappend hdrlist [cleanpath $cur_hdr]
    }
    set mod($i,hdrs) $hdrlist

    return $founddir
}

#----------------------------------------------------------------------------
# fixmods
#
#----------------------------------------------------------------------------
proc fixmods { } {
    global mod
    global num_mods
    global modulename_to_sortedindex

    ::log::log info "\n**** Fixing modules"

    set hintdir "invalid"
    set lmods {}
    for {set i 0} {$i < $num_mods} {incr i} {
         ::log::log debug "  $mod($i,name)"
         lappend lmods $mod($i,name)
         set hintdir [gen_filelists $i $hintdir]
         gen_ancestor $i

         #Fix up the forcenopkg indice for use
         #If true then no n_addmodules entry should
         #be made.
         if {$mod($i,autonopak)} {
             set mod($i,forcenopkg) true
         }

         foreach moddep $mod($i,moduledeps) {
             #this will bail if it fails
             findmodbyname $moddep
         }
    }
    
    set sorted_mods [sort_mods_old $lmods]
    set len_smods [llength $sorted_mods]
    for {set i 0} {$i < $len_smods} {incr i} {
        set modname [lindex $sorted_mods $i]
        array set modulename_to_sortedindex [list $modname $i]
    }
}



#============================================================================
#  Bundles
#============================================================================

#----------------------------------------------------------------------------
#    fixbundles
#----------------------------------------------------------------------------
proc fixbundles { } {
    global bundle
    global num_bundles

    ::log::log info "\n**** Fixing bundles"
    for {set i 0} {$i < $num_bundles} {incr i} {
        ::log::log debug "  $bundle($i,name)"
        foreach module $bundle($i,modules) {
            findmodbyname $module
        }
        foreach target $bundle($i,targets) {
            findtargetbyname $target
        }
    }
}



#============================================================================
#  Targets
#============================================================================

#----------------------------------------------------------------------------
#    fixtargets
#----------------------------------------------------------------------------
proc fixtargets { } {
    global bundle
    global mod
    global tar
    global num_tars
    global home

    ::log::log info "\n**** Fixing targets"
    for {set i 0} {$i < $num_tars} {incr i} {
        ::log::log debug "  $tar($i,name)"

        # fix up the mergedmods list
        foreach bit $tar($i,bundles) {
            set ext [findbundlebyname $bit]
            addtolist tar($i,targetdeps) $bundle($ext,targets)
            addtolist tar($i,mergedmods) $bundle($ext,modules)
        }
        addtolist tar($i,mergedmods) $tar($i,modules)
        set tar($i,mergedmods) [lsort -unique $tar($i,mergedmods)]

        #check if one of the modules has set a moduledefinition file
        foreach module $tar($i,mergedmods) {
            global mod
            #we only search until we found the first, because more than one
            #is not supported
            set m [findmodbyname $module]
            if { $mod($m,moddeffile) != "" } {
                #found a filename
                set filename "./code/$mod($m,trunkdir)/src/$mod($m,dir)/$mod($m,moddeffile).def"
                if { [file exists $filename] } {
                    #found the file
                    set tar($i,moddeffile) $filename
                    break
                } else {
                    ::log::log debug "Warning: Module Definition File set but file not found $filename."
                }
            }
        }

        #fetch up a complete lib list for this target
        foreach bit $tar($i,mergedmods) {
            set ext [findmodbyname $bit]
            addtolist tar($i,libs_win32)  $mod($ext,libs_win32)
            addtolist tar($i,libs_win32_debug)  $mod($ext,libs_win32_debug)
            addtolist tar($i,libs_win32_release)  $mod($ext,libs_win32_release)
            addtolist tar($i,libs_linux)  $mod($ext,libs_linux)
            addtolist tar($i,libs_macosx) $mod($ext,libs_macosx)

        }
        set tar($i,libs_win32) [lsort -unique $tar($i,libs_win32)]
        set tar($i,libs_win32_debug) [lsort -unique $tar($i,libs_win32_debug)]
        set tar($i,libs_win32_release) [lsort -unique $tar($i,libs_win32_release)]
        set tar($i,libs_linux) [lsort -unique $tar($i,libs_linux)]
        set tar($i,libs_macosx) [lsort -unique $tar($i,libs_macosx)]
    }

    check_makedir $home/build/pkg
    ::log::log info "\n*** Generating resource files"
    write_resfiles
    ::log::log info "\n*** Generating package files"
    write_pkgfiles
    #this is all we can do until the workspace is resolved
}

#============================================================================
#  Workspace
#
#  All the fun stuff happens here - This is the big codger that the
#  makefile generators have to deal with for everything except file and
#  module names.
#
#  Targets are merged up into their respective workspace (after a fashion)
#  and bundles are ditched at this point.  For sanity all pkg files will
#  be generated and kept with the relevant workspace under the dir 'pkg'
#============================================================================

#----------------------------------------------------------------------------
#    fixworkspaces
#----------------------------------------------------------------------------
proc fixworkspaces { wslist } {
    global wspace
    global num_wspaces
    global tar
    global num_tars
    global mod
    global home

    ::log::log info "\n**** Fixing workspaces"    
    for {set i 0} {$i < $num_wspaces} {incr i} {
        #format
        # wspace(idx,tar#name#,#fields#)
        if {$wslist != ""} {
            if {[lsearch $wslist $wspace($i,name)] == -1} {
                ::log::log debug "-- $wspace($i,name) not in workspace list ($wslist)"
                continue
            }
        }
        ::log::log debug "  $wspace($i,name)"
        foreach tarname $wspace($i,targets) {
            set idx [findtargetbyname $tarname]
            set wspace($i,$tarname,type)           $tar($idx,type)
            set wspace($i,$tarname,rtti)           $tar($idx,rtti)
            set wspace($i,$tarname,exceptions)     $tar($idx,exceptions)
            set wspace($i,$tarname,icon)           $tar($idx,icon)
            set wspace($i,$tarname,resource_win32) $tar($idx,resource_win32)
            set wspace($i,$tarname,dllextension)   $tar($idx,dllextension)
            set wspace($i,$tarname,moddeffile)     $tar($idx,moddeffile)
            set wspace($i,$tarname,modules)        $tar($idx,mergedmods)

            set wspace($i,$tarname,libs_win32_release) $tar($idx,libs_win32_release)
            set wspace($i,$tarname,libs_win32_debug)   $tar($idx,libs_win32_debug)
            set stdlibs $tar($idx,libs_win32)
            addtolist wspace($i,$tarname,libs_win32_release) $stdlibs
            addtolist wspace($i,$tarname,libs_win32_debug)   $stdlibs

            set wspace($i,$tarname,libs_linux)  $tar($idx,libs_linux)
            set wspace($i,$tarname,libs_macosx) $tar($idx,libs_macosx)
            set wspace($i,$tarname,targetdeps)  $tar($idx,targetdeps)
            set wspace($i,$tarname,platform)    $tar($idx,platform)
            set wspace($i,$tarname,defs)        ""

            # fix up the preproc defines
            lappend wspace($i,$tarname,defs) $wspace($i,globaldefs)
            foreach tdef $wspace($i,targetdefs) {
                if {[string match [lindex $tdef 0] $tarname]} {
                    lappend wspace($i,$tarname,defs) [lrange $tdef 1 2]
                }
            }

            # 3. percolate libs
            if {$tar($idx,type) != "lib"} {
                # all tardeps are expected to be libs (no dlls)
                # and libs are expected to depend on each other

                foreach dep $tar($idx,targetdeps) {
                    set depidx [findtargetbyname $dep]
                    addtolist wspace($i,$tarname,libs_win32_release) $tar($depidx,libs_win32)
                    addtolist wspace($i,$tarname,libs_win32_debug) $tar($depidx,libs_win32)

                    addtolist wspace($i,$tarname,libs_win32_release) $tar($depidx,libs_win32_release)
                    addtolist wspace($i,$tarname,libs_win32_debug) $tar($depidx,libs_win32_debug)

                    addtolist wspace($i,$tarname,libs_linux) $tar($depidx,libs_linux)
                    addtolist wspace($i,$tarname,libs_macosx) $tar($depidx,libs_macosx)
                }
                set wspace($i,$tarname,libs_win32_release) [lsort -unique $wspace($i,$tarname,libs_win32_release)]
                set wspace($i,$tarname,libs_win32_debug) [lsort -unique $wspace($i,$tarname,libs_win32_debug)]
                set wspace($i,$tarname,libs_linux) [lsort -unique $wspace($i,$tarname,libs_linux)]
                set wspace($i,$tarname,libs_macosx) [lsort -unique $wspace($i,$tarname,libs_macosx)]
            }
        }

    }
}

#----------------------------------------------------------------------------
# EOF
#----------------------------------------------------------------------------
