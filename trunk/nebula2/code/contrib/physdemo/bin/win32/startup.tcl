#-------------------------------------------------------------------------------
#   startup.tcl
#
#   This is the Elium startup scripted.  Generally sets path assigns, but may
#   do other things - just look yourself, it isn't that long, yeesh!
#
#   (C) 2004 MeganFox
#-------------------------------------------------------------------------------

set oldCwd [psel]

#-------------------------------------------------------------------------------
#   set up resource assigns
#-------------------------------------------------------------------------------
sel /sys/servers/file2
    set proj [.manglepath "proj:"]
    set home [.manglepath "home:"]
    
    .setassign "meshes"   "data/meshes/"
    .setassign "textures" "data/textures/"
    if {[exists /sys/servers/gfx]} {
        if {[/sys/servers/gfx.getfeatureset] == "dx9"} {   
            .setassign "shaders" "data/shaders/2.0/"
            puts "Shader directory: data/shaders/2.0"
        } else {
            .setassign "shaders" "data/shaders/fixed/"
            puts "Shader directory: data/shaders/fixed"
        }
    } else {
        .setassign "shaders" "data/shaders/2.0/"
        puts "Shader directory: data/shaders/2.0"
    }
    .setassign "anims"    "data/anims/"
	.setassign "shapes"   "data/shapedefs/"
    .setassign "lights"   "data/lightdefs/"

#-------------------------------------------------------------------------------
#   restore original directory
#-------------------------------------------------------------------------------
sel $oldCwd
   
