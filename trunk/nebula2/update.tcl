#----------------------------------------------------------------------------
#   update.tcl
#   ==========
#   regenerates the project/make files and what not
#----------------------------------------------------------------------------

#----------------------------------------------------------------------------
# Globals
#----------------------------------------------------------------------------
global home
global debug
global platform
global verbose
global data_loaded
global loaded_generators
global usegui

#package require profiler
#::profiler::init

set chosen_workspaces {}
set buildgen ""
set verbose false
set usegui false
set data_loaded false

# Set this to true if you want extra debug info to get printed
set debug false

# Setup the home directory we are working from
set home [file dir [info script]]/
cd $home
set home ./


#----------------------------------------------------------------------------
#  Included scripts
#----------------------------------------------------------------------------
source $home/buildsys/lib/log.tcl
source $home/buildsys/buildutil.tcl
source $home/buildsys/loadbld.tcl
source $home/buildsys/generatebld.tcl
source $home/buildsys/validatebld.tcl
source $home/buildsys/compilerapi.tcl
source $home/buildsys/helperfunctions.tcl

# set default verbosity level
proc configure_log_suppression {} {
    global verbose

    # allow all
    ::log::lvSuppressLE emergency 0
    if (!$verbose) {
        # suppress debug
        ::log::lvSuppressLE debug 1
    }
}

configure_log_suppression

set platform [get_platform]

# Load the generator scripts
set loaded_generators ""
foreach gen [glob -nocomplain $home/buildsys/compiler/*.tcl] {
    set gen_name [file tail [file rootname $gen]]
    lappend loaded_generators $gen_name
    namespace eval $gen_name {
        source $gen
    }
}

proc configure_gui {} {
    # The window
    frame .window

    # The menu bar
    menu .window.menubar -type menubar
    .window.menubar add cascade -label Run -menu .window.menubar.run \
        -underline 0
    menu .window.menubar.run -tearoff 0
    .window.menubar.run add checkbutton -label "Verbose" -underline 0 \
        -variable verbose
    .window.menubar.run add separator
    .window.menubar.run add command -label "Select Compiler..." -underline 7 \
        -command { select_generator }
    .window.menubar.run add command -label "Select Workspaces..." -underline 7 \
        -command { select_workspaces }
    .window.menubar.run add separator
    .window.menubar.run add command -label "Run" -underline 0 \
        -command { run_buildsystem }
    .window.menubar.run add separator
    .window.menubar.run add command -label Exit -underline 1 -command { exit }

    . configure -menu .window.menubar

    # The text area
    text .window.text -wrap none -yscrollcommand ".window.v_scroll set" \
        -xscrollcommand ".window.h_scroll set"
    scrollbar .window.v_scroll -command ".window.text yview"
    scrollbar .window.h_scroll -command ".window.text xview" -orient horizontal

    # Layout
    pack .window.v_scroll -side right -fill y
    pack .window.h_scroll -side bottom -fill x
    pack .window.text -side left -fill both -expand 1
    pack .window -fill both -expand 1

    # Set up tags for the log levels in the text widget
    foreach level $::log::levels {
        .window.text tag config $level -foreground [::log::lv2color $level]
    }

    focus .window
}

proc log_message {level msg} {
    .window.text insert end "$msg\n" $level
    .window.text yview moveto 1
    update
}

proc load_data {} {
    global data_loaded

    loadbldfiles
    set data_loaded true
}

proc list_workspaces {} {
    global data_loaded
    global num_wspaces
    global wspace

    configure_log_suppression

    if { $data_loaded == false } {
        load_data
    }

    ::log::log info ""
    ::log::log info "Available workspaces:"
    for {set i 0} {$i < $num_wspaces} {incr i} {
        ::log::log info "  $wspace($i,name): $wspace($i,annotate)"
    }
    exit
}

proc list_generators {} {
    global usegui

    configure_log_suppression

    set generator_info [get_generator_info]

    ::log::log info "Supported build systems:"
    foreach generator $generator_info {
        ::log::log info "[lindex $generator 0]:"
        ::log::log info "    [lindex $generator 1]"
    }
    exit
}

proc select_workspaces {} {
    global data_loaded
    global wspace
    global num_wspaces
    global chosen_workspaces

    configure_log_suppression

    if { $data_loaded == false } {
        load_data
    }

    if {[winfo exists .workspaces]} {
        focus .workspaces
        return
    }
    toplevel .workspaces
    frame .workspaces.f
    label .workspaces.f.l -text "Available workspaces:"
    pack .workspaces.f.l -side top -anchor w
    frame .workspaces.f.b
    button .workspaces.f.b.cancel -text "Cancel" \
        -command { destroy .workspaces }
    button .workspaces.f.b.accept -text "Select Workspaces" \
        -command {
            global chosen_workspaces
            global wspace

            set chosen_workspaces {}
            set active [.workspaces.f.f.list curselection]
            foreach item $active {
                lappend chosen_workspaces $wspace($item,name)
            }
            destroy .workspaces
        }
    pack .workspaces.f.b.cancel -side left
    pack .workspaces.f.b.accept -side right
    pack .workspaces.f.b -side bottom -anchor e
    frame .workspaces.f.f
    listbox .workspaces.f.f.list -yscroll ".workspaces.f.f.scroll set" \
        -selectmode extended -width 20
    scrollbar .workspaces.f.f.scroll -orient vertical \
        -command ".workspaces.f.f.list yview"
    pack .workspaces.f.f.scroll -side right -fill y
    pack .workspaces.f.f.list -side left -fill both
    pack .workspaces.f.f -side left -fill both -anchor nw
    message .workspaces.f.description -text "" -width 150 -anchor nw
    pack  .workspaces.f.description -side right -fill both -anchor nw \
        -expand 1

    bind .workspaces.f.f.list <ButtonRelease-1> {
        global wspace
        set active [.workspaces.f.f.list index @%x,%y]
        set description $wspace($active,annotate)
        if {$description eq ""} {
            set description "No description provided."
        }
        .workspaces.f.description configure -text $description
    }
    pack .workspaces.f -fill both -expand 1

    set idx 0
    set activeIdxList {}
    for {set i 0} {$i < $num_wspaces} {incr i} {
        .workspaces.f.f.list insert end $wspace($i,name)
        if {[lsearch -exact $chosen_workspaces $wspace($i,name)] >= 0} {
            lappend activeIdxList $idx
        }
        incr idx
    }
    foreach activeIdx $activeIdxList {
        .workspaces.f.f.list selection set $activeIdx
        .workspaces.f.f.list activate $activeIdx
        .workspaces.f.f.list see $activeIdx
    }

    focus .workspaces
}

proc select_generator {} {
    global buildgen
    global usegui

    configure_log_suppression

    set generator_info [get_generator_info]

    if {[winfo exists .generators]} {
        focus .generators
        return
    }
    toplevel .generators
    frame .generators.f
    label .generators.f.l -text "Available build system support:"
    pack .generators.f.l -side top -anchor w
    frame .generators.f.b
    button .generators.f.b.cancel -text "Cancel" \
        -command { destroy .generators }
    button .generators.f.b.accept -text "Select Compiler" \
        -command {
            global buildgen

            set active [.generators.f.f.list curselection]
            set generator_info [get_generator_info]
            set buildgen [lindex [lindex $generator_info $active] 0]
            destroy .generators
        }
    pack .generators.f.b.cancel -side left
    pack .generators.f.b.accept -side right
    pack .generators.f.b -side bottom -anchor e
    frame .generators.f.f
    listbox .generators.f.f.list -yscroll ".generators.f.f.scroll set" \
        -selectmode browse -width 20
    scrollbar .generators.f.f.scroll -orient vertical \
        -command ".generators.f.f.list yview"
    pack .generators.f.f.scroll -side right -fill y
    pack .generators.f.f.list -side left -fill both
    pack .generators.f.f -side left -fill both -anchor nw
    message .generators.f.description -text "" -width 150 -anchor nw
    pack  .generators.f.description -side right -fill both -anchor nw \
        -expand 1

    bind .generators.f.f.list <<ListboxSelect>> {
        set active [.generators.f.f.list curselection]
        set generator_info [get_generator_info]
        set description [lindex [lindex $generator_info $active] 1]
        .generators.f.description configure -text $description
    }
    pack .generators.f -fill both -expand 1

    set idx 0
    set activeIdx -1
    foreach generator $generator_info {
        .generators.f.f.list insert end [lindex $generator 0]
        if {[lindex $generator 0] eq $buildgen} {
            set activeIdx $idx
        }
        incr idx
    }
    if {$activeIdx != -1} {
        .generators.f.f.list selection set $activeIdx
        .generators.f.f.list activate $activeIdx
        .generators.f.f.list see $activeIdx
    }

    focus .generators
}

proc run_buildsystem {} {
    global buildgen
    global chosen_workspaces

    configure_log_suppression

    if {[catch {run_buildsystem_worker $chosen_workspaces $buildgen} result]} {
        global errorInfo
        ::log::log error $result
        ::log::log debug "*** TRACE ***"
        ::log::log debug $errorInfo
        if {[::log::lvIsSuppressed debug]} {
            ::log::log error "To get more information about this error, exit update.tcl and run again with 'Verbose' (-verbose) turned on."
        }
    }
}

proc run_buildsystem_worker {workspaces buildgen} {
    global debug
    global data_loaded
    global loaded_generators

    if { $data_loaded == false } {
        load_data
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
        dump_api_data generateapibld $workspaces
    }

    # ETERNAL TODO: Properly validate data 
    ::log::log info "\n**** Validating bld files"
 
    if { $debug } {
        dump_api_data validatebld $workspaces
    }

    ::log::log info "\n->Done loading bld files."
    #---------------------------------------------------------------------------
    #  Call the generators
    #---------------------------------------------------------------------------
    ::log::log info ""
    ::log::log info ":: GENERATING buildfiles..."
    ::log::log info "==========================="

    if {$buildgen == ""} {
        ::log::log debug "Running all generators..."
        foreach gen $loaded_generators {
            namespace inscope $gen generate $workspaces
        }
    } else {
        ::log::log debug "Running $buildgen generator"
        namespace inscope $buildgen generate $workspaces
    }

    #puts [::profiler::print]
    ::log::log info "\ndone."
}

# deal with args
set i 0
while {$i < $argc} {
    if {[lindex $argv $i] == "-help" || [lindex $argv $i] == "--help"} {
        puts "Usage:"
        puts "   update.tcl \[-help\] \[-build buildsystem\] \[workspacelist\]"
        puts "              \[-listworkspaces\] \[-listcompilers\]"
        puts "              \[-verbose\] \[-gui\]"
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
        puts " -gui: Use the GUI."
        exit
    } elseif {[lindex $argv $i] == "-build"} {
        set i [expr $i + 1]
        set buildgen [lindex $argv $i]
    } elseif {[lindex $argv $i] == "-listworkspaces"} {
        list_workspaces
    } elseif {[lindex $argv $i] == "-listcompilers"} {
        list_generators
    } elseif {[lindex $argv $i] == "-verbose"} {
        set verbose true
        ::log::lvSuppressLE debug 0
    } elseif {[lindex $argv $i] == "-gui"} {
        set usegui true
    } else {
        lappend chosen_workspaces [lindex $argv $i]
    }
    set i [expr $i + 1]
}

if { [string first wish [info nameofexecutable]] != -1 } {
    set usegui true
}

if { $usegui } {
    package require Tk 8.4

    configure_gui

    ::log::lvCmdForall log_message
} else {
    run_buildsystem
}

#----------------------------------------------------------------------------
#   EOF
#----------------------------------------------------------------------------

