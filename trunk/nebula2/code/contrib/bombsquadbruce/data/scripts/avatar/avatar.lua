--avatar.lua

avatarModelName = avatarName..'_Animation'

avatar.anims = 
{
    idle = { state = 1,           spd = 0.25             --[[numFrames = 20]] }, 
    run = { state = 0,            spd = 1.2              * avatar.speed.forward / 700, numFrames = 20 }, 
    
    secondsPerFrame = 0.04
}

avatar.sounds = {
    initialRunTimerVal =  4 * avatar.anims.secondsPerFrame / avatar.anims.run.spd,
    timeBeforeStep = avatar.anims.secondsPerFrame * avatar.anims.run.numFrames / (2*avatar.anims.run.spd)
}
   
function createAvatar()
    writeDebug( 'Create avatar' )
    local cwd = psel()
    determineStartPos()
    selNew( 'bbmobileactor',playerPath )
        avatar.object = lookup( getCwd() )
        
    avatar.object:setshapenode( modelsPath..'/'..avatarModelName..'/model/skinned0' )
    avatar.curAnimState = {}
    avatar.SetAnimState( 'idle' )
    avatar.object:setmaxspd( avatar.speed.forward )
    avatar.object:setapparentturnspd( avatar.speed.pseudoRotation )
    avatar.object:setturnspd( avatar.speed.rotation )
    local height = map.object:getheight(avatar.startXPos, avatar.startYPos)
    avatar.object:setposition( avatar.startXPos, height, avatar.startYPos ) -- Avatar shouldn't start on a slab to avoid possible immediate death
    writeDebug( 'Player pos = '..avatar.startXPos..', '..height..', '..avatar.startYPos )
    writeDebug( water.object:getlevel() )
    avatar.object:setcollisionradius( avatar.collisionRadius )
    avatar.lastGoodXZPos = { x = avatar.startXPos, z = avatar.startYPos }
    avatar.moveState = "Walk"
    avatar.v = { 0, 0, 0 } --velocity
    avatar.lastStrafeSpeed = 0
    avatar.lastSpeed = 0
    avatar.anims.playTime = nil
    avatar.sounds.runTimer = nil
    avatar.InitSounds()
    writeDebug( 'createAvatar done.' )
end

function avatar.Destroy()
    if avatar.object ~= nil then 
        avatar.object = nil 
    end
end

function loadAvatarData()
    engineObj:loadobject( 'objects:'..avatarName..'/'..avatarModelName..'.n2' )
    -- create hardpoints for subobjects
    assert( modelsPath == getCwd() )
    local cwd = psel()
        sel( avatarModelName )
            sel( 'model/skinned0' )
                selNew( 'nattachmentnode', 'rightHand' )
                    call( 'setjointbyindex', 16 )
                    engineObj:loadobject( 'objects:equipment/bomb_sniffer.n2' )
                    sel( 'bomb_sniffer' )
                        call('seteulerz', 90 )
                        call('setactive', true )
                    sel('..')
                sel('..')                    
    sel( cwd )
end

function determineStartPos()
	getLine = io.lines( map.playerCoordsPath )
        parseFn = string.gfind( getLine(), '%d+' )
        avatar.startXPos = parseFn() * map.gridInterval
        avatar.startYPos = parseFn() * map.gridInterval
end

--private methods
function avatar.InitSounds()
    local cwd = psel()
    sel( playerPath )
        avatar.sounds.step = soundMgr:CreateSound( 'footstep', false, true, 0.5 )
        avatar.sounds.stepSplash = soundMgr:CreateSound( 'footstepWater', false, true, 0.7 )
    sel( cwd )
end

--- callback & input functions
function avatar.Trigger( deltaTime )
    --don't set the avatar's position directly; instead,
    --manipulate its velocity
    if world.object:isgameactive() then
        avatar.rotationalVelocity = avatar.object:getcurturn() -- the player can always turn regardless of moveState
        avatar.DoPlayerMotion(deltaTime)
        avatar.StickToGround()
        avatar.object:move( avatar.v[1], avatar.v[2], avatar.v[3], avatar.rotationalVelocity, deltaTime )
        avatar.StayAboveGround()
        avatar.AdjustAnim(deltaTime)
        avatar.PlayFootsteps(deltaTime)
    end
end

-- "Private Methods"
function avatar.StayAboveGround()
    x,y,z = avatar.object:getposition()
    local groundLevel = map.object:getheight(x,z)
    if y < groundLevel then
        avatar.object:setposition( x, groundLevel, z )
    end
end

function avatar.SetVelocity(x,y,z)
    if x ~= nil then avatar.v[1] = x end
    if y ~= nil then avatar.v[2] = y end
    if z ~= nil then avatar.v[3] = z end
end

function avatar.Accelerate( x, y, z )
    if x ~= nil then avatar.v[1] = avatar.v[1] + x end
    if y ~= nil then avatar.v[2] = avatar.v[2] + y end
    if z ~= nil then avatar.v[3] = avatar.v[3] + z end
end

function avatar.CalcSpeed( deltaTime, spd, lastSpeed, timeSoFar )
    if spd == 0 then
        avatar[lastSpeed] = 0
    else
        if        (spd > 0 and avatar[lastSpeed] <= 0)
              or (spd < 0 and avatar[lastSpeed] >= 0) then
            avatar[lastSpeed] = spd
            avatar[timeSoFar] = deltaTime
        end
        if avatar[timeSoFar] < avatar.accelerationTime then
            spd = spd * avatar[timeSoFar] / avatar.accelerationTime
            avatar[timeSoFar] = avatar[timeSoFar] + deltaTime
        end
    end
    return spd       
end

function avatar.DoPlayerMotion(deltaTime)
    local headingVec = {avatar.object:getheadingvector()}
    local curStrafeSpd = avatar.CalcSpeed(deltaTime, avatar.object:getcurstrafe(), 'lastStrafeSpeed', 'strafeAccelerationTime' )
    local deltaHeight = 0
    local strafeVector = { headingVec[3] * curStrafeSpd, deltaHeight, -headingVec[1] * curStrafeSpd }
    local fwdSpeed = avatar.CalcSpeed(deltaTime, avatar.object:getcurfwd(), 'lastSpeed', 'fwdAccelerationTime' )
    local fwdVector = vecScale( headingVec, fwdSpeed )
    local newVelocity = vecSum( fwdVector, strafeVector )
    avatar.SetVelocity( newVelocity[1], newVelocity[2], newVelocity[3] )
end

function avatar.StickToGround( )
    local x,y,z = avatar.object:getposition()
    local groundLevel = map.object:getheight(x,z)
    avatar.SlipDownSlope( x, y, z )
end

function avatar.SlipDownSlope( x, y, z )
    -- calculate the slope at Avatar's current position
    local normal = { map.object:getnormal( x, z ) } -- already normalized
        --we're interested in the angle between the normal and the x,z plane, 
        --which is (pi - cos^-1(dotProduct( normal, xzPlaneNormal )), but xzPlaneNormal == 0,1,0 so
        --the angle is pi - cos^-1(normal[2])
        --we don't care about the actual slope angle in some particular unit, though -- just if it's a lot or a little.
        --so screw the cos and just use 1 - normal[2]
        --Then we have a slope ranging from 0 (standing on flat plane --> normal[2]==1) to 1 (standing on vertical plane --> normal[2] == 0)
    local slope = 1 - normal[2]
    if slope > avatar.cosMaxClimbableSlope then
        -- push avatar in the direction of the normal, then have him fall to the ground at that point
        -- this will have the desired effect of causing him to slip farther if the slope is steeper
        avatar.Accelerate( normal[1] * avatar.slipFactor, nil, normal[3] * avatar.slipFactor )
    else -- we are on the ground and it isn't too steep
        avatar.SaveLastGoodPosition(x,z)
    end
    avatar.Accelerate( nil, -10000, nil )
end

function avatar.SaveLastGoodPosition(safeX, safeZ)
    -- everytime the avatar leaves the ground, there is a danger that
    -- it will land in a collision.  Here, we save the last position on the
    -- ground, so we can restore the avatar to that point if it collides.
    avatar.lastGoodXZPos.x = safeX
    avatar.lastGoodXZPos.z = safeZ
end

function avatar.ResetPosition()
    --called to escape from collision:
    --returns the avatar to lastGoodPos,
    --turning off all items
    avatar.object:setposition( avatar.lastGoodXZPos.x,  map.object:getheight( avatar.lastGoodXZPos.x, avatar.lastGoodXZPos.z ), avatar.lastGoodXZPos.z )
    avatar.moveState = "Walk"
    avatar.v = { 0, 0, 0 } --velocity
end

function avatar.AdjustAnim( deltaTime ) -- PoP Controls
--you need to add the turn animations somehow!
--if there are any....
--in any case, you need controls s.t. if the user taps a direction the avatar isn't facing,
--you initiate a turn to face that direction perfectly.  This must be very quick, but not instant.
--If they press any arrow, they immediately move in that direction, regardless of what direction
--it looks like they're facing.  So you must uncouple model orientation from avatar rotation.
    if avatar.IsPlayingAnim() then
        avatar.anims.playTime = avatar.anims.playTime + deltaTime
        if avatar.anims.playTime > avatar.anims.totalPlayTime then
            avatar.anims.playTime = nil
        end
    else
        local newAnimStateName = avatar.curAnimState.name
        if avatar.moveState == "Walk" or avatar.moveState == "WaterWalk" then
            if avatar.v[1] == 0 and avatar.v[3] == 0 then
                newAnimStateName = 'idle'
            else
                newAnimStateName = 'run'
            end
        else
            newAnimStateName = 'idle'
        end
        avatar.SetAnimState( newAnimStateName )
    end
end

function avatar.SetAnimState( newStateName )
    local stateData = avatar.anims[newStateName]
    local stateNum = stateData.state
    if stateNum ~= avatar.curAnimState.num then    
        avatar.curAnimState.num = stateNum
        avatar.curAnimState.name = newStateName
        if newStateName == 'run' then 
            avatar.sounds.runTimer  = avatar.sounds.initialRunTimerVal
        else
            avatar.sounds.runTimer  = nil
        end
        avatar.object:setanimstate( stateNum, stateData.spd )
    end
end

function avatar.StartOneShotAnim(name)
    avatar.anims.playTime = 0
    avatar.anims.totalPlayTime = avatar.anims.secondsPerFrame * avatar.anims[name].numFrames / avatar.anims[name].spd
    avatar.SetAnimState( name )
end

function avatar.IsPlayingAnim()
    return avatar.anims.playTime ~= nil 
end

function avatar.PlayFootsteps(deltaTime)
    if avatar.sounds.runTimer ~= nil then 
        avatar.sounds.runTimer = avatar.sounds.runTimer + deltaTime
        if avatar.sounds.runTimer >= avatar.sounds.timeBeforeStep then
            avatar.sounds.runTimer = avatar.sounds.runTimer - avatar.sounds.timeBeforeStep
            if avatar.moveState == 'Walk' then            
                local x, y, z = avatar.object:getposition()
                local cmAboveWater = y - water.object:getlevel()
                if cmAboveWater >= 0 then -- on land
                    avatar.sounds.step:Play()
                elseif cmAboveWater >= -70 then --- splashing in the water
                    avatar.sounds.stepSplash:Play()
                --else too deep for step sounds
                end
            else
                avatar.sounds.stepSplash:Play()
            end
        end
    end
end
