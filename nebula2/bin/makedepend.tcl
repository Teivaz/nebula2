#--------------------------------------------------------------------
#   makedepend.tcl
#   ==============
#   Generate list of dependencies for a given source file.
#   (C) 2000 A.Weissflog
#--------------------------------------------------------------------


#--------------------------------------------------------------------
#   structure of dep_cache:
#   dep_cache($file) { list of dependencies for that file}
#--------------------------------------------------------------------
global dep_cache

#--------------------------------------------------------------------
#   get_depends $filename 
#   Return list of files that depend on $filename. Will only
#   consider non-system include statement (those where the filename
#   is enclosed in ""'s). Recurses into each depend file..
#
#   11-Mar-00   floh    created
#--------------------------------------------------------------------
proc get_depends {filename includePath initial_list} {
    global dep_cache

    set l $initial_list

    # check if file is already in dep_cache...
    if {[catch {set dummy $dep_cache($filename)} err] == 0} {
        for {set i 0} {$i < [llength $dep_cache($filename)]} {incr i} {
            set fname [lindex $dep_cache($filename) $i]
            if {[lsearch $l $fname] == -1} {
                append l $fname " "
            }
        }
    } else {
        # no cache hit, process files normally...
        set cid [open $filename r]
        while {![eof $cid]} {
            set line [gets $cid]

            # remove any characters that may confuse list manipulation routines...
            set line [string map {\{ " " \} " " \" "_"} $line]

            # check for valid "#include" statement
            if {[lindex $line 0] == "\#include"} {
                if {[string match "*_*/*.h_*" $line]} { 
                    # check if file is already in depend list
                    set fname [string trim [lindex $line 1] "_"]
                    if {[lsearch $l $fname] == -1} {
                        # nope, add it to depend list and recurse down
                        append l $fname " "
                        set l [get_depends "$includePath/$fname" "$includePath" $l]
                    }
                }
            }
        }
        close $cid

        # add new cache entry
        set dep_cache($filename) $l
    }
    return $l
}

#--------------------------------------------------------------------
#   EOF
#--------------------------------------------------------------------

