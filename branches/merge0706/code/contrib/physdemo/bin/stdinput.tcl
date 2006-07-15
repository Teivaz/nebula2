#-------------------------------------------------------------------------------
#   Defines the standard input mappings for PhysDemo
#-------------------------------------------------------------------------------

# (if you don't have a mousewheel or whatever, rebind the functions to other keys in here - this is just a
# script file, you can change it however you want, you won't break anything - just keep a backup, so you
# don't lose the command names)

set oldCwd [psel]

sel /sys/servers/input
    .beginmap
    .map "keyb0:esc.down"          "script:/sys/servers/gui.togglesystemgui"
    .map "relmouse0:btn0.pressed"  "look"
    .map "relmouse0:btn1.pressed"  "zoom"
    .map "relmouse0:btn2.pressed"  "pan"
    .map "relmouse0:-x"            "left"
    .map "relmouse0:+x"            "right"
    .map "relmouse0:-y"            "up"
    .map "relmouse0:+y"            "down"
    .map "relmouse0:+zbtn.down"    "zoomIn"
    .map "relmouse0:-zbtn.down"    "zoomOut"
    .map "keyb0:f12.down"          "screenshot"
    .map "keyb0:1.down"            "create_box"
    .map "keyb0:2.down"            "create_sphere"
    .map "keyb0:3.down"            "create_big_sphere"
    .map "keyb0:4.down"            "create_stack"
    .map "keyb0:6.down"            "drop_box"
    .map "keyb0:7.down"            "drop_sphere"
    .map "keyb0:8.down"            "drop_stack"
    .map "keyb0:0.down"            "kaboom"
    .map "keyb0:space.down"        "fire_bullet"
    .map "keyb0:prior.pressed"     "cfm_up"
    .map "keyb0:next.pressed"      "cfm_down"
    .map "keyb0:home.pressed"      "erp_up"
    .map "keyb0:end.pressed"       "erp_down"
    .map "keyb0:insert.pressed"    "fps_cutoff_up"
    .map "keyb0:delete.pressed"    "fps_cutoff_down"

    .map "keyb0:p.down"    "create_stack_line"
    .map "keyb0:w.down"    "create_wall"

    .endmap
sel ..

sel $oldCwd
