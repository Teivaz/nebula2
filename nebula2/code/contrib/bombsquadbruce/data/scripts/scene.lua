-----------------------------------------------------------------------------
--  scene.lua
--  =========
--  Set up static scene (everything that the renderer just 
--  renders, rather than being passed it by the world)
--
-----------------------------------------------------------------------------

scenePath = gamePath..'/scene'

runscript('skybox.lua')


local function createLight()
    selNew('nlightnode','light')
        call( 'setvector', 'LightDiffuse', 1.2, 1.2, 1.2, 1.0 )
        call( 'setvector', 'LightAmbient', 0.60, 0.60, 0.60, 1.0 )
        call( 'setvector', 'LightPos', 0.5, -3, 1, 1 )
    sel('..')
end

function initScene()
    InitMap()
    local cwd = psel()
    selNew ('ntransformnode', scenePath )
        loadterrain('terrain:'..map.detailMap, 'terrain:'..map.textureMap, map.detailSize )
        createLight()	
        createskybox('sky')
    sel(mainRenderer.path)
        call( 'setscene', scenePath )
    sel(cwd)	
end
