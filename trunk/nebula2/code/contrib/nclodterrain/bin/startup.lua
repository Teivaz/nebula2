--------------------------------------------------------------------------------
--   bin/startup.lua
--
--   This is the central Nebula runtime startup script which is 
--   used by various tools, like the Maya plugin or the viewer.
--
--   The script mainly sets up assigns and Nebula variables.
--
--   (C) 2003 RadonLabs GmbH
---------------------------------------------------------------------------------

pushcwd('.')

sel('/sys/servers/file2')
call('setassign', 'luascript', 'home:code/contrib/nluaserver/bin/')

dofile (mangle('luascript:nebthunker.lua'))
dofile (mangle('luascript:console.lua'))


---------------------------------------------------------------------------------
--   set up resource assigns
---------------------------------------------------------------------------------

f = nebula.sys.servers.file2
proj = f:manglepath('proj:')
home = f:manglepath('home:')

f:setassign('meshes', proj .. '/export/meshes/')
f:setassign('textures', proj .. '/export/textures/')
if (exists('/sys/servers/gfx')) then
	if (nebula.sys.servers.gfx:getfeatureset() == 'dx9') then
		f:setassign('shaders', home .. '/export/shaders/2.0/')
		puts('Shader directory: ' .. home .. '/export/shaders/2.0')
	else
		f:setassign('shaders', home .. '/export/shaders/fixed/')
		puts('Shader directory: ' .. home .. '/export/shaders/fixed')
	end
else
	f:setassign('shaders', home .. '/export/shaders/2.0/')
	puts('Shader directory: ' .. home .. '/export/shaders/2.0')
end
f:setassign('anims', proj .. '/export/anims/')
f:setassign('gfxlib', proj .. '/export/gfxlib/')

-- nclod directories
f:setassign('nclodtextures', proj .. '/code/contrib/nclodterrain/export/textures/')
f:setassign('nclodterrains', proj .. '/code/contrib/nclodterrain/export/terrains/')
f:setassign('nclodshaders', proj .. '/code/contrib/nclodterrain/export/shaders/')

--------------------------------------------------------------------------------
--   restore original directory
--------------------------------------------------------------------------------

popcwd()
   
