#-------------------------------------------------------------------------------
#   Defines the standard input mappings for nviewer.exe
#-------------------------------------------------------------------------------

set oldCwd [psel]

sel /sys/servers/input
	.beginmap
    .map "keyb0:space.down"        "reset"
    .map "keyb0:esc.down"          "console"
    .map "relmouse0:btn0.pressed"  "look"
    .map "relmouse0:btn1.pressed"  "zoom"
    .map "relmouse0:btn2.pressed"  "pan"
    .map "relmouse0:-x"            "left"
    .map "relmouse0:+x"            "right"
    .map "relmouse0:-y"            "up"
    .map "relmouse0:+y"            "down"
    .map "relmouse0:-z"            "zoomIn"
    .map "relmouse0:+z"            "zoomOut"
    .map "keyb0:f1.down"           "mayacontrols"
    .map "keyb0:f2.down"           "flycontrols"
    .map "keyb0:1.down"            "speed0"
    .map "keyb0:2.down"            "speed1"
    .map "keyb0:3.down"            "speed2"
    .map "keyb0:f5.down"           "setpos0"
    .map "keyb0:f6.down"           "setpos1"
    .map "keyb0:f7.down"           "setpos2"
    .map "keyb0:f8.down"           "setpos3"
	.endmap
sel ..

sel $oldCwd
