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
#   27-Mar-03   cubejk  ignore xbox includes
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
        if {[catch { set cid [open $filename r] } result]} {
            puts stderr "While processing $filename:\n"
            puts stderr "    $result\n"
            return $initial_list
        }
        set inside_xbox 0
        set inside_dep_ignore 0
        while {![eof $cid]} {
            set line [gets $cid]
            # remove any characters that may confuse list manipulation routines...
            set line [string map {\{ " " \} " " \" "_"} $line]

            #ignore '#ifdef __XBxX__'
            if { ([lindex $line 0] == "#ifdef" && [lindex $line 1] == "__XBxX__") 
                  || ($inside_xbox == 1) } {
                set inside_xbox 1
                if { ([lindex $line 0] == "\#endif") } {
                    set inside_xbox 0
                }
            } elseif { ([lindex $line 0] == "\#define" && [lindex $line 1] == "dep_ignore")
                       || ($inside_dep_ignore == 1) } {
                set inside_dep_ignore 1
                if { ([lindex $line 0] == "\#undef") && [lindex $line 1] == "dep_ignore" } {
                    set inside_dep_ignore 0
                }
            } else {
                # check for valid "#include" statement
                if {[lindex $line 0] == "\#include"} {
                    if {[string match "xbox/*.h" $line]} {
                        # Skip over ..
                    } elseif {[string match "*_*/*.h_*" $line]} { 
                        # check if file is already in depend list
                        set fname [string trim [lindex $line 1] "_"]
                        if {[lsearch $l $fname] == -1} {
                            # nope, add it to depend list and recurse down
                            #puts "Found: $fname in $filename:$i."
                            append l $fname " "
                            set l [get_depends "$includePath/$fname" "$includePath" $l]
                        }
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

