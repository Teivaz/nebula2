#-------------------------------------------------------------------------------
#   Defines the standard lighting for nviewer.exe
#-------------------------------------------------------------------------------

set oldCwd [psel]
sel /usr/scene

new ntransformnode stdlight
    sel stdlight
    .seteuler 0 -45 45
    new nvolumelightnode l
        sel l
        .setposition 50 0 0
        .setscale 1000 1000 1000
        .setvector lightDiffuse 1 1 1 1
        .setvector lightSpecular 1 1 1 1
        .setvector lightAmbient 1 1 1 1
    sel ..    
sel ..    
sel $oldCwd




