#====================================================================
#   UTILITY PROCEDURES
#
#   Generic handy stuff
#
#   Cobbled up from the original buildsys courtesy of Radon Labs
#   Copyright (c) 2003 Radon Labs
#
#   This file is licenesed under the terms of the Nebula License
#====================================================================

#--------------------------------------------------------------------
# procs:
#
#    addtolist destlist $srclist
#    translate_platdefs $platform_list
#    get_platform
#    findmodbyname      $name
#    findtargetbyname   $name
#    findbundlebyname   $name
#    findwspacebyname   $name
#    sort_mods          $mod_list
#    findrelpath        $frompath $topath
#    findrelpath_clean  $frompath $topath
#    cleanpath          $path
#    test_platform      $platform_list $platform_test
#    check_makedir      $path
#
#--------------------------------------------------------------------

global modulename_to_sortedindex

# This is a K combinator that is used to get the value of a list
# and unset the variable to avoid extra data copies.
proc K {a b} {
    return $a
}

#--------------------------------------------------------------------
#   addtolist dest_list $list
#
#   This strips each element out of $list and appends it with a
#   single trailing space to dest_list.
#
#   CAVEAT:  If dest_list has a trailing space then this function
#            will append an extra space before appending the first
#            element... don't think this really makes any difference
#            though, *shrug*, seems to be better than ending up with
#            not enough spaces. -- Vadim
#--------------------------------------------------------------------
proc addtolist {var list} {
    upvar $var v
    for {set i 0} {$i < [llength $list]} {incr i} {
        append v " " [lindex $list $i]
    }
}

#--------------------------------------------------------------------
#  translate_platdefs
#
#   takes a list of platform tags and translates them to valid
#   nebula preproc defs.
#
#  currently:
#   all =    [discarded]
#   win32 =  __WIN32__
#   linux =  __LINUX__
#   macosx = __MACOSX__
#--------------------------------------------------------------------
proc translate_platdefs { platform_list } {
    set retval ""

    foreach bit $platform_list {
        if {$bit == "all"} {
            continue
        } elseif {$bit == "win32"} {
            set bit __WIN32__
        } elseif {$bit == "linux"} {
            set bit __LINUX__
        } elseif {$bit == "macosx"} {
            set bit __MACOSX__
        } else {
           error "ERROR: unknown platform: $bit"
        }
        addtolist retval $bit
    }
    return $retval
}

#--------------------------------------------------------------------
#   get_platform()
#
#   Detect platform and return one of the following strings:
#   win32   -> Windows 95, 98, NT4, 2K, XP
#   linux   -> Unix variant
#   macosx  -> Mac OSX
#   unknown -> unknown
#--------------------------------------------------------------------
proc get_platform {} {
    global tcl_platform
    set p "unknown"
    if {$tcl_platform(platform) == "windows"} {
        if {$tcl_platform(os) == "Windows NT"} {
            if {$tcl_platform(osVersion) >= 5.0} {
                ::log::log debug "Windows 2000 detected"
            } else {
                ::log::log debug "Windows NT detected"
            }
        } else {
            ::log::log debug "Windows9x detected"
        }
        set p "win32"
    } elseif {$tcl_platform(platform) == "unix"} {
        if {$tcl_platform(os) == "Darwin"} {
            set p "macosx"
            ::log::log debug "Mac OS X detected"
        } else {
            set p "linux"
            ::log::log debug "Linux detected"
        }
    } else {
        set p "unknown"
        ::log::log debug "Unknown platform detected"
    }
    return $p
}

#--------------------------------------------------------------------
#   findmodbyname $name
#
#   Find module index by name.
#--------------------------------------------------------------------
proc findmodbyname {name} {
    global modsbyname
    if {[catch {set result $modsbyname($name)} err]} {
        error "ERROR: module '$name' not defined!"
    }
    return $result
    error "ERROR: module '$name' not defined!"
}

#--------------------------------------------------------------------
#   findtargetbyname $name
#
#   Find target index by name.
#--------------------------------------------------------------------
proc findtargetbyname { tname} {
    global tar
    global num_tars
    for {set i 0} {$i < $num_tars} {incr i} {
        if {$tname == $tar($i,name)} {
            return $i
        }
    }

    error "ERROR: target '$tname' not defined!"
}

#--------------------------------------------------------------------
#   findbundlebyname $name
#
#   Find target index by name.
#--------------------------------------------------------------------
proc findbundlebyname {name} {
    global bundle
    global num_bundles
    for {set i 0} {$i < $num_bundles} {incr i} {
        if {$name == $bundle($i,name)} {
            return $i
        }
    }
    error "ERROR: bundle '$name' not defined!"
}

#--------------------------------------------------------------------
#   findwspacebyname $name
#
#   Finds wspace index by name
#--------------------------------------------------------------------
proc findwspacebyname {name} {
    global wspace
    global num_wspaces
    for {set i 0} {$i < $num_wspaces} {incr i} {
        if {$name == $wspace($i,name)} {
            return $i
        }
    }
    error "ERROR: workspace '$name' not defined!"
}


#--------------------------------------------------------------------
#  sort_mods $modname_list
#
#  Sorts mods for nObject init dependence based on generated ancestors.
#--------------------------------------------------------------------

proc sort_mods { orig_list } {
    global modulename_to_sortedindex
    
    set sort_indices ""
    foreach mname $orig_list {
        lappend sort_indices [list $modulename_to_sortedindex($mname) $mname]
    }
    
    # now grab sorted names
    set sorted_list ""
    foreach i [lsort -unique -integer -index 0 $sort_indices] {
        set index [lindex $i 0]
        set name [lindex $i 1]
        lappend sorted_list $name
    }
    return $sorted_list    
}

#--------------------------------------------------------------------
#  sort_mods_old $modname_list
#
#  Sorts mods for nObject init dependence based on generated ancestors.
#  non nObject derived modules (generated) are placed first in the list
#  in all cases, but are not removed so that validation routines may
#  use this data
#
#  CAVEAT:  This does not sort modules based on setmoddepends{}
#           dependencies.
#--------------------------------------------------------------------
proc sort_mods_old { orig_list } {
    global mod
    global num_mods

    # sort the modules for proper dep order (for init)
    # eek!
    set mod_source {}
    set mod_sorted {}

    # dump all non nObject derived mods in the list
    for {set i 0} {$i < [llength $orig_list]} {incr i} {
        set idx [findmodbyname [lindex $orig_list $i]]
        if {$mod($idx,autonopak)} {
            lappend mod_sorted $mod($idx,name)
        } else {
            lappend mod_source $mod($idx,name)
        }
    }

    #handle nObject
    set i [lsearch $mod_source "nobject"]
    if {$i != -1} {
        lappend mod_sorted "nobject"
        set mod_source [lreplace [K $mod_source [set mod_source {}]] $i $i]
    }

    set count [llength $mod_sorted]
    while {$count < $num_mods } {
        set test [lindex $mod_sorted [expr $count - 1]]
        set i 0
        while {$i < [llength $mod_source]} {
            set source_idx [findmodbyname [lindex $mod_source $i]]
            if {[string match -nocase $test $mod($source_idx,ancestor)]} {
                lappend mod_sorted $mod($source_idx,name)
                set mod_source [lreplace [K $mod_source [set mod_source {}]] $i $i]
            } else {
                incr i
            }
        }
        incr count
    }

    return $mod_sorted
}

#--------------------------------------------------------------------
#  findrelpath $relfrom $relto
#
#  Will return the relative path from the relfrom to the relto
#  dirs.  Both inputs will be cleaned through cleanpath first and
#  the clenapath caveat that both input dirs be relative to home
#  applies. $relto may contain a filename as it's last path part.
#
#  The return is not passed through clean path and is a dead
#  relative path without a leading ./ or trailing /
#--------------------------------------------------------------------
proc findrelpath { relfrom relto } {
    return [findrelpath_clean [cleanpath $relfrom] [cleanpath $relto]]
}

#--------------------------------------------------------------------
#  findrelpath_clean $relfrom $relto
#
#  Helper method for findrelpath, but pulled out so that things
#  that know they have clean paths can save the work.
#--------------------------------------------------------------------
proc findrelpath_clean { relfrom relto } {
    set lfrom [split $relfrom /]
    set lto [split $relto /]

    #remove identical bits from both paths
    while {[lindex $lfrom 0] == [lindex $lto 0]} {
        set lfrom [lreplace [K $lfrom [set lfrom {}]]  0 0]
        set lto   [lreplace [K $lto [set lto {}]] 0 0]
    }

    set retval ""
    for {set i 0} {$i < [llength $lfrom]} {incr i} {
        if {[llength [lindex $lfrom $i]] > 0} {
            set retval [file join $retval ".."]
        }
    }
    set ret [file join $retval [join $lto /]]
    return $ret
}



#--------------------------------------------------------------------
# cleanpath $path
#
# We have to clean up our own paths as [file join] does some
# ignorant things.  All paths used in the buildsys are
# required to be relative from home and are required to be
# cleaned through this func before being used elsewhere. This
# will be tolerant of paths that have a file.ext on them.
#
# CAVEAT: This expects to receive a relative path from $home
# CAVEAT: This will always prepend './' to the front of the path
#--------------------------------------------------------------------
proc cleanpath {path} {
    set p [split $path /]
    set p1 {}

    # strip all '.' path bits - [file join] will not
    # this also has the effect of collapsing all '//' to '/'
    # this also has the effect of trimming '/' off the ends
    foreach bit $p {
        if {$bit != "." && $bit != ""} {
            lappend p1 $bit
        }
    }

    set p2 {}
    set prior false
    # walk the list and resolve all necessary '..' bits
    # [file join] does not....
    foreach t $p1 {
        if {$t == ".." && $prior} {
            #rewind
            set p2 [lreplace [K $p2 [set p2 {}]] end end]
            if {[lindex $p2 end] != ".." && [llength $p2] > 0} {
                set prior true
                continue
            }
            set prior false
            continue
        } elseif {$t != ".."} {
            lappend p2 $t
            set prior true
            continue
        }

        # .. and !prior
        lappend p2 $t
        set prior false
    }
    return ./[join $p2 /]
}

#--------------------------------------------------------------------
# test_platform
#
# searches a platform list for a valid matching platform and returns
# true|false to that effect
#--------------------------------------------------------------------
proc test_platform {platform_list platform_test} {
    foreach plat $platform_list {
        if {$plat == "all" || $plat == $platform_test} {
            return true
        }
    }
    return false
}

#--------------------------------------------------------------------
#  check_makedir $path
#
#  Simply checks for the existence of path and creates it if
#  necessary.
#--------------------------------------------------------------------
proc check_makedir {path} {
    global debug
    if {![file exists $path]} {
        if { $debug } {
			::log::log debug "-> Creating directory $path"
		}
        file mkdir $path
    } else {
		if { $debug } {
			::log::log debug "-> Directory $path exists"
		}
    }	
}


#----------------------------------------------------------------------------
# EOF
#----------------------------------------------------------------------------
