-----------------------------------------------------------------------------
--  skybox.tcl
--  ==========
--  Provides method to create a skybox
--
-----------------------------------------------------------------------------
--

-----------------------------------------------------------------------------
--  Create a skybox plane, unable to share nshadernode
-----------------------------------------------------------------------------
function createskyboxplane(name, texture, position, orientation )
	local cwd = psel()
	selNew('nshapenode',name)
        call('seteuler', orientation.x, orientation.y, orientation.z )
        call('setposition', position.x * skybox.scale , position.y * skybox.scale, position.z * skybox.scale )
        call('setshader', 'colr', 'shaders:skybox.fx' )
        call('setmesh',  'skybox:plane.n3d2' )
        call('settexture', 'DiffMap0', 'skybox:'..texture )
        call('setgroupindex', 0 )
        call('setrenderpri', -10 )
        call( 'setscale', skybox.scale, skybox.scale, skybox.scale )
    sel(cwd)
end

-----------------------------------------------------------------------------
--  Create a skybox
-----------------------------------------------------------------------------
function createskybox(name)
        -- we make a big skybox instead of a little one centred on the user's head because the latter prevents the horizon, as defined by the water, from always intersecting the same skybox plane, which in turn makes it impossible to make a good skybox texture that includes land as well as sky
        -- the problems with this approach are:
        --  1. The skybox can only be as big as the far plane allows, which means it conceals distance objects, so that e.g. fog must be adjusted to the skybox distance
        --  2. The intersection of water plane and skybox is visible at the corners, where two straight water edges run together.  The solution to this would, presumably, be to use a skydome instead
    local cwd = psel()
    selNew( 'bbshapenode', name )
        writeDebug( "scale = "..skybox.scale )
        call( 'setlockviewerxz', true ) -- don't lock y, because we want to interact naturally with the water horizon as noted above
        call( 'setposition',  0, water.level + skybox.horizonOffset, 0 ) -- keeps the horizon fixed at the water level
        createskyboxplane('front','sky_f.dds', { x = 0, y = 0, z = -skybox.baseSize }, { x = 90, y = 0, z = 0 } )
        createskyboxplane('back','sky_b.dds', { x = 0, y = 0, z = skybox.baseSize }, { x = 90, y = 180, z = 0 } ) 
        createskyboxplane('left','sky_r.dds', { x = -skybox.baseSize, y = 0, z = 0 }, { x = 90, y = 90, z = 0 } ) 
        createskyboxplane('right','sky_l.dds', { x = skybox.baseSize, y = 0, z = 0 }, { x = 90, y = -90, z = 0 } ) 
        createskyboxplane('up','sky_u.dds', { x = 0, y = skybox.baseSize, z = 0 }, { x = 180, y = 90, z = 0 } ) 
        createskyboxplane('down','sky_d.dds', { x = 0, y = -skybox.baseSize, z = 0 }, { x = 0, y = -90, z = 0 } ) 
    sel(cwd)
end

