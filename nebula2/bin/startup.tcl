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
    set home [.manglepath "home:"]
    .setassign "meshes"   "$home/export/meshes/"
    .setassign "textures" "$home/export/textures/"
    .setassign "shaders"  "$home/export/shaders/dx9/"
    .setassign "anims"    "$home/export/anims/"
    .setassign "gfxlib"   "$home/export/gfxlib/"

#-------------------------------------------------------------------------------
#   restore original directory
#-------------------------------------------------------------------------------
sel $oldCwd
   
