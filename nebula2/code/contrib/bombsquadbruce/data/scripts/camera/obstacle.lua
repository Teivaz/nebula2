-- the following camera checks for obstacles between the actual chase camera and the avatar
obstacleCamera = 
{
    nearPlane = chaseCamera.nearPlane,
    angleOfView = 20.0,
    farPlane = 280,
    offset = chaseCamera.cameraOffset,
    focalOffset = chaseCamera.focalOffset,
    decelFactor = chaseCamera.decelFactor,
    minHeightAboveGround = chaseCamera.minHeightAboveGround,

    path = gamePath..'/obstacleCamera',
    class = 'bbchasecamera',
    
    manipulatedLastTick = {}, -- a list of the objects we've made translucent, so we can make them untranslucent again when they are not longer in the way
    minAlpha = 0.3, -- how transparent the obstructing objects may become
    fadeRate = 0.05, -- per frame, not per second; this may be risky with very fast machines, but it looks bad when the alpha jumps due to a high deltaT
    
    Init = function()
        writeDebug( 'obstacleCamera.Init()' )
        obstacleCamera.object = lookup( obstacleCamera.path )
        obstacleCamera.LazyInit = chaseCamera.LazyInit
        pin( obstacleCamera.object  )
        obstacleCamera.object:settargetentitypath( playerPath )
        obstacleCamera:UpdateProperties() 
        function obstacleCamera.object:AdjustPosition( x, y, z, deltaTime )
            local pos = {x,y,z}
            pos = chaseCamera.SmoothMovement( pos, deltaTime, obstacleCamera )
            pos = camera.StayAboveGround( obstacleCamera, pos )
            
            --now we cheat and do stuff other than just adjust our position
            obstacleCamera.object:manipulatevisibleelements() -- calls DoManipulate on every object in our frustum
            local toRemove = {}
            for name, val in obstacleCamera.manipulatedLastTick do
                val.transparency = val.transparency + obstacleCamera.fadeRate
                if val.transparency > 1 then 
                    val.transparency = 1
                    table.insert( toRemove, name ) -- we haven't been in the frustum for a while -- remove us from manipulatedLastTick to avoid redundant setshaderoverride commands
                elseif val.transparency < obstacleCamera.minAlpha then
                    val.transparency = obstacleCamera.minAlpha
                end
                val.obj:setshaderoverridefloat4( 'MatDiffuse', -1,-1,-1,val.transparency ) -- this is redundant, but the number of objects is small
            end
            for idx, objName in toRemove do
                obstacleCamera.manipulatedLastTick[objName] = nil    
            end
                
            return pos[1], pos[2], pos[3]
        end
        begincmds( 'bbcamera', 1 )
            addcmd( 'bbcamera', 'v_DoManipulate_o' ) 
        endcmds( 'bbcamera' )	
        function obstacleCamera.object:DoManipulate( rootObj )
            --turns objects in our frustum semitransparent so they won't block the camera
            --but only after postprocessing the culling and removing (some) false positives
            if vecLen( vecDiff( {rootObj:getposition()}, {obstacleCamera.object:getposition()} ) ) < obstacleCamera.farPlane + 160 then
                local val = obstacleCamera.manipulatedLastTick[rootObj:getname()]
                if val == nil then
                    obstacleCamera.manipulatedLastTick[rootObj:getname()] = { transparency = 1.0 - obstacleCamera.fadeRate * 2, obj = rootObj }
                else
                    val.transparency = val.transparency - obstacleCamera.fadeRate * 2
                end
            end
        end               
    end,
    
    Activate = function(self)
        game.object:setcamera(self.path)
        game.object:setdebugcamera(flyCamera.path)
    end,
    
    UpdateProperties = function( self )
        self.object:setcameraoffset( self.offset[1], self.offset[2], self.offset[3] ) 
        self.object:setfocaloffset( self.focalOffset[1], self.focalOffset[2], self.focalOffset[3] ) 
    end,
    
    Reset = function( self )
        self.manipulatedLastTick = {} 
    end,
}

camera.instances.obstacleCamera = obstacleCamera