-- ---------------------------------------------------------------------------
--
-- set the heightmap
--
-- ---------------------------------------------------------------------------

mapPath = '/data/map'
mapResourceLoaderPath = mapPath..'/resourceloader'

function initMap( )
    local cwd = psel()
	selNew( 'nmap',mapPath )
        map.object = lookup( mapPath )
        call( 'setheightmap', map.heightMapPath )
        call( 'setgridinterval', map.gridInterval )
        call( 'setheightrange', map.minHeight, map.maxHeight )
    sel( cwd )
    call( 'setmap', mapPath)

end

-- ---------------------------------------------------------------------------
--   Load a terrain with default attributes
-- ---------------------------------------------------------------------------

function loadterrain(detailMap, textureMap, detailSize )
    selNew('nmapnode','map')
        initMap()
        call( 'setblocksize', 33 )
        call( 'seterror', 1 )
        call( 'setdetailsize', detailSize )
        call( 'setposition', 0, 0, 0 )
        call( 'seteuler', 0, 0, 0 )
        call( 'setactive', true )
        call( 'settexture', 'DiffMap0', textureMap )
        --call( 'settexture', 'DiffMap1', detailMap )
        call( 'setshader', "colr", "shaders:terrain.fx" )
        call( 'setfloat', 'MatTranslucency', terrainTransparency )
        local oldMeshUsage = call( 'getmeshusage' )
        call( 'setmeshusage', oldMeshUsage..'NeedsVertexShader' )
        new( 'nmapresourceloader', mapResourceLoaderPath )
        call( 'setresourceloader', mapResourceLoaderPath )
	call( 'setrenderpri', -1 )
    sel('..')
end

