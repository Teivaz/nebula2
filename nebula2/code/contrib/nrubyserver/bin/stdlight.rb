#-------------------------------------------------------------------------------
#   Defines the standard lighting for nviewer.exe
#-------------------------------------------------------------------------------

oldCwd = psel
sel '/usr/scene'

new 'ntransformnode', 'stdlight'
    sel 'stdlight'
        new 'nlightnode', 'l'
        sel 'l'
        setposition 1000, 500, 1000
        setvector 'LightDiffuse',   1, 1, 1, 1
        setvector 'LightSpecular',  1, 1, 1, 1
        setvector 'LightAmbient' ,  1, 1, 1, 1
        setvector 'LightDiffuse1' , 0.3, 0.3, 0.3, 1.0
        setvector 'LightSpecular1', 0.3, 0.3, 0.3, 1.0

sel oldCwd




