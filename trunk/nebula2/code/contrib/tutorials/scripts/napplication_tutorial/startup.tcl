proc OnStartup {} {
    sel /sys/servers/file2
        .setassign "meshes"   "proj:export/meshes/"
        .setassign "textures" "proj:export/textures/"
        .setassign "anims"    "proj:export/anims/"
        .setassign "gfxlib"   "proj:export/gfxlib/"
        .setassign "shaders"  "proj:data/shaders/2.0/"
#        .setassign "shaders"  "proj:data/shaders/fixed/"
}

proc OnGraphicsStartup {} {

}

proc OnMapInput {} {

}

#-------------------------------------------------------------------------------
#   This procedure is called when the gui server is opened.
#-------------------------------------------------------------------------------
proc OnGuiServerOpen {} {

}

#-------------------------------------------------------------------------------
#   This procedure is called when the gui server is closed.
#-------------------------------------------------------------------------------
proc OnGuiServerClose {} {

}
