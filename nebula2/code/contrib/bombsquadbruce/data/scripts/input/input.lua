--  input.lua
--  =========
--  Set up various mappings of input devices to game controls.
--
-----------------------------------------------------------------------------
runscript( 'input/keymaps.lua')
-----------------------------------------------------------------------------

function makeThunk( path )
	return 'script:nebula'..(string.gsub( path, '/', '.' ))..':' -- converts e.g. '/game/world' into 'script:nebula.game.world:'
end
-----------------------------------------------------------------------------

function activateControls(path)
    local cwd = psel()
	sel(path)
		call( 'setcontrol', 'SPIN_LEFT', 'mouseleft' )
		call( 'setcontrol', 'SPIN_RIGHT', 'mouseright' )
		call( 'setcontrol', 'SPIN_UP', 'mouseup' )
		call( 'setcontrol', 'SPIN_DOWN', 'mousedown' )
		call( 'setcontrol', 'MOVE_FWD', 'forwards' )
		call( 'setcontrol', 'MOVE_BACK', 'backwards' )
		call( 'setcontrol', 'MOVE_LEFT', 'left' )
		call( 'setcontrol', 'MOVE_RIGHT', 'right' )
		call( 'setcontrol', 'ZOOM_IN', '' )
		call( 'setcontrol', 'ZOOM_OUT', '' )
    sel( cwd )
end
-----------------------------------------------------------------------------
function deactivateControls(path)
    local cwd = psel()
	sel(path)
		call( 'setcontrol', 'SPIN_LEFT', '' )
		call( 'setcontrol', 'SPIN_RIGHT', '' )
		call( 'setcontrol', 'SPIN_UP', '' )
		call( 'setcontrol', 'SPIN_DOWN', '' )
		call( 'setcontrol', 'MOVE_FWD', '' )
		call( 'setcontrol', 'MOVE_BACK', '' )
		call( 'setcontrol', 'MOVE_LEFT', '' )
		call( 'setcontrol', 'MOVE_RIGHT', '' )
		call( 'setcontrol', 'ZOOM_IN', '' )
		call( 'setcontrol', 'ZOOM_OUT', '' )
	sel(cwd)
end
-----------------------------------------------------------------------------

function UniversalControls()
	--controls that are generally active:

	--channels:
	MapKey( keyMaps.move.forwards, 'pressed', 'forwards' )
	MapKey( keyMaps.move.backwards, 'pressed', 'backwards')
	MapKey( keyMaps.move.left,	'pressed', 'left' )
	MapKey( keyMaps.move.right, 'pressed', 'right')
	inputSrvThunk:map('relmouse0:-x',		'mouseleft')
	inputSrvThunk:map('relmouse0:+x',		'mouseright')
	inputSrvThunk:map('relmouse0:-y',		'mouseup')
	inputSrvThunk:map('relmouse0:+y',		'mousedown')
	--inputSrvThunk:map('relmouse0:+z',		'mousewheelfwd')
	--inputSrvThunk:map('relmouse0:-z',		'mousewheelback')			
	MapKey( keyMaps.quit, 'down', 'script:guiServer:Up()')
	
    MapKey( keyMaps.screenshot, 'up', 'script:Screenshot()')

	if debugControlsActive then
        MapKey( keyMaps.camera.activatePlayerCam, 'down',	'script:chaseCamera:Activate()')
        MapKey( keyMaps.camera.activateFreeCam, 'down',		'script:flyCamera:Activate()')
        MapKey( keyMaps.camera.activateObstacleCam, 'down',		'script:obstacleCamera:Activate()')
        MapKey( keyMaps.admin.toggleConsole, 'down','script:nebula.sys.servers.console:toggle()')
        MapKey( keyMaps.admin.showStats, 'down', 'script:showStats("gfx*")' )
        MapKey( keyMaps.admin.showStats, 'up', 'script:showStats("gui*")' )
    end
end
-----------------------------------------------------------------------------

function PlayerCentricControls()
	writeDebug( 'PlayerCentricControls' )
	UniversalControls()

	--Player Controls:
	activateControls(playerPath)
	deactivateControls(flyCamera.path)
	deactivateControls(chaseCamera.path)
	
	if debugControlsActive then
        MapKey( keyMaps.camera.toggleControls, 'down',		'script:mapControls(FlyCameraControls)')
    end
end

-----------------------------------------------------------------------------

function FlyCameraControls()
	UniversalControls()

	-- Camera controls
	cameraThunk = makeThunk(flyCamera.path)
	MapKey( keyMaps.camera.drag, 'down',          cameraThunk..'setspinmode("DRAG")')
	MapKey( keyMaps.camera.drag, 'up',                cameraThunk..'setspinmode("NONE")')
	MapKey( keyMaps.camera.rotate, 'down',         cameraThunk..'setspinmode("ROTATE")')
	MapKey( keyMaps.camera.rotate, 'up',              cameraThunk..'setspinmode("NONE")')
	MapKey( keyMaps.camera.overhead, 'down',  'script:flyCamera.SetOverhead()')
	MapKey( keyMaps.camera.reset, 'down',         'script:flyCamera.Reset()')

	activateControls(flyCamera.path)
	deactivateControls(playerPath)
	deactivateControls(chaseCamera.path)
	
    if debugControlsActive then
        MapKey( keyMaps.camera.toggleControls, 'down',		'script:mapControls(PlayerCentricControls)')
    end
end
-----------------------------------------------------------------------------

function mapControls(setControlsFunc)
	local _cwd = psel()
	sel('/sys/servers/input')
		inputSrvThunk=nebula.sys.servers.input
		writeDebug( 'beginmap' )
		call('beginmap')
			setControlsFunc()
		writeDebug( 'endmap' )
		call('endmap')
	sel(_cwd)
end
-----------------------------------------------------------------------------

function MapKey( keyMap, action, cmd )
    if type( keyMap ) == 'table' then
        for idx, key in keyMap do
            MapKey( key, action, cmd )
        end
    else
        inputSrvThunk:map( keyMap..'.'..action, cmd )
    end
end
-----------------------------------------------------------------------------

function GameOverControls()
    UniversalControls()
	
	deactivateControls(flyCamera.path)
	deactivateControls(playerPath)
	deactivateControls(chaseCamera.path)
end

-----------------------------------------------------------------------------
function SetFlyCameraControls()
-- has no effect if we aren't using the fly camera
-- To get back to player controls, reactivate the
-- corresponding camera.
    mapControls(FlyCameraControls)
end