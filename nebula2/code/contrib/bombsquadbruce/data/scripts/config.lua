language = 'english'
--language = 'deutsch'

debugControlsActive = true
logging = true

win = {
  --mode ='fullscreen', 
  mode = 'windowed', 
  xpos = 0,
  ypos = 0,
  xres = 800,
  yres = 600,
  title = 'Bombsquad Bruce',
}

distanceTransparency = 6500 -- the distance at which game objects because completely transparent
terrainTransparency = 10000 -- as above, but for the terrain 
--10000 looks good as far as fading out instead of popping out goes, but the horizon line is pretty visible through the transparent terrain
--13000 or more conceals the horizon line well, but then you need a larger farplane (>= 17000) to avoid popping

world = {
    gravity = 980, -- cm/s^2 
    wind = { 0.5, 0.0, 0.3, strength = 0.0, -- these will change in the course of the game
                strVariance = 0.75, -- the maximum amount the strength can change per second (but it will usually be less)
                smoothness = 1, -- the number of seconds that a particular strength rate-of-change tendency lasts
                dirVariance = 0.3, -- the maximum amount (0-1) that the direction is allowed to change every time the wind "dies down"
                dirChangeStrThreshhold = 0.01 -- the maximum wind strength that counts as "died down"
    },
}

avatar = {
    speed = { 
        forward = 700,
        rotation = 5,   
        pseudoRotation = 4, -- if you use the keys rather than the mouse, the speed at which the apparent rotation occurs     
    },
    accelerationTime = 0.6, -- how many seconds it takes for us to get up to full speed
    maxClimbableSlope = 30, -- The slope angle in degrees at which the avatar starts to slip downwards.
    slipFactor = 1250, -- how far the avatar slips when it tries to climb a slope steeper than MaxClimbableSlope
    collisionRadius = 10
}

camera = { farPlane = 12000 }--the bigger this is, the better the terrain looks, but the skybox horizon shifts more visibly if the farPlane is relatively near: e.g. map's maxHeight = 2000 -> farPlane <= 13000

function normalTreeInitFunc()
    defaultInitShaderFunc('tree.fx')
    call( 'setvector', 'MatDiffuse', 1,1,1,1 )
end


 --[[   
        billboardDistance is the distance beyond which billboards are used instead of models
        maxTilt is the cosine of the maximum slope on which this kind of tree is allowed to "grow"
        The rest of the parameters, you probably shouldn't need to change.  ]]
maxTiltForColliders = 0.17
vegetationDefs = 
{
    {
        name = 'tree01',
        subDir = 'trees',
        shaderFunc = normalTreeInitFunc,
        maxTilt = maxTiltForColliders,
        collRad = 90,
        alpha = true,
        LODDists = { billboardDist = 4000 }
    }
}

forest = {
    terrainMapTrees = {   vegetationDefs[1]  },
    billboardDistTransparency =6000 -- for reasons I can't understand, the billboard always looks foggier than the model, given the same fog settings and shader.
}
