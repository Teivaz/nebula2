#-------------------------------------------------------------------------------
#   Defines the standard lighting for nviewer.exe
#-------------------------------------------------------------------------------

set oldCwd [psel]
sel /usr/scene

new ntransformnode stdlight
    sel stdlight
    new nspotlightnode l0
        sel l0
        .setposition 25 20 25
        .seteuler -30 45 0
        .setorthogonal 1000 1000 0.25 175
        .setvector LightDiffuse 1 1 1 1
        .setvector LightSpecular 1 1 1 1
        .setvector LightAmbient 0.25 0.25 0.25 1
        .settexture LightModMap "textures:system/white.dds"
    sel ..    
sel ..    
sel $oldCwd




