-- setdependentvars.lua

function initLODDists( index, vegDef )
--sets defaults where needed and squares the given distances (because everything works with squared distances internally)
    if vegDef.LODDists ~= nil then
        if vegDef.LODDists.cullDist == nil then
            vegDef.LODDists.cullDist = distanceTransparency
        else 
            vegDef.LODDists.cullDist = vegDef.LODDists.cullDist * vegDef.LODDists.cullDist
        end
        if vegDef.LODDists.billboardDist == nil then
            vegDef.LODDists.billboardDist = vegDef.LODDists.cullDist
        else
            vegDef.LODDists.billboardDist = vegDef.LODDists.billboardDist * vegDef.LODDists.billboardDist
        end
        if vegDef.LODDists.lowpolyDist == nil then
            vegDef.LODDists.lowpolyDist = vegDef.LODDists.billboardDist
        else
            vegDef.LODDists.lowpolyDist = vegDef.LODDists.lowpolyDist * vegDef.LODDists.lowpolyDist
        end
    end
end


skybox = {
    baseSize = 5
}
skybox.scale = camera.farPlane / ( skybox.baseSize * math.sqrt(3) )  --ensures that the skybox isn't clipped by the far plane, even at the corners of the cube            
skybox.horizonOffset = 210 * camera.farPlane / (1000*skybox.baseSize) -- you don't even want to know, except to observe that as the skybox gets bigger, its position relative to the horizon must increase proportionally

distanceTransparency = distanceTransparency * distanceTransparency -- because the .fx files use distance squared
terrainTransparency = terrainTransparency * terrainTransparency
forest.billboardDistTransparency = forest.billboardDistTransparency * forest.billboardDistTransparency
table.foreach( vegetationDefs, initLODDists )

avatar.cosMaxClimbableSlope = 1 - math.cos( avatar.maxClimbableSlope * math.pi / 180 ) 
