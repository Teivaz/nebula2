--gameUniversal.lua

-- GameT is the generic game class
GameT = {
    initialized = false,
    inProgress = false,
}
 
--[[
    The main function of "game" is to trigger the scripts that define
    the particular game played.
    
    Different scripts may ask to be triggered at different rates.
    There are two options:
    1.  As often as possible
    2.  At as close to a fixed rate as possible.
    A third type, which runs as often as possible up to a certain rate,
    is also conceivable; one would have to decide if it would try to 
    approximate its maximum rate as nearly as possible, which would
    involve accumulating fractions to, say, activate in two out of three 
    frames, or whether it would conservatively drop all fractions every 
    time is was activated (so if its max was 40 and the frame rate was
    60, it would only have an effective rate of 30).
    Whether such a third type makes sense is unclear: since type 1
    scripts are only activated when type 2 scripts have been taken care
    of, type 1 scripts could be used for unimportant stuff (even if expensive)
    as long as all essential activities (e.g. game logic and user input)
    were run as type 2 scripts.  OTOH, that may result in redundant looping
    if  scripts are only set to type 2 to designate them as "essential".
    An alternative would be to set "inessential" tasks as type 2, with a 
    very low maximum rate.  Depending on how low, however, they might 
    either be unnecessarily (on fast machines) choppy, or overload slow ones.
    Also, spikes might occur when such tasks were activated, causing frame
    rate stuttering; making such tasks cheaper (probably by making them
    capable of stopping and continuing their work) would be a better solution.
    
    An "input tick" is also conceivable, but it is not yet clear if user input
    can be separated from the rest of the game logic to any useful degree.

    Rendering always occurs as fast as possible, to avoid (on powerful
    machines) "tearing" caused by a render rate that is less than and not a 
    divisor of the monitor's refresh rate: renderer @ 60Hz + monitor@90Hz -->
    monitor displays image 1 for 2/90 of a sec, then image 2 for 1/90, etc.
    On less powerful machines, the choice is to activate video synch (which
    will drop the render rate to the nearest divisor or the refresh rate) or
    live with the tearing. This is the driver's problem, not ours.

    If other game systems are not going to be called in synch with the renderer
    (that is, as often as possible), then they need to provide a way for the 
    renderer to interpolate between two of their states; otherwise, we get 
    tearing again as, say, the physics boxes get rendered twice in state 1,
    once in state 2, twice in state 3, etc.
]]

GameT.registryRegistry = {
    fixedTick = {},
    variableDelta = {}
}

--[[
    function registerScript( func )
    Register a script to be to be run as often as possible.

    @param func The name of the script function to be called

    There is no minimum guaranteed rate, nor is the time between two calls 
    guaranteed to be anywhere near constant.  The script will be passed the
    length of time between this call and its previous call.
]]

function GameT:RegisterScript( func )
	table.insert( self.registryRegistry.variableDelta, func )
end

--[[
    function registerFixedTickScript( func, callsPerSecond )
    Register a script to be to be run at a fixed rate,
    given by the game tick.

    @param scriptName      The name of the script function to be called.

    Notionally, the script is called once every 1/gameTick seconds
    (one "tick").  In practice, this is only approximate, even in the best 
    case -- the call will always lag slightly behind its scheduled time.
    If this lag exceeds one tick, the script, when it finally does get
    called, will be called repeatedly until its local time 
    (=global starting time + time of one tick * number of ticks so far)
    "catches up" to within one tick of the global current time.

    To simulate a perfectly regular update, the script will always be passed 
    the theoretical tick time.

    E.g. Suppose and you've registered FixScript with gameTick==12.
    If GameTick::Trigger() is called 8 times per second:
        1st Trigger(): call FixScript(1/12)
        2nd Trigger(): call FixScript(1/12) twice
        Repeat the above four times per second for a total of 12 calls.
    If GameTick::Trigger() is called 12 times per second, 
        FixScript(1/12) will be called once per Trigger() call -- 12 / sec.
    If GameTick::Trigger() is called 18 times per second:
        1st Trigger(): don't call FixScript
        2nd Trigger(): call FixScript(1/12)
        3rd Trigger(): call FixScript(1/12)
        Repeat the above three times per second for a total of 12 calls.
]]
function GameT:RegisterFixedTickScript( func )
	table.insert( self.registryRegistry.fixedTick,  func )
end

function GameT:InitRegistries()
    function self.object.RunScripts( obj, scriptType, deltaTime )
		table.foreachi( self.registryRegistry[scriptType], 
                function( index, script )
				pcall( script, deltaTime )
			end )
	end
end

--InitGame sets up the infrastructure for the script registries,as well as
--any other initialization calls that will be the same for all games
--It is called once, when the game is created.
function GameT:Init()
    self.object:setgametick( self.ticksPerSecond )  
    self:RegisterScripts()
    self.renderer:Init()
    self:InitGameSpecific()
    self.initialized = true
end
    
function GameT:Constructor()
    begincmds( 'ccgame', 2 )
    addcmd( 'ccgame', 'v_Run_v' )
    addcmd( 'ccgame', 'v_RunScripts_sf' )
    endcmds( 'ccgame' )

    pin( self.object )
    function self.object.Run( obj )
        if not self.initialized then
            self:Init()
        end
        self:Run()
    end
    
    self:InitRegistries()
end
