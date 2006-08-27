#-------------------------------------------------------------------------------
#   bin/startup.tcl
#
#   This is the central Nebula runtime startup script which is
#   used by various tools, like the Maya plugin or the viewer.
#
#   The script mainly sets up assigns and Nebula variables.
#
#   (C) 2003 RadonLabs GmbH
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

    .setassign "data"     "$home/data/"
    .setassign "export"   "$proj/export/"

    .setassign "renderpath" "data:shaders"
    .setassign "scripts"    "data:scripts"
    .setassign "physics"    "export:physics/"
    .setassign "meshes"     "export:meshes/"
    .setassign "textures"   "export:textures/"
    .setassign "anims"      "export:anims/"
    .setassign "gfxlib"     "export:gfxlib/"
    .setassign "lights"     "export:lightlib/"
    .setassign "db"         "export:db/"
    .setassign "levels"     "export:levels/"
    .setassign "sound"      "export:audio/"
    .setassign "cegui"      "export:cegui/"
    .setassign "charlib"    "gfxlib:characters/"

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
#   This method is not yet called.
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
    .map "keyb0:f4.down"  "script:ToggleSceneVisualization"
    .map "keyb0:f5.down" "script:/sys/servers/capture.toggle"
    .map "keyb0:f11.down" "console"
    .map "keyb0:f12.down" "makescreenshot"
    .map "keyb0:t.down" "script:/sys/servers/time.resettime"
    .map "keyb0:1.down" "script:/sys/servers/time.stoptime"
    .map "keyb0:2.down" "script:/sys/servers/time.starttime"
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
    .endmap
    sel $cwd
}

#-------------------------------------------------------------------------------
#   OnMapInput
#
#   This procedure is called when input should be mapped.
#-------------------------------------------------------------------------------
proc OnMapInput {} {
    set oldCwd [psel]
    sel /sys/servers/input

    .beginmap
    .map "keyb0:f1.down"                "script:/sys/servers/gui.togglesystemgui"
    .map "keyb0:f2.down"                "togglePhysicsVisualization"
    .map "keyb0:f3.down"                "toggleGraphicsVisualization"
    .map "keyb0:f4.down"                "script:ToggleSceneVisualization"
    .map "keyb0:f5.down"                "script:/sys/servers/capture.toggle"
    .map "keyb0:f9.down"                "toggleFOVVisualization"
    .map "keyb0:f12.down"               "script:DoScreenCapture "
    .map "keyb0:esc.down"               "escape"
    .map "keyb0:tab.down"               "tabDown"

    .map "relmouse0:btn2.down"          "vwrOn"
    .map "relmouse0:btn2.up"            "vwrOff"
    .map "relmouse0:-x"                 "vwrLeft"
    .map "relmouse0:+x"                 "vwrRight"
    .map "relmouse0:-y"                 "vwrUp"
    .map "relmouse0:+y"                 "vwrDown"
    .map "keyb0:space.down"             "vwrReset"
    .map "relmouse0:+zbtn.down"         "vwrZoomOut"
    .map "relmouse0:-zbtn.down"         "vwrZoomIn"

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
    .map "keyb0:shift.up"             	"inventoryOff"
    .map "relmouse0:+zbtn.down"         "ScrollDown"
    .map "relmouse0:-zbtn.down"         "ScrollUp"
    .map "keyb0:tab.up"                 "cycleFocus"
    .map "keyb0:t.down"                 "timeReset"

#    .map "keyb0:1.down"                 "script:DebugRecordPosition"
#    .map  keyb0:f1.down                 "script:/sys/servers/console.watch *"
#    .map  keyb0:f1.up                   "script:/sys/servers/console.unwatch"

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

    # define the system skin
    set skin [/sys/servers/gui.newskin system]
    sel $skin
    # set texture path pre- and postfix (NOTE: don't change path to textures:system!!!)
    .settextureprefix "home:export/textures/system/"
    .settexturepostfix ".dds"

    # active and inactive window modulation color
    .setactivewindowcolor 1.0 1.0 1.0 1.0
    .setinactivewindowcolor 0.6 0.6 0.6 0.6
    .setbuttontextcolor 0.0 0.0 0.0 1.0
    .settitletextcolor  0.0 0.0 0.0 1.0
    .setlabeltextcolor  0.0 0.0 0.0 1.0
    .setentrytextcolor  0.0 0.0 0.0 1.0
    .settextcolor       0.0 0.0 0.0 1.0
    .setmenutextcolor   0.0 0.0 0.0 1.0

    # define brushes
    .beginbrushes

    # window title bar, window background, tooltip background
    .addbrush titlebar skin  66 152 10 20 1.0 1.0 1.0 1.0
    .addbrush window   skin   8 154  4  4 1.0 1.0 1.0 1.0
    .addbrush tooltip  skin   8 154  4  4 1.0 1.0 0.878 0.8
    .addbrush pink     skin   8 154  4  4 1.0 0.0 1.0 1.0
    .addbrush dragbox  skin   8 154  4  4 1.0 0.8 0.8 0.5

    # text entry field
    .addbrush textentry_n skin 446 124 8 8 0.7 0.7 0.7 1.0
    .addbrush textentry_p skin 446 124 8 8 0.8 0.8 0.8 1.0
    .addbrush textentry_h skin 446 124 8 8 0.9 0.9 0.9 1.0
    .addbrush textcursor  skin 446 124 8 8 0.4 0.4 0.4 1.0

    # the window close button
    .addbrush close_n skin 388 40 16 16 1.0 1.0 1.0 1.0
    .addbrush close_h skin 404 40 16 16 1.0 1.0 1.0 1.0
    .addbrush close_p skin 420 40 16 16 1.0 1.0 1.0 1.0

    # the window size button
    .addbrush size_n skin 372 40 16 16 1.0 1.0 1.0 1.0
    .addbrush size_h skin 372 40 16 16 1.0 1.0 1.0 1.0
    .addbrush size_p skin 372 40 16 16 1.0 1.0 1.0 1.0

    # arrows
    .addbrush arrowleft_n  skin  68 40 16 16 1.0 1.0 1.0 1.0
    .addbrush arrowleft_h  skin  84 40 16 16 1.0 1.0 1.0 1.0
    .addbrush arrowleft_p  skin 100 40 16 16 1.0 1.0 1.0 1.0
    .addbrush arrowright_n skin 116 40 16 16 1.0 1.0 1.0 1.0
    .addbrush arrowright_h skin 132 40 16 16 1.0 1.0 1.0 1.0
    .addbrush arrowright_p skin 148 40 16 16 1.0 1.0 1.0 1.0
    .addbrush arrowup_n    skin 164 40 16 16 1.0 1.0 1.0 1.0
    .addbrush arrowup_h    skin 180 40 16 16 1.0 1.0 1.0 1.0
    .addbrush arrowup_p    skin 196 40 16 16 1.0 1.0 1.0 1.0
    .addbrush arrowdown_n  skin  20 40 16 16 1.0 1.0 1.0 1.0
    .addbrush arrowdown_h  skin  36 40 16 16 1.0 1.0 1.0 1.0
    .addbrush arrowdown_p  skin  52 40 16 16 1.0 1.0 1.0 1.0

    # sliders
    .addbrush sliderhori_bg    skin 228  40 16 16 0.5 0.5 0.5 1.0
    .addbrush slidervert_bg    skin 228  40 16 16 0.5 0.5 0.5 1.0
    .addbrush sliderknobhori_n skin 276  40 16 16 1.0 1.0 1.0 1.0
    .addbrush sliderknobhori_h skin 292  40 16 16 1.0 1.0 1.0 1.0
    .addbrush sliderknobhori_p skin 308  40 16 16 1.0 1.0 1.0 1.0
    .addbrush sliderknobvert_n skin 324  40 16 16 1.0 1.0 1.0 1.0
    .addbrush sliderknobvert_h skin 340  40 16 16 1.0 1.0 1.0 1.0
    .addbrush sliderknobvert_p skin 356  40 16 16 1.0 1.0 1.0 1.0

    # standard buttons
    .addbrush button_n skin 192 152 96 20 1.0 1.0 1.0 1.0
    .addbrush button_h skin 288 152 96 20 1.0 1.0 1.0 1.0
    .addbrush button_p skin 384 152 96 20 1.0 1.0 1.0 1.0

    .addbrush menu_n skin 192 172 96 16 1.0 1.0 1.0 1.0
    .addbrush menu_h skin 288 172 96 16 1.0 1.0 1.0 1.0
    .addbrush menu_p skin 384 172 96 16 1.0 1.0 1.0 1.0

    .addbrush button_64x16_n skin   0 0 64 16 1.0 1.0 1.0 1.0
    .addbrush button_64x16_h skin  64 0 64 16 1.0 1.0 1.0 1.0
    .addbrush button_64x16_p skin 128 0 64 16 1.0 1.0 1.0 1.0

    .addbrush button_96x16_n skin 192 0 96 16 1.0 1.0 1.0 1.0
    .addbrush button_96x16_h skin 288 0 96 16 1.0 1.0 1.0 1.0
    .addbrush button_96x16_p skin 384 0 96 16 1.0 1.0 1.0 1.0

    .addbrush button_64x24_n skin   0 16 64 24 1.0 1.0 1.0 1.0
    .addbrush button_64x24_h skin  64 16 64 24 1.0 1.0 1.0 1.0
    .addbrush button_64x24_p skin 128 16 64 24 1.0 1.0 1.0 1.0

    .addbrush button_96x24_n skin 192 16 96 24 1.0 1.0 1.0 1.0
    .addbrush button_96x24_h skin 288 16 96 24 1.0 1.0 1.0 1.0
    .addbrush button_96x24_p skin 384 16 96 24 1.0 1.0 1.0 1.0

    .addbrush button_64x20_n skin 192 152 96 20 1.0 1.0 1.0 1.0
    .addbrush button_64x20_h skin 288 152 96 20 1.0 1.0 1.0 1.0
    .addbrush button_64x20_p skin 384 152 96 20 1.0 1.0 1.0 1.0

    .addbrush menu_64x16_n skin   0 172 64 16 1.0 1.0 1.0 1.0
    .addbrush menu_64x16_h skin  64 172 64 16 1.0 1.0 1.0 1.0
    .addbrush menu_64x16_p skin 128 172 64 16 1.0 1.0 1.0 1.0

    .addbrush menu_128x16_n skin 192 172 96 16 1.0 1.0 1.0 1.0
    .addbrush menu_128x16_h skin 288 172 96 16 1.0 1.0 1.0 1.0
    .addbrush menu_128x16_p skin 384 172 96 16 1.0 1.0 1.0 1.0

    # list views
    .addbrush list_background skin 446  72 8 8 1.0 1.0 1.0 1.0
    .addbrush list_selection  skin  64 172 64 16 1.0 1.0 1.0 1.0

    # icons
    .addbrush console_n skin     0  56 48 48 1.0 1.0 1.0 1.0
    .addbrush console_p skin     0  56 48 48 0.5 0.5 0.5 1.0
    .addbrush console_h skin     0 104 48 48 1.0 1.0 1.0 1.0

    .addbrush texbrowser_n skin  48  56 48 48 1.0 1.0 1.0 1.0
    .addbrush texbrowser_p skin  48  56 48 48 0.5 0.5 0.5 1.0
    .addbrush texbrowser_h skin  48 104 48 48 1.0 1.0 1.0 1.0

    .addbrush gfxbrowser_n skin  96  56 48 48 1.0 1.0 1.0 1.0
    .addbrush gfxbrowser_p skin  96  56 48 48 0.5 0.5 0.5 1.0
    .addbrush gfxbrowser_h skin  96 104 48 48 1.0 1.0 1.0 1.0

    .addbrush dbgwindow_n skin  144  56 48 48 1.0 1.0 1.0 1.0
    .addbrush dbgwindow_p skin  144  56 48 48 0.5 0.5 0.5 1.0
    .addbrush dbgwindow_h skin  144 104 48 48 1.0 1.0 1.0 1.0

    .addbrush syswindow_n skin  192  56 48 48 1.0 1.0 1.0 1.0
    .addbrush syswindow_p skin  192  56 48 48 0.5 0.5 0.5 1.0
    .addbrush syswindow_h skin  192 104 48 48 1.0 1.0 1.0 1.0

    .addbrush contrwindow_n skin  336  56 48 48 1.0 1.0 1.0 1.0
    .addbrush contrwindow_p skin  336  56 48 48 0.5 0.5 0.5 1.0
    .addbrush contrwindow_h skin  336 104 48 48 1.0 1.0 1.0 1.0

    .addbrush hidegui_n skin    240  56 48 48 1.0 1.0 1.0 1.0
    .addbrush hidegui_p skin    240  56 48 48 0.5 0.5 0.5 1.0
    .addbrush hidegui_h skin    240 104 48 48 1.0 1.0 1.0 1.0

    .addbrush quit_n skin       288  56 48 48 1.0 1.0 1.0 1.0
    .addbrush quit_p skin       288  56 48 48 0.5 0.5 0.5 1.0
    .addbrush quit_h skin       288 104 48 48 1.0 1.0 1.0 1.0

    .addbrush hardpoint_n skin       0  188 48 48 1.0 1.0 1.0 1.0
    .addbrush hardpoint_p skin       0  188 48 48 0.5 0.5 0.5 1.0
    .addbrush hardpoint_h skin       48 188 48 48 1.0 1.0 1.0 1.0

    .addbrush disp_n skin       384  56 48 48 1.0 1.0 1.0 1.0
    .addbrush disp_p skin       384  56 48 48 0.5 0.5 0.5 1.0
    .addbrush disp_h skin       384 104 48 48 1.0 1.0 1.0 1.0

    # the left and right logos
    .addbrush n2logo n2logo 0 0 64 64 1.0 1.0 1.0 0.5
    .addbrush leftlogo  radonlabs 0 0 128 64 1.0 1.0 1.0 1.0
    .addbrush rightlogo n2toolkit 0 0 64 64 1.0 1.0 1.0 1.0

    # the color hexagon for the colorpicker
    .addbrush colorhex colorhexagon  0 0 170 141 1.0 1.0 1.0 1.0

    .endbrushes

    /sys/servers/gui.setsystemskin $skin
    /sys/servers/gui.setskin $skin

    # create the Nebula dock window
    /sys/servers/gui.newwindow nguidockwindow true

    sel $cwd
}

#-------------------------------------------------------------------------------
#   OnGuiServerClose
#
#   This procedure is called when the gui server is closed.
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
