runscript('scene.lua')
runscript('camera/camera.lua')

mainRenderer = {
    path = gamePath..'/renderer' 
}

-- the following method is "inherited" (from gameUniversal, GameT:Init()); don't rename it
function mainRenderer:Init() 
	local cwd = psel()
	selNew ('ccrenderer', self.path )
        camera.InitAll()
    sel( gamePath )
    call( 'setrenderer', self.path )
	sel(cwd)
end

function showStats(statsToWatch)
	nebula.sys.servers.console:unwatch()
	nebula.sys.servers.console:watch(statsToWatch)
end

