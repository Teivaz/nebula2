#-------------------------------------------------------------------------------
#   packsrc.tcl
#   ===========
#   Pack all files as defined in "filelist.txt" into a
#   zip archive. These should be the files that define "OpenNebula"
#   (which is the Nebula version on the Sourceforge CVS server).
#
#   This is a modified version of distribution.tcl 
#
#   (C) 2001 RadonLabs GmbH -- A.Weissflog
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
#   remove write protection from files
#-------------------------------------------------------------------------------
proc schreibschutzweg { startDir changedir } {
	set     pwd     [pwd]
    cd      $changedir
	foreach match [glob -nocomplain -- * ] {
		if 	[file isfile $match] { file attributes $match -readonly 0 }
	}
	foreach file  [glob -nocomplain -- *] {
		if 	[file isdirectory $file] { schreibschutzweg "$startDir/$file" $file }
	}
	cd $pwd    
return
}

#-------------------------------------------------------------------------------
#   Copy all public files.
#-------------------------------------------------------------------------------
proc kopie {} {
global sourcenames
global destnames
global counter
    for { set i 1 } { $i <= $counter } { incr i } {
        set  destfile   [ lindex $destnames   [ expr $i - 1 ] ]
        set  sourcefile [ lindex $sourcenames [ expr $i - 1 ] ]
        set  targetdir  [ file dirname $destfile ]
        file copy -force $sourcefile $targetdir
    }
return
}

#-------------------------------------------------------------------------------
#   Append-build 2 lists of all public files in a given directory. The first list 
#   contains the filenames of all source files, the second list contains the 
#   filenames of the corresponding 
#-------------------------------------------------------------------------------
proc public { ordner pattern } {
global home
global target
global sourcenames
global destnames
global counter
    file mkdir $target/$ordner
    cd   $ordner
	foreach match [glob -nocomplain -- $pattern] {
		if 	[file isfile $match] {
            incr    counter
            lappend sourcenames [pwd]/$match 
            lappend destnames   $target/$ordner/$match
		}
	}    
    cd $home
return
}

#--------------------------------------------------------------------------------
# main
#--------------------------------------------------------------------------------
set home [file join [file dir [info script]] .. .. .. ]
cd  $home
set distributiondir "$home/temp/dist"

file delete -force $home/temp
file mkdir $distributiondir 

set sourcenames ""
set destnames   ""
set counter     0
set target "$distributiondir/win32"

# --- read file list ---
puts   "  public source code"
source $home/filelist.txt

# --- copy stuff over ---
puts   "   $counter  source files public."
kopie

# --- remove write protection ---
puts    "Windows Version den Schreibschutz entfernen."
schreibschutzweg    $home/temp/dist/win32   $home/temp/dist/win32

# --- zip it ---
#cd          $home
#cd          $home/temp/dist/win32
#exec        zip  -9 -R n.zip *.*
#set         datumshort       [clock format [clock seconds] -format %d%b%y]
#file        rename -force n.zip $home/temp/neb$datumshort.zip
#puts        "done"

