#--------------------------------------------------------------------
#   update.tcl
#   ==========
#--------------------------------------------------------------------

source $env(RL_HOME)/bin/makemake.tcl

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
	gen_solution nebulalib 0
	gen_solution tools 0	
}
gen_makefile
# These must come after the solution files
# as we add some defauly libs to libs_win32
gen_workspace nebulalib 0
gen_workspace tools 0

puts "done."

#--------------------------------------------------------------------
#   EOF
#--------------------------------------------------------------------

