-------------------------------------------------------------------------------
--   Defines the standard lighting for nviewer.exe
-------------------------------------------------------------------------------

pushcwd('.')

sel('/usr/scene')

new('ntransformnode','stdlight')
    sel('stdlight')
    new('nlightnode', 'l')
	l = lookup('l')
	sel('l')
	l:setposition(1000,500,1000)
	l:setvector('LightDiffuse',0.8,0.8,0.8,1)
	l:setvector('LightSpecular',1,1,1,1)
	l:setvector('LightAmbient',0.2,0.2,0.2,1)
	l:setvector('LightDiffuse1', 0.3, 0.3, 0.3, 1.0)
	l:setvector('LightSpecular1', 0.3, 0.3, 0.3, 1.0)
    sel('..') 
sel('..') 

popcwd()




