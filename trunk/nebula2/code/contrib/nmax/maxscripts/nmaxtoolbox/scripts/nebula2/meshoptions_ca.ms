-------------------------------------------------------------------------------
--  meshoptions_ca.ms
--
--  (c)2005 Kim, Hyoun Woo
-------------------------------------------------------------------------------

-------------------------------------------------------------------------------
--  custom attributes for mesh options.
--  executed in 'Mesh Options' menu item of 'Nebula2' menu
--  'Custom Attributes' are shown in modifier panel
-------------------------------------------------------------------------------
nMeshOptionsCA = attributes "nMeshOptions"
(
    -- parameters for mesh types
    parameters MeshTypes rollout:rOptions
    (
        -- default value of a radiobutton is '1'
        meshtype type:#integer default:1 animatable:false ui:rdoMeshTypes
        --attachednode type:#boolean default:false animatable:false ui:chkAttachedNode
    )

    -- parameters for vertex options
    parameters VertexOptions rollout:rOptions
    (
        normal  type:#boolean default:true animatable:false ui:chkNormal
        color   type:#boolean default:true animatable:false ui:chkColor
        uvs     type:#boolean default:true animatable:false ui:chkUvs
        tangent type:#boolean default:false animatable:false ui:chkTangent
    )

    parameters MeshDirSetting rollout:rOptions
    (
        meshDir type:#string ui:edtDirSetting
    )
    
    rollout rOptions "Nebula2 Mesh Options"
    (
        -- mesh types
        radiobuttons rdoMeshTypes "Mesh Types" align:#left labels:#("Normal", "Swing", "Shadow", "Collision") columns:1
        
        --checkbox chkAttachedNode "Attached Node" align:#left
        
        -- vertex options
        label lblVertexOption "Vertex Options" align:#left
        
        checkbox chkNormal "Normal" align:#left
        checkbox chkColor "Color" align:#left
        checkbox chkUvs "Uvs" align:#left
        checkbox chkTangent "Tangent" align:#left

        -- destination directory setting
        label lblDirSetting "Destination Directory : " align:#left
        edittext edtDirSetting "" align:#left fieldWidth:100 across:2 readonly:true 
        button btnDirSetting  "<<" align:#right width:20 height:18
        on btnDirSetting pressed do
        (
            mapPath = getSavePath caption:"Select path where the mesh to be exported."
            if mapPath != undefined then 
            (
                edtDirSetting.text = mapPath
            )
        ) 
        
        on rdoMeshTypes changed state do
        (
            -- 'normal' mesh type
            if state == 1 do
            (
                chkColor.checked = true
                chkUvs.checked   = true 
                                
                -- enables ui controls
                --chkAttachedNode.enabled = true
                
                chkColor.enabled = true
                chkUvs.enabled = true 
                chkTangent.enabled = true 
            )
            -- 'swing' mesh type
            if state == 2 do
            (
                chkColor.checked = true
                chkUvs.checked   = true 
                                
                -- enables ui controls
                --chkAttachedNode.enabled = false
                
                chkColor.enabled = true
                chkUvs.enabled = true 
                chkTangent.enabled = true 
            )
            -- 'shadow' mesh type
            if state == 3 do
            (
                chkNormal.checked  = false
                chkColor.checked   = false
                chkUvs.checked     = false 
                chkTangent.checked = false
                
                -- disables ui controls
                chkNormal.enabled  = false
                chkColor.enabled   = false
                chkUvs.enabled     = false 
                chkTangent.enabled = false
            )
            -- 'collision' mesh type
            if state == 4 do
            (
                chkColor.checked = false
                chkUvs.checked   = false 
                                
                -- disables ui controls
                --chkAttachedNode.enabled = false
                
                chkColor.enabled = false
                chkUvs.enabled = false
                chkTangent.enabled = false
            )
        )         
    )    
)
