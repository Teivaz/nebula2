-----------------------------------------------------------------------------
--  entity.lua
--  =========
--  Set up actors.
--
-----------------------------------------------------------------------------

runscript('mesh.lua')
runscript('terrain.lua')

function createGameObjectLibrary()
	-----------------------------------------------------------------------------
	--  Create models
	-----------------------------------------------------------------------------
	selSafe('nroot', modelsPath )
		table.foreach( vegetationDefs, loadvegetationmesh )
        loadAvatarData()
	sel('..')
end

function loadvegetationmesh( index, vegDef )
    local name = vegDef.name
    local underscoreIndex = string.find( name, '_' )
    local ddsName = name
    if underscoreIndex ~= nil then
        ddsName = string.sub( name, 1, underscoreIndex - 1 ) -- e.g. tree02_1 and tree02_2 both use tree02.dds
    end
    loadmesh( name, 'Nature/'..vegDef.subDir, name..'.n3d2', ddsName..'.dds', vegDef.shaderFunc )    
    name = name..'_bill'
    ddsName = name -- billboards, unlike models, never share a texture (tree02_1_bill.dds and tree02_2_bill.dds are separate), so we can't use the ddsName above
    loadmesh( name, 'Nature/'..vegDef.subDir, name..'.n3d2', ddsName..'.dds', function() defaultInitShaderFunc(nil, forest.billboardDistTransparency ) end ) 
end
