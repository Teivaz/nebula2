#--------------------------------------------------------------------
#   update.tcl
#   ==========
#--------------------------------------------------------------------

set home [file join [file dir [info script]] .. .. .. ]

source $home/bin/makemake.tcl

# set config variables
set headerPrefix "..\\inc"
set sourcePrefix "..\\src"
set vstudioPrefix "../vstudio"
set binPrefix "../../../bin/win32"
set global_gendeps 0

#--------------------------------------------------------------------
# read any extension packages, and create list of targets
#--------------------------------------------------------------------
foreach ext [glob -nocomplain *.epk] {
    source $ext
}

#--------------------------------------------------------------------
# generate VisualStudio workspace for the public Nebula distribution
# (without WFTools, see below)
#--------------------------------------------------------------------
puts ""
puts ":: GENERATING buildfiles..."
puts "==========================="
puts "Looking for uuidgen...."
if { [catch { exec uuidgen }] } {
	puts "uuidgen.exe not found skipping Visual Studio Solutions."
} else {
	puts "uuidgen.exe found"
	# Do the default two first
	gen_solution nebulalib 0
	gen_solution tools 0	

	# Then, Check the rest of the data for type "workspace" - and if they're a workspace, call gen_solution on them
	global tar
	global num_tars
	for {set i 0} {$i < $num_tars} {incr i} {
		if {"workspace" == $tar($i,type) && "nebulalib" != $tar($i,name) && "tools" != $tar($i,name)} {
			gen_solution $tar($i,name) 0
		}
	}
}
gen_makefile
# These must come after the solution files
# as we add some default libs to libs_win32
# First add the default workspaces
gen_workspace nebulalib 0
gen_workspace tools 0

# Then, Check the rest of the data for type "workspace" - and if they're a workspace, call gen_workspace on them
global tar
global num_tars
for {set i 0} {$i < $num_tars} {incr i} {
	if {"workspace" == $tar($i,type) && "nebulalib" != $tar($i,name) && "tools" != $tar($i,name)} {
		gen_workspace $tar($i,name) 0
	}
}

puts "done."

#--------------------------------------------------------------------
#   EOF
#--------------------------------------------------------------------

