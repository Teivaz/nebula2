function CreateForest()
    local i = 0
    for line in io.lines( map.treeCoordsPath ) do
        local parseFn = string.gfind( line, '%d+' )
        local xPos = parseFn() * map.gridInterval
        local yPos = parseFn() * map.gridInterval
        local vegDef = forest.terrainMapTrees[ math.random( table.getn( forest.terrainMapTrees ) ) ]
        local tilt = GetTilt( xPos, yPos )
        if tilt < vegDef.maxTilt then
            if true == CreateObj( vegDef.name, vegDef.collRad, xPos, yPos, getInitVegFunc( vegDef ) ) then
                i=i+1
            end
        end
    end
    writeDebug( "Placed "..i.." trees." )
end

function getInitVegFunc( vegDef )
    local cwd = psel()
    sel( '/sys/servers/scene/' )
        local alphaFlagHandle = call( 'getalphaflaghandle' )
        local LODDistHandle = call('getloddisthandle')
        local BillboardHandle = call('getlodshapehandle', 1)
    sel( cwd )
        
    return function()
        if true == vegDef.alpha then
            call( 'setintvariable', alphaFlagHandle, 1 )
        end
        if vegDef.LODDists ~= nil then 
            call( 'setfloat4variable', LODDistHandle, vegDef.LODDists.lowpolyDist, vegDef.LODDists.billboardDist, vegDef.LODDists.cullDist, -1 )
        end
        call( 'setpathvariable', BillboardHandle, modelsPath..'/'..vegDef.name..'_bill' )
    end
end

function GetTilt( posx, posy )
    local normal = { map.object:getnormal( posx, posy ) }
    return math.abs( vecDot( normal, { 1, 0, 0 } ) )
end
