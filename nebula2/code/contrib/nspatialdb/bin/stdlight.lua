-------------------------------------------------------------------------------
--   Defines the standard lighting for nviewer.exe
-------------------------------------------------------------------------------

pushcwd('.')

sel('/usr/scene')

new('ntransformnode','stdlight')
    sel('stdlight')
    new('nspotlightnode', 'l0')
	l = lookup('l0')
	sel('l0')
	l:setposition(0,10,0)
	l:seteuler(-30,45,0)
	l:setorthogonal(1000,1000,0.25,175)
	l:setvector('LightDiffuse',0.8,0.8,0.8,1)
	l:setvector('LightSpecular',1,1,1,1)
	l:setvector('LightAmbient',0.2,0.2,0.2,1)
	l:settexture('LightModMap','textures:system/white.dds')
    sel('..') 
sel('..') 

popcwd()




