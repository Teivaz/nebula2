-- ---------------------------------------------------------------------------
--   mesh.lua
--   ========
--   Handy utility methods relating to meshes and models.
--
-- ---------------------------------------------------------------------------
--[[
function simpleloadmesh( uniqueName, dataName )
  --assumes that dataName describes the filenames for mesh and texture, and the subdirectory of objects: in which these are to be found
  loadmesh( uniqueName, dataName, dataName..'.n3d2', dataName..'.dds', defaultInitShaderFunc )
end]]
 	
-- ---------------------------------------------------------------------------
--   Load a mesh with default attributes
--  If you want to add multiple possible textures, with an 
--  nTextureAnimator, set the texture parameter to a table
-- { animatorName='someName', animatorChannel='someChannel', 'tex1.dds', 'tex2.dds', ... }
-- initFunc should, at least, set the shader, and may include other arbitrary calls
-- ---------------------------------------------------------------------------
function loadmesh (uniqueName, dir,mesh,texture, initFunc )
    if initFunc == nil then
        initFunc = defaultInitShaderFunc
    end
    dir = dir..'/'
    selNew ('ccshapenode', uniqueName )
        writeDebug( 'loadmesh created '..getCwd() )
        initFunc()
        call('setmesh',  'objects:'..dir..mesh )
        --call('setgroupindex', 0 )
        if type(texture) == 'table' then
            selNew('ntextureanimator', texture.animatorName )
                call( 'setlooptype', 'oneshot' )
                call( 'setchannel', texture.animatorChannel )
                call( 'setshaderparam', 'DiffMap0' )
                table.foreachi( texture, addTextureChoice('objects:'..dir) )
            sel( '..' )
        else
            call('settexture', 'DiffMap0', 'objects:'..dir..texture )
        end
    sel('..')
end

function addTextureChoice( texturePath )
    return function( index, texture )
        call( 'addtexture', texturePath..texture )
    end
end

uniqueID = -1
function CreateObj( objDir, collisionRadius, xpos, zpos, initActorFunc, showErrorOnFail )
	local cwd = psel()
	uniqueID = uniqueID + 1
	local uniqueName = objDir..'Actor-'..uniqueID 
	local retVal = false
	sel( actorsPath )
	selNew('ccactor', uniqueName )
		call('setshapenode', modelsPath..'/'..objDir )
		call('setposition', xpos, 0, zpos )
		if collisionRadius == nil then
            collisionRadius = -1
        end
		call('setcollisionradius', collisionRadius )
		call('setrotation', math.random() * math.pi * 2 )
		if initActorFunc ~= nil then
            initActorFunc()
        end
	sel(worldPath)
		retVal = call('addgameobject', actorsPath..'/'..uniqueName )	
		if false == retVal and false ~= showErrorOnFail then
            writeDebug( 'Failed to CreateObj '..uniqueName..' ('..objDir..') at '..xpos..', '..zpos )
        end
	sel(cwd)
	return retVal
end

function defaultInitShaderFunc(shader, distTrans )
    if shader == nil then
        shader = 'default.fx'
    end
    if distTrans == nil then
        distTrans = distanceTransparency
    end
    call('setshader', 'colr', 'shaders:'..shader )
    call( 'setfloat', 'MatTranslucency', distTrans )
    local oldMeshUsage = call( 'getmeshusage' )
    call( 'setmeshusage', oldMeshUsage..'NeedsVertexShader' )
end
