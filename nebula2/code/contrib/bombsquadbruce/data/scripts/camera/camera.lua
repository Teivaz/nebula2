-- camera.lua

camera.instances = {} -- a list of all the camera instances in the game, 
                                 --generated in the instance-specific files below

runscript( 'camera/fly.lua' )
runscript( 'camera/follow.lua' )
runscript( 'camera/obstacle.lua' )
-- ---------------------------------------------------------------------------
--  Set up camera
-- ---------------------------------------------------------------------------

function camera.InitAll()
    table.foreach( camera.instances, camera.Init )
end

function camera.Init( index, instance )
	local cwd = psel()
	selNew( instance.class, instance.path )
        call( 'setfarplane', instance.farPlane )
        call( 'setnearplane', instance.nearPlane )
        call( 'setangleofview', instance.angleOfView )
        instance.Init()
	sel(cwd)		
	--writeDebug( 'Done Init' )
end

function camera.OrientationVecToQuat( vec )
    vec = vecNormalize( vec )
    if 0 == vec[1] and 0 == vec[2] and 1 == vec[3] then
        return { 0, 1, 0, 0 }
    else
        return { vec[2], -vec[1], 0, 1 - vec[3] }
    end
end

function camera.StayAboveGround( instance, pos )
    local height = map.object:getheight( pos[1], pos[3] ) 
    pos[2] = math.max( pos[2], height + instance.minHeightAboveGround )
    return pos
end


-- ---------------------------------------------------------------------------
--   Functions for ccfreecameras only
-- ---------------------------------------------------------------------------

freeCamera = {}

function freeCamera.Set( instance, pos, orientation )
--pos as (x,y,z) array, orientation as quaternion (x,y,z,w) array
	local cwd = psel()
	sel(instance.path)
        if nil ~= pos then 
            call( 'setposition', pos[1], pos[2], pos[3])
        end
        if nil ~= orientation then
            call( 'setorientation', orientation[1], orientation[2], orientation[3], orientation[4] )
        end
	sel(cwd)
end
