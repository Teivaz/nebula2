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
global debug
global verbose

set workspaces ""
set buildgen ""
set listworkspaces false
set listcompilers false
set verbose false
# deal with args
set i 0
while {$i < $argc} {
    if {[lindex $argv $i] == "-help" || [lindex $argv $i] == "--help"} {
        puts "Usage:"
        puts "   update.tcl \[-help\] \[-build buildsystem\] \[workspacelist\]"
        puts "              \[-listworkspaces\] \[-listcompilers\]"
        puts "              \[-verbose\]"
        puts ""
        puts " -help: Display this information"
        puts " -build: Specify which build system generator should be used."
        puts "         If this isn't specified, then all available generators"
        puts "         will be run."
        puts " \[workspacelist\]: The list of workspaces that should be"
        puts "          generated. By default, all workspaces will be"
        puts "          generated."
        puts " -listworkspaces: List the available workspaces."
        puts " -listcompilers: List the build systems that are supported."
        puts " -verbose: Print extra information (but not full debug"
        puts "          information). This is off by default but basic"
        puts "          information will still be printed."
        exit
    } elseif {[lindex $argv $i] == "-build"} {
        set i [expr $i + 1]
        set buildgen [lindex $argv $i]
    } elseif {[lindex $argv $i] == "-listworkspaces"} {
        set listworkspaces true
    } elseif {[lindex $argv $i] == "-listcompilers"} {
        set listcompilers true
    } elseif {[lindex $argv $i] == "-verbose"} {
        set verbose true
    } else {
        lappend workspaces [lindex $argv $i]
    }
    set i [expr $i + 1]
}

set vstudioPrefix "code/nebula2/vstudio"

# Set this to true if you want extra debug info to get printed
set debug false

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
source $home/buildsys/helperfunctions.tcl

# generators
set gen_list ""
foreach gen [glob -nocomplain $home/buildsys/compiler/*.tcl] {
    set gen_name [file tail [file rootname $gen]]
    lappend gen_list $gen_name
    namespace eval $gen_name {
        source $gen
    }
}

if { $listcompilers } {
    puts "Supported build systems:"
    foreach gen $gen_list {
        puts "   - $gen"
    }
    puts ""
    foreach gen $gen_list {
        puts "$gen:"
        if {[catch { namespace inscope $gen description } result]} {
            puts "  ERROR: $result"
        }
        puts "---"
        puts ""
    }
    exit
}

set platform [get_platform]

#----------------------------------------------------------------------------
#  Prep the pak file data
#----------------------------------------------------------------------------

# Load the data
loadbldfiles
if { $listworkspaces } {
    puts ""
    puts "Available workspaces:"
    for {set i 0} {$i < $num_wspaces} {incr i} {
        puts "  $wspace($i,name): $wspace($i,annotate)"
    }
    exit
}
if { $debug } {
    dump_data loadbld
}

# Massage data (generation, etc.)
fixmods
fixbundles
fixtargets
fixworkspaces $workspaces
if { $debug } {
    dump_data generatebld
    dump_api_data generateapibld
}

# ETERNAL TODO: Properly validate data 
puts "\n**** Validating bld files"
 
if { $debug } {
    dump_api_data validatebld   
}
add_pkgs $workspaces

puts "\n->Done loading bld files."
#----------------------------------------------------------------------------
#  Call the generators
#----------------------------------------------------------------------------
puts ""
puts ":: GENERATING buildfiles..."
puts "==========================="

if {$buildgen == ""} {
    puts "Running all generators..."
    foreach gen $gen_list {
        namespace inscope $gen generate $workspaces
    }
} else {
    puts "Running $buildgen generator"
    namespace inscope $buildgen generate $workspaces
}

puts "\ndone."

#----------------------------------------------------------------------------
#   EOF
#----------------------------------------------------------------------------

