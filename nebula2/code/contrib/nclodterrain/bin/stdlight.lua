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
	l:setposition(130,30,20)
	l:seteuler(-30,45,0)
	l:setorthogonal(1000,1000,0.25,175)
	l:setvector('LightDiffuse',1,1,1,1)
	l:setvector('LightSpecular',1,1,1,1)
	l:setvector('LightAmbient',0,0,0,0)
	l:settexture('LightModMap','textures:system/white.dds')
    sel('..') 
sel('..') 

popcwd()




