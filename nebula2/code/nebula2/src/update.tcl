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
gen_solution nebulalib 0
gen_solution tools 0
gen_makefile

puts "done."

#--------------------------------------------------------------------
#   EOF
#--------------------------------------------------------------------

