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
            .setassign "shaders" "$home/export/shaders/2.0/"
            puts "Shader directory: $home/export/shaders/2.0"
        } else {
            .setassign "shaders" "$home/export/shaders/fixed/"
            puts "Shader directory: $home/export/shaders/fixed"
        }
    } else {
        .setassign "shaders" "$home/export/shaders/2.0/"
        puts "Shader directory: $home/export/shaders/2.0"
    }
    #.setassign "gfxlib"   "$proj/export/gfxlib/"
    .setassign "gfxlib"   "$home/export/gfxlib/"

    # set sound and music repository for nopenal
    .setassign "oal"     "$proj/code/contrib/nopenal/export/"
#-------------------------------------------------------------------------------
#   restore original directory
#-------------------------------------------------------------------------------
sel $oldCwd

/sys/servers/console.watch *
