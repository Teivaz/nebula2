-- world.lua: Setup game world
-- ---------------------------------------------------------------------------
runscript( 'avatar/avatar.lua' )
runscript( 'water.lua' )

--public functions
    
function world.Create()   
    world.Destroy()
    initScene()

	local cwd = psel()
	selNew( 'ccofmworld', worldPath )
        world.object = lookup( getCwd() )
        world.object:setoncollisionscript( 'world.OnCollision()' )
        world.object:setmap( mapPath) -- also loads the map, so don't use it before this point
    nebula.sys.servers.variable:declarevariable( 'wind', 'wind' )
    game.object:setworld( worldPath )
    selNew('nroot',actorsPath)
        createWater()
        createAvatar()
	sel(worldPath)
		call('setplayer', playerPath)
		call('setwater', waterPath )	
		call( 'startgameobjs' )
            CreateForest()
		call( 'endgameobjs' )
	sel(cwd)
	mapControls(PlayerCentricControls)
    chaseCamera:Activate()
	world.Pause(false)
end

function world.Destroy()
    if world.object ~= nil then 
        world.object = nil 
    end
end

-- Callback functions:

function world.Trigger(deltaTime)
    if world.paused ~= true then 
        updateWind(deltaTime)
    end
end

function world.OnCollision()
    avatar.ResetPosition()
end

-- private functions

function world.Pause(pause)
    if pause then
        mapControls(GameOverControls)        
        world.object:pause(true)
        if world.paused ~= true then 
            soundMgr:MassFadeOut()
            world.paused = true
        end
    else
        mapControls( PlayerCentricControls ) -- this isn't optimally elegant; it would be better to have stored the state (including controls) before opening this menu
        world.object:pause(false)
        if world.paused ~= false then 
            world.paused = false
            soundMgr:MassFadeIn()
        end
    end
end

function updateWind(deltaTime)
    local cwd = psel()
    sel( '/sys/servers/variable' )
        if nil == smoothness or 0 >= smoothness then
            smoothness = world.wind.smoothness
            sign = 2*math.random() - 1 -- determine the character of the wind changes for the next smoothness seconds
        else
            smoothness = smoothness - deltaTime
        end
        --update strength to simulate gusting
        world.wind.strength = Clamp( world.wind.strength + math.random() * world.wind.strVariance * deltaTime * sign, -1, 1 )
        if math.abs(world.wind.strength) < world.wind.dirChangeStrThreshhold then
            -- the wind is weak now; this is a subtle time to change wind dir
            world.wind[1] = Clamp( world.wind[1] + ( 2*math.random() - 1 ) * world.wind.dirVariance, -1, 1 )
            world.wind[3] = Clamp( world.wind[3] + ( 2*math.random() - 1 ) * world.wind.dirVariance, -1, 1 )
        end
        call( 'setvectorvariable', 'wind', world.wind[1], world.wind[2], world.wind[3], world.wind.strength );
    sel(cwd)
end

