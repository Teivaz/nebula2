-- startup.lua
-- ---------------------------------------------------------------------------
-- This script is run automatically by the exe,
-- once, when the engine is started.
-- It runs all non project-specific initialization code.
-- ---------------------------------------------------------------------------

dofile(mangle('home:data/scripts/nebthunker.lua')) -- allows us to thunk

function runscript(script)
    if logging then io.write( 'runscript : '..script..' start\n' ) end
	path = nebula.sys.servers.file2:manglepath('scripts:'..script)
	dofile(path)
	if logging then io.write( 'runscript : '..script..' end\n' ) end
end

function initServers()
	new('bbsceneserver', '/sys/servers/scene')
	new("nconserver", "/sys/servers/console")
	new("bbcullingmgr", "/sys/servers/culling" )
	new("nshadowserver", "/sys/servers/shadow" )
    initGfxServer()
    new('ndi8server', '/sys/servers/input')
end

function setAssigns()    
	a = nebula.sys.servers.file2
	a:setassign('data', 		    'home:data/')   
	a:setassign('objects', 		'data:objects/')   
	a:setassign('gui', 		    'data:gui/')   
	a:setassign('animations', 'data:animations/') 
	a:setassign('terrain',		'data:terrain/') 
	a:setassign('skybox',		'terrain:skybox/') 
	a:setassign('sun',		    'terrain:sun/') 
	a:setassign('scripts', 		'data:scripts/')   
	a:setassign('sounds', 		'data:sounds/')   
	a:setassign('particles', 	'data:particles/') 
	a:setassign('anims', 'objects:') -- a hack to work around hard coded export directories
    a:setassign('meshes', 'objects:') -- a hack to work around hard coded export directories
    a:setassign('textures', 'objects:') -- a hack to work around hard coded export directories
    a:setassign( 'shaders',     'data:shaders/' )
end

function initGfxServer()
  	new('nd3d9server', '/sys/servers/gfx')
    local gfx = nebula.sys.servers.gfx
    gfx:setdisplaymode( win.title, win.mode, win.xpos, win.ypos, win.xres, win.yres, true )
    gfx:setcursorvisibility( 'none' )
    gfx:opendisplay()
end  

function initLog()
    if logging == true then
        io.output( win.title..'.log' )
    end
end

-- Quit() exits the program
function Quit()
    engineObj:stopengine()
end


---------------------------------------------------------------------------
-- Here's where it all begins...
---------------------------------------------------------------------------
setAssigns()
runscript( 'config.lua' ) -- get game-specific parameters, in particular for the display
initLog()
initServers()
engineObj = lookup('/engine') -- do not change this!  It is fixed by the c++ code
runscript( 'universal/util/util.lua' )
runscript( 'universal/gameUniversal.lua' )
runscript( 'universal/gui/guiServer.lua' )
runscript( 'universal/sound.lua' )
-- Now creates the initial BBGame object 
runscript( 'game.lua' ) 
-- now create the game text strings
runscript('strings/'..language..'.lua') -- this should come last


    
