-- Follow cam (the main gameplay camera -- hovers behind the avatar's shoulder)

chaseCamera = {
    nearPlane = 10, 
    angleOfView = 60.0,
    farPlane = camera.farPlane, 

    cameraOffset =  { 0, 320, -300 },
    focalOffset = { 0, 180, 0 } ,

    minZoomDist = 0,
    maxZoomDist = 2500,
    zoomSpeed = 60,
    moveSpeed = 80,
    orbitSpeed = 0.03,
    rotateSpeed = 1.5,
    minHeightAboveGround = 100, -- in cm
    decelFactor = 3.5, -- the higher  this is, the slower the camera approaches its final target position
    lockHeight = false, 
    path = gamePath..'/followcamera',
    class = 'bbchasecamera'
}


camera.instances.chaseCamera = chaseCamera


function chaseCamera.Init()
    writeDebug( 'chaseCamera.Init()' )
    
    -- Declare some additional script cmds 
    begincmds( 'bbchasecamera', 1 )
        addcmd( 'bbchasecamera', 'fff_AdjustPosition_ffff' ) --see below for definition
    endcmds( 'bbchasecamera' )	

	chaseCamera.object = lookup( chaseCamera.path )
	pin( chaseCamera.object  )

    chaseCamera.object:setminzoomdist( chaseCamera.minZoomDist )
	chaseCamera.object:setmaxzoomdist( chaseCamera.maxZoomDist )
	chaseCamera.object:setzoomspeed( chaseCamera.zoomSpeed )
	chaseCamera.object:setmovespeed( chaseCamera.moveSpeed)
	chaseCamera.object:setorbitspeed( chaseCamera.orbitSpeed )
	chaseCamera.object:setrotatespeed( chaseCamera.rotateSpeed )		
    chaseCamera.object:settargetentitypath( playerPath )
    local offset = chaseCamera.cameraOffset
    chaseCamera.object:setcameraoffset( offset[1], offset[2], offset[3] ) 
    offset = chaseCamera.focalOffset
    chaseCamera.object:setfocaloffset( offset[1], offset[2], offset[3] ) 
	
	--define the commands for this instance of the camera object
	
	-- AdjustPosition is called automatically by BBChaseCamera::Trigger(),
	-- which is in turn called by the game
	function chaseCamera.object:AdjustPosition( x, y, z, deltaTime )
        obstacleCamera.object:trigger(deltaTime)
        local pos = chaseCamera.SmoothMovement( {x,y,z}, deltaTime, chaseCamera )
        pos = camera.StayAboveGround( chaseCamera, pos )
  		return pos[1], pos[2], pos[3]
	end
end

function chaseCamera.GetPos()
    return chaseCamera.pos
end

function chaseCamera.GetOrientation()
    return {chaseCamera.object:getorientation()}
end

function chaseCamera.SmoothMovement( desiredPos, deltaTime, curCamera )
    curCamera:LazyInit()
    if nil == curCamera.pos then
        curCamera.pos = desiredPos
    else
        local deltaPos = vecDiff( desiredPos, curCamera.pos ) -- in cm
        local desiredVelocity = vecScale( deltaPos, 1 / deltaTime ) -- in cm/s; if attained, this would get us to the goal in this tick
        local limitedVelocity = vecScale( desiredVelocity, 1 / curCamera.frameAdjustedDecelFactor )       
        if vecLen( limitedVelocity ) > curCamera.maxSpeed then
            limitedVelocity = vecScale( limitedVelocity, curCamera.maxSpeed / vecLen( limitedVelocity ) )
        end
        curCamera.velocity = limitedVelocity
        curCamera.pos = vecSum( curCamera.pos, vecScale( curCamera.velocity, deltaTime ) ) -- cm
        if curCamera.lockHeight then
            curCamera.pos[2] = desiredPos[2]
        end
    end
    return curCamera.pos
end

function chaseCamera:LazyInit()
    if nil == self.frameAdjustedDecelFactor then
        --This ensures that the camera takes the same amount of *time* to
        -- reach its target, not the same number of *frames*
        assert( game.ticksPerSecond > 0 )
        self.frameAdjustedDecelFactor = math.pow( self.decelFactor, game.ticksPerSecond / 30 )
    end
    if nil == self.maxSpeed then
        self.maxSpeed =  avatar.speed.forward -- assumes forward is the fastest speed
    end
end

function chaseCamera:Reset()
    self.pos = nil
end

function chaseCamera:Activate()
    game.object:setcamera(self.path)
end
