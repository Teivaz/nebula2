#-------------------------------------------------------------------------------
#   Defines the standard lighting for npyviewer.exe
#-------------------------------------------------------------------------------

oldCwd = psel()
sel('/usr/scene')

stdLight = new('nspotlightnode', 'stdlight')
stdLight.setposition(1000, 500, 1000)
stdLight.setorthogonal(10, 10, 0.25, 175)
stdLight.setvector('LightDiffuse',   1,   1,   1,   1)
stdLight.setvector('LightSpecular',  1,   1,   1,   1)
stdLight.setvector('LightAmbient',   1,   1,   1,   1)
stdLight.setvector('LightDiffuse1',  0.3, 0.3, 0.3, 1.0)
stdLight.setvector('LightSpecular1', 0.3, 0.3, 0.3, 1.0)
stdLight.settexture('LightModMap', 'textures:system/spotlight.dds')

sel(oldCwd)

