--   water.lua: Configurable water parameters
-- ---------------------------------------------------------------------------
water = {
    level = -20, -- keep in mind that this affects all maps, so you probably don't want to change it
    waveHeight =10.0, -- how much the water rises and falls
    waveFrequency = 0.5, -- how often (how quickly) it does so 
    rippleSpeed = 0.005, -- how fast the texture slides along the surface
    surfaceAlpha = 0.8, -- how transparent the water surface is
    layerAlpha = 0.35, -- how transparent the layers of water under the surface are (smaller value will make the layering more subtle)
    numLayers = 3, --how many "depth" layers
    meshSize = 400, -- water.n3d2 is meshSize x meshSize
    numTiles = 20 -- water.n3d2 is divided into numTiles * numTiles quads
}

-- ----------------------------------------------------------------------------
--   water methods (don't mess with these unless you 
--   know what you're doing)
-- ---------------------------------------------------------------------------


function createWater()
    local cwd = psel()
    sel( modelsPath )
        local waterName = 'water'
        loadmesh( waterName, 'Nature/water', 'water.n3d2', 'water.dds', getWaterInitFunc( water.rippleSpeed ) )
        sel( cwd )
        selNew('ccwater', waterName )
            waterPath = getCwd()
            water.object = lookup( waterPath )
            water.object:setshapenode( modelsPath..'/'..waterName )
            water.object:setwaveproperties( water.waveHeight, water.waveFrequency )
            writeDebug( 'waterLevel='..water.level )
            water.object:setbasewaterlevel( water.level )
            defineWaterLayer( water.layerAlpha )
            for i = 1, water.numLayers do
                water.object:addlayer( i * 30 )
            end
    sel(cwd)
end

function commonWaterInitFunc()
    local oldMeshUsage = call( 'getmeshusage' )
    call( 'setmeshusage', oldMeshUsage..'NeedsVertexShader' )
    call( 'setscale', water.scale, 1, water.scale )
    call( 'setlockviewerxz', true )
end

function getWaterInitFunc( rippleSpeed )
    return function()
        call('setshader', 'colr', 'shaders:water.fx' )
        call( 'setrenderpri', 1 )
        call( 'setfloat', 'Frequency', rippleSpeed )
        call( 'settexcoordscale', water.meshSize * water.scale / water.numTiles )
        call( 'setvector', 'MatTransparency', 0.0, 0.0, 0.0, water.surfaceAlpha )
        commonWaterInitFunc()
    end
end

function defineWaterLayer( alpha )
  --Additional water plane underneath the main water surface
  --contribute to gradually increasing opacity for deeper water
  -- (which, among other things, is useful for hiding the lower half of the skybox)
  -- Depth is measured from the water's lowest point (water.level - water.waveHeight)
    local cwd = psel()
    sel( modelsPath )
        local name = 'waterLayer' 
        loadmesh( name, 'Nature/water', 'water_1x1.n3d2', 'water.dds', getWaterLayerInitFunc( alpha ) )
        local layerPath = getCwd()..'/'..name
    sel( cwd )
    call( 'setlayernode', layerPath )
end

 function getWaterLayerInitFunc( alpha )
     return function()
        call('setshader', 'colr', 'shaders:underwater.fx' )
        call( 'setvector', 'MatTransparency', 0.0, 0.0, 0.0, alpha )
        commonWaterInitFunc()
    end
end




