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

set oldCwd [psel]

#-------------------------------------------------------------------------------
#   set up resource assigns
#-------------------------------------------------------------------------------
sel /sys/servers/file2
    set proj [.manglepath "proj:"]
    set home [.manglepath "home:"]
    
    #.setassign "meshes"   "$proj/export/meshes/"
    #.setassign "textures" "$proj/export/textures/"
    .setassign "meshes"   "$home/export/meshes/"
    .setassign "textures" "$home/export/textures/"

    if {[exists /sys/servers/gfx]} {
        if {[/sys/servers/gfx.getfeatureset] == "dx9"} {   
            .setassign "shaders" "$home/data/shaders/2.0/"
            puts "Shader directory: $home/data/shaders/2.0"
        } else {
            .setassign "shaders" "$home/data/shaders/fixed/"
            puts "Shader directory: $home/data/shaders/fixed"
        }
    } else {
        .setassign "shaders" "$home/data/shaders/2.0/"
        puts "Shader directory: $home/data/shaders/2.0"
    }
    #.setassign "anims"    "$proj/export/anims/"
    #.setassign "gfxlib"   "$proj/export/gfxlib/"
    .setassign "anims"    "$home/export/anims/"
    .setassign "gfxlib"   "$home/export/gfxlib/"

    # set shader repository for ngameswf
    .setassign "gswf"     "$home/code/contrib/ngameswf/export/shaders/gswf/"
#-------------------------------------------------------------------------------
#   restore original directory
#-------------------------------------------------------------------------------
sel $oldCwd
   
