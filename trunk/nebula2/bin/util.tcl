#-------------------------------------------------------------------------------
#   util.tcl
#
#   Standalone utility function lib.
#
#   (C) 2002 RadonLabs GmbH
#-------------------------------------------------------------------------------

#--------------------------------------------------------------------
#   get_platform()
#   Detect platform and return one of the following strings:
#   win9x   -> Windows 95 and 98
#   winnt   -> Windows NT 4
#   win2k   -> Windows 2000
#   unix    -> Unix variant
#   unknown -> unknown
#--------------------------------------------------------------------
proc get_platform {} {
    global tcl_platform
    set p "unknown"
    if {$tcl_platform(platform) == "windows"} {
        if {$tcl_platform(os) == "Windows NT"} {
            if {$tcl_platform(osVersion) >= 5.0} {
                set p "win2k"
                puts "Windows 2000 detected"
            } else {
                set p "winnt"
                puts "Windows NT detected"
            }
        } else {
            set p "win9x"
            puts "Windows9x detected"
        }
    } elseif {$tcl_platform(platform) == "unix"} {
        set p "unix"
        puts "Unix detected"
    } else {
        set p "unknown"
        puts "Unknown platform detected"
    }
    return $p
}

