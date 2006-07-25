#-------------------------------------------------------------------------------
#   startup.tcl
#   ===========
#   Contains script procedures which are called at startup and cleanup.
#   (C) 2004 RadonLabs GmbH
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
#   OnStartup
#
#   This procedure is called right after application startup. Use it
#   to initialize Nebula assigns.
#-------------------------------------------------------------------------------
proc OnStartup {} {

    puts "startup.tcl: OnStartup called"

    set oldCwd [psel]
    sel /sys/servers/file2

    # setup assigns
    set proj [.manglepath "proj:"]
    set home [.manglepath "home:"]

    .setassign "data"     "$proj/data/"
    .setassign "export"   "$proj/export/"

    .setassign "renderpath" "$home/data/shaders"
    .setassign "scripts"    "data:scripts"
    .setassign "physics"    "export:physics/"
    .setassign "meshes"     "export:meshes/"
    .setassign "textures"   "export:textures/"
    .setassign "anims"      "export:anims/"
    .setassign "gfxlib"     "export:gfxlib/"
    .setassign "lights"     "export:lightlib/"
    .setassign "levels"     "export:levels/"
    .setassign "sound"      "export:audio/"
    .setassign "charlib"    "gfxlib:characters/"
    .setassign "save"       "user:options"

    sel $oldCwd
}

#-------------------------------------------------------------------------------
#   OnShutdown
#
#   This procedure is called when the application shuts down.
#-------------------------------------------------------------------------------
proc OnShutdown {} {
    puts "startup.tcl: OnShutdown called"
}

#-------------------------------------------------------------------------------
#   OnGraphicsStartup
#
#   This method is called right after graphics is initialized.
#-------------------------------------------------------------------------------
proc OnGraphicsStartup {} {

    set oldCwd [psel]
    sel /sys/servers/file2

    if {[exists /sys/servers/gfx]} {
        set featureSet [/sys/servers/gfx.getfeatureset]
         if {($featureSet == "dx9") || ($featureSet == "dx9flt")} {
            # use HDR renderer for dx9 graphics cards (the current
            # HDR renderer doesn't use float render targets anymore)
            /sys/servers/scene.setrenderpathfilename "renderpath:dx9hdr_renderpath.xml"
        } else {
            # non-DX9 hardware, use fixed function render path
            /sys/servers/scene.setrenderpathfilename "renderpath:dx7_renderpath.xml"
        }
    }
    sel $oldCwd
}

#-------------------------------------------------------------------------------
#   OnGraphicsShutdown
#
#   This method is called right after graphics is initialized.
#-------------------------------------------------------------------------------
proc OnGraphicsShutdown {} {
    # empty
}

#-------------------------------------------------------------------------------
#   OnViewerMapInput
#
#   This procedure is called by nviewer when input should be mapped.
#-------------------------------------------------------------------------------
proc OnViewerMapInput {} {
    set cwd [psel]
    sel /sys/servers/input
    .beginmap

    .map "keyb0:space.down" "reset"
    .map "keyb0:esc.down" "script:/sys/servers/gui.togglesystemgui"
    .map "keyb0:f1.down" "mayacontrols"
    .map "keyb0:f2.down" "flycontrols"
    .map "keyb0:f4.down" "script:ToggleSceneVisualization"
    .map "keyb0:f5.down" "script:/sys/servers/capture.toggle"
    .map "keyb0:f6.down" "script:/sys/servers/time.resettime"
    .map "keyb0:f7.down" "script:/sys/servers/time.stoptime"
    .map "keyb0:f8.down" "script:/sys/servers/time.starttime"
    .map "keyb0:f12.down" "script:/sys/servers/capture.savetiledscreenshot 2 2"

    .map "relmouse0:btn0.pressed" "look"
    .map "relmouse0:btn1.pressed" "zoom"
    .map "relmouse0:btn2.pressed" "pan"
    .map "relmouse0:+zbtn.down"   "ScrollDown"
    .map "relmouse0:-zbtn.down"   "ScrollUp"
    .map "relmouse0:-x" "left"
    .map "relmouse0:+x" "right"
    .map "relmouse0:-y" "up"
    .map "relmouse0:+y" "down"
    .map "relmouse0:-z" "zoomIn"
    .map "relmouse0:+z" "zoomOut"
    .map "keyb0:w.pressed"              "moveForward"
    .map "keyb0:a.pressed"              "moveLeft"
    .map "keyb0:s.pressed"              "moveBackward"
    .map "keyb0:d.pressed"              "moveRight"
    .map "keyb0:up.pressed"             "moveForward"
    .map "keyb0:left.pressed"           "moveLeft"
    .map "keyb0:right.pressed"          "moveRight"
    .map "keyb0:down.pressed"           "moveBackward"
    .endmap
    sel $cwd
}

#-------------------------------------------------------------------------------
#   OnMapInput
#
#   This procedure is called by Mangalore when input should be mapped.
#-------------------------------------------------------------------------------
proc OnMapInput {} {
    set oldCwd [psel]
    sel /sys/servers/input

    .beginmap
    .map "keyb0:f1.down" "script:/sys/servers/gui.togglesystemgui"
    .map "keyb0:f2.down" "togglePhysicsVisualization"
    .map "keyb0:f3.down" "toggleGraphicsVisualization"
    .map "keyb0:f4.down" "script:ToggleSceneVisualization"
    .map "keyb0:f5.down" "script:/sys/servers/capture.toggle"
    .map "keyb0:f7.down" "script:/sys/servers/time.stoptime"
    .map "keyb0:f8.down" "script:/sys/servers/time.starttime"
    .map "keyb0:f12.down" "script:/sys/servers/capture.savetiledscreenshot 4 4"

    .map "relmouse0:btn2.down"          "vwrOn"
    .map "relmouse0:btn2.up"            "vwrOff"
    .map "relmouse0:-x"                 "vwrLeft"
    .map "relmouse0:+x"                 "vwrRight"
    .map "relmouse0:-y"                 "vwrUp"
    .map "relmouse0:+y"                 "vwrDown"
    .map "keyb0:space.down"             "vwrReset"

    .map "relmouse0:btn0.down"          "lmbDown"
    .map "relmouse0:btn0.up"            "lmbUp"
    .map "relmouse0:btn0.pressed"       "lmbPressed"
    .map "relmouse0:btn1.down"          "rmbDown"
    .map "relmouse0:btn1.up"            "rmbUp"
    .map "relmouse0:btn1.pressed"       "rmbPressed"
    .map "relmouse0:btn2.down"          "mmbDown"
    .map "relmouse0:btn2.up"            "mmbUp"
    .map "relmouse0:btn2.pressed"       "mmbPressed"

    .map "keyb0:ctrl.pressed"           "ctrlPressed"
    .map "keyb0:shift.pressed"          "shiftPressed"
    .map "keyb0:home.pressed"           "zoomIn"
    .map "keyb0:end.pressed"            "zoomOut"
    .map "keyb0:w.pressed"              "moveForward"
    .map "keyb0:a.pressed"              "moveLeft"
    .map "keyb0:s.pressed"              "moveBackward"
    .map "keyb0:d.pressed"              "moveRight"
    .map "keyb0:w.up"                   "moveStop"
    .map "keyb0:a.up"                   "moveStop"
    .map "keyb0:s.up"                   "moveStop"
    .map "keyb0:d.up"                   "moveStop"
    .map "keyb0:up.pressed"             "moveForward"
    .map "keyb0:left.pressed"           "moveLeft"
    .map "keyb0:right.pressed"          "moveRight"
    .map "keyb0:down.pressed"           "moveBackward"
    .map "keyb0:up.up"                  "moveStop"
    .map "keyb0:left.up"                "moveStop"
    .map "keyb0:down.up"                "moveStop"
    .map "keyb0:right.up"               "moveStop"
    .map "keyb0:space.down"             "moveJump"
    .map "keyb0:e.down"                 "use"
    .map "keyb0:r.down"                 "reload"
    .map "keyb0:shift.down"             "inventoryOn"
    .map "keyb0:shift.up"               "inventoryOff"
    .map "relmouse0:+zbtn.down"         "ScrollDown"
    .map "relmouse0:-zbtn.down"         "ScrollUp"
    .map "keyb0:tab.up"                 "cycleFocus"

    .endmap

    sel $oldCwd
}

#-------------------------------------------------------------------------------
#   OnGuiServerOpen
#
#   This function is called when the Nebula2 GUI server is opened.
#-------------------------------------------------------------------------------
proc OnGuiServerOpen {} {
    set cwd [psel]

    # initialize the default tooltip
    set guiRoot [/sys/servers/gui.getrootpath]
    sel $guiRoot
    new nguitooltip Tooltip
        sel Tooltip
        .setdefaultbrush "tooltip"
        .setfont "GuiSmall"
        .setcolor 0 0 0 1
        .setalignment "left"
        .setborder 0.005 0.005
    sel ..

    # load system skin
    source "scripts:systemskin.tcl"
}

#-------------------------------------------------------------------------------
#   OnGuiServerClose
#
#   This function is called when the Nebula2 GUI server is closed.
#-------------------------------------------------------------------------------
proc OnGuiServerClose {} {

}

#-------------------------------------------------------------------------------
#   ToggleSceneVisualization
#-------------------------------------------------------------------------------
proc ToggleSceneVisualization {} {

    if {[/sys/servers/scene.getrenderdebug] == "true"} {
        /sys/servers/scene.setrenderdebug "false"
    } else {
        /sys/servers/scene.setrenderdebug "true"
    }
}
