-- fly camera
flyCamera = { -- for debugging purposes; allows free movement around the world
    nearPlane = 100.0,
    angleOfView = 60.0,
    farPlane = 12000, 

-- reset returns the camera to the default:
  defaultPos = { 0, 1000, 0 }, 
  defaultOrientation = {1,0,1}, --the vector along which to look
--how quickly the controls move the camera:
  moveSpeed = 80,
  rotateSpeed = 0.15,
  path = gamePath..'/flycamera',
  class = 'ccfreecamera',
  isDebugCam = true
}

camera.instances.flyCamera = flyCamera

function flyCamera.Init()
    flyCamera.overheadViewHeight = flyCamera.farPlane - 100    
    call('setmovespeed', flyCamera.moveSpeed)
	call('setrotatespeed', flyCamera.rotateSpeed )
	flyCamera.defaultOrientationQuat = camera.OrientationVecToQuat( flyCamera.defaultOrientation )
	flyCamera.Reset( )
end

function flyCamera.Reset()
    freeCamera.Set( flyCamera, flyCamera.defaultPos, flyCamera.defaultOrientationQuat )
end

function flyCamera.SetOverhead()
    freeCamera.Set( flyCamera, { map.centre.x, flyCamera.overheadViewHeight, map.centre.y }, camera.OrientationVecToQuat( { 0, -1, 0 } ) )
end

function flyCamera:Activate()
    game.object:setdebugcamera(self.path)
end
