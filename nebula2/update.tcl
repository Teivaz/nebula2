#----------------------------------------------------------------------------
#   update.tcl
#   ==========
#   regenerates the project/make files and what not
#----------------------------------------------------------------------------

#----------------------------------------------------------------------------
# Globals
#----------------------------------------------------------------------------
global home
global platform
global vstudioPrefix

set vstudioPrefix "code/nebula2/vstudio"

# Setup the home directory we are working from
set home [file dir [info script]]/
cd $home
set home ./

#----------------------------------------------------------------------------
#  Included scripts
#----------------------------------------------------------------------------
source $home/buildsys/buildutil.tcl
source $home/buildsys/loadbld.tcl
source $home/buildsys/generatebld.tcl
source $home/buildsys/validatebld.tcl
source $home/buildsys/compilerapi.tcl

# generators
set gen_list ""
foreach gen [glob -nocomplain $home/buildsys/compiler/*.tcl] {
    set gen_name [file tail [file rootname $gen]]
    lappend gen_list $gen_name
    namespace eval $gen_name {
        source $gen
    }
}
puts $gen_list

set platform [get_platform]

#----------------------------------------------------------------------------
#  Prep the pak file data
#----------------------------------------------------------------------------

# Load the data
loadbldfiles
dump_data loadbld

# Massage data (generation, etc.)
fixmods
fixbundles
fixtargets
fixworkspaces
dump_data generatebld
dump_api_data generateapibld



# ETERNAL TODO: Properly validate data 
puts "\n**** Validating bld files"
 
dump_api_data validatebld   


puts "\n->Done loading bld files."
#----------------------------------------------------------------------------
#  Call the generators
#----------------------------------------------------------------------------
puts ""
puts ":: GENERATING buildfiles..."
puts "==========================="

foreach gen $gen_list {
    namespace inscope $gen generate
}

# VC7
#----------------------------------------------------------------------------
#puts "Looking for uuidgen...."
#if { [catch { exec uuidgen }] } {
#	puts "uuidgen.exe not found skipping Visual Studio Solutions."
#} else {
#	puts "uuidgen.exe found"
#	gen_solution
#}


# Unix
#----------------------------------------------------------------------------
#gen_makefile


# VC6
#----------------------------------------------------------------------------
# These must come after the solution files
# as we add some default libs to libs_win32
#gen_workspace

puts "\ndone."

#----------------------------------------------------------------------------
#   EOF
#----------------------------------------------------------------------------

