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
    )

    -- parameters for vertex options
    parameters VertexOptions rollout:rOptions
    (
        normal  type:#boolean default:true animatable:false ui:chkNormal
        color   type:#boolean default:true animatable:false ui:chkColor
        uvs     type:#boolean default:true animatable:false ui:chkUvs
        tangent type:#boolean default:false animatable:false ui:chkTangent
    )
    
    rollout rOptions "Nebula2 Mesh Options"
    (
        -- mesh types
        radiobuttons rdoMeshTypes "Mesh Types" align:#left labels:#("Normal", "Swing", "Shadow", "Collision") columns:1
        
        -- vertex options
        label lblVertexOption "Vertex Options" align:#left
        
        checkbox chkNormal "Normal" align:#left
        checkbox chkColor "Color" align:#left
        checkbox chkUvs "Uvs" align:#left
        checkbox chkTangent "Tangent" align:#left
        
        on rdoMeshTypes changed state do
        (
            -- 'normal' mesh type
            if state == 1 do
            (
                chkColor.checked = true
                chkUvs.checked   = true 
                                
                -- enables ui controls
                
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
                
                chkColor.enabled = true
                chkUvs.enabled = true 
                chkTangent.enabled = true 
            )
            -- 'shadow' mesh type
            if state == 3 do
            (
                chkColor.checked = false
                chkUvs.checked   = false 
                
                -- disables ui controls
                chkColor.enabled = false
                chkUvs.enabled = false 
                chkTangent.enabled = false
            )
            -- 'collision' mesh type
            if state == 4 do
            (
                chkColor.checked = false
                chkUvs.checked   = false 
                                
                -- disables ui controls
                
                chkColor.enabled = false
                chkUvs.enabled = false
                chkTangent.enabled = false
            )
        )         
    )    
)

