map = {
    gridInterval = 180, --scales the map (but don't change this much, or tree barriers defined in terrain maps won't work)
    detailMap = 'detailmap.dds', -- currently unused
    heightmapSize = 65, -- the side length of the (square) bitmap in pixels.  This minus one must be divisible by 32.
    minHeight = -500,
    maxHeight = 500,
    textureMap = 'grastextur01.dds',
}

local function GetCoordsFilePath( baseFilename )
    return mangle( 'terrain:HT-Maps/'..baseFilename..'.txt' )
end

local function SetCoordsFilePaths( )
    map.treeCoordsPath = GetCoordsFilePath( 'treeCoords' )
    map.gridCoordsPath = GetCoordsFilePath( 'gridCoords' )
    map.playerCoordsPath = GetCoordsFilePath( 'playerCoords' )
end

function InitMap()
    map.farCorner = { x = map.heightmapSize * map.gridInterval, y = map.heightmapSize * map.gridInterval } 
    map.centre = { x=map.farCorner.x/2, y = map.farCorner.y / 2 }
    map.detailSize = 128 * 7 * map.gridInterval
    water.scale = math.min( math.max( map.farCorner.x, map.farCorner.y ), 2 * camera.farPlane ) / water.meshSize
    map.heightMapPath = mangle( 'terrain:HT-Maps/height.bmp' )
    SetCoordsFilePaths()
end
