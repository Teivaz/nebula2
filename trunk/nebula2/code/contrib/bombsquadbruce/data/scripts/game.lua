--   game.lua
--   ========
--   Game-specific initialization
-- ---------------------------------------------------------------------------

--a "path" is fully qualified, a "dir" is not
gamePath = '/game' 
worldPath = gamePath..'/world'
actorsPath = worldPath..'/'..'actors'
avatarName = 'julie'
playerPath = actorsPath..'/'..avatarName
modelsPath ='/lib/models'

-- ---------------------------------------------------------------------------
--   Run setup scripts to define functions
-- ---------------------------------------------------------------------------
runscript('setdependentvars.lua')
runscript('map.lua')
runscript('renderer.lua')
runscript('entity.lua')
runscript('input/input.lua')
runscript('world.lua')
runscript('newGui/newGui.lua')
runscript( 'forest.lua' )

-- ---------------------------------------------------------------------------
assert( sel( gamePath ) == nil )
new( 'ccgame', gamePath )

game = 
    Instantiate( GameT, 
        {   object = lookup(gamePath),
            renderer = mainRenderer,
            ticksPerSecond = 30 -- the rotation speed may depend on this, for reasons which I can't fathom at the moment :(
                                            --also, if I'm going to run this at a fixed tick, I should really be doing interpolation, which I'm not
        }
    )
-- ---------------------------------------------------------------------------------
-- the following methods are "inherited"; don't rename them
-- ---------------------------------------------------------------------------------
-- ---------------------------------------------------------------------------
-- This is called once when the game is first started
-- ---------------------------------------------------------------------------
function game:RegisterScripts()
	self:RegisterScript( world.Trigger )
	self:RegisterFixedTickScript( avatar.Trigger )	
	self:RegisterScript( soundMgr.Trigger )
end
-- ---------------------------------------------------------------------------
-- This is called once when the game is first started
-- ---------------------------------------------------------------------------
function game:InitGameSpecific()
    guiServer:Create( ) -- new gui (nGuiServer)    
    createGameObjectLibrary()
    game.initialStart = true
end
-- ---------------------------------------------------------------------------
-- This is called every time the game is (re)started
-- ---------------------------------------------------------------------------
function game:Run()
    avatar.Destroy() -- must do this before we delete it by recreating actorsPath!
	world.Create()
	if game.initialStart == true then
        guiServer:Activate('mainMenu' )
    end    
end

-----------------------------------------------------------
-- Everything after this is all ours
-----------------------------------------------------------
function game:Restart()
    writeDebug( '********Restart***********' )
    game.initialStart = false
    guiServer:Deactivate() -- turn off all menus (except the ingame menu)
    self.inProgress = true
    PruneNebThunks()
    --reset the cameras (their old positions may be invalid on the new map)
    flyCamera.Reset()
    chaseCamera:Reset()    
    obstacleCamera:Reset()
    self.object:start() -- calls game:Run()
end

-----------------------------------------------------------
-- Tell the engine which game to start
-----------------------------------------------------------

nebula.engine:setgame( gamePath )
