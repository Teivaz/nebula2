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
        call( 'setvector', 'LightDiffuse', 1.0, 1.0, 1.0, 1.0 )
        call( 'setvector', 'LightAmbient', 0.60, 0.60, 0.60, 1.0 )
        call( 'setposition', 1000, 500, 1000 )
        call( 'setvector', 'LightSpecular', 0, 0, 0, 1 )
        call( 'setvector', 'LightDiffuse1', 0, 0, 0, 1 )
        call( 'setvector', 'LightSpecular1', 0, 0, 0, 1 )
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