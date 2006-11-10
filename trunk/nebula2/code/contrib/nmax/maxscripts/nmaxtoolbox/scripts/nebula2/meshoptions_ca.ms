-------------------------------------------------------------------------------
--  meshoptions_ca.ms
--
--  (c)2005 Kim, Hyoun Woo
-------------------------------------------------------------------------------

-------------------------------------------------------------------------------
--  custom attributes for mesh options.
--  executed in 'Mesh Options' menu item of 'Nebula2' menu
--  'Custom Attributes' are shown in modifier panel
-- 
--  10-Nov-06  kims  Added stuff for sky system.
-------------------------------------------------------------------------------
nMeshOptionsCA = attributes "nMeshOptions"
(
    -- parameters for mesh types
    parameters MeshTypes rollout:rOptions
    (
        -- default value of a radiobutton is '1'
        meshtype type:#integer default:1 animatable:true ui:rdoMeshTypes
        --attachednode type:#boolean default:false animatable:true ui:chkAttachedNode
    )

    -- parameters for vertex options
    parameters VertexOptions rollout:rOptions
    (
        normal  type:#boolean default:true animatable:true ui:chkNormal
        color   type:#boolean default:true animatable:true ui:chkColor
        uvs     type:#boolean default:true animatable:true ui:chkUvs
        tangent type:#boolean default:false animatable:true ui:chkTangent
    )

    parameters SkyLinkedElem rollout:rOptions
    (
        linkedElem type:#string default:"" animatable:true ui:edtLinkedElem 
    )

    parameters MeshDirSetting rollout:rOptions
    (
        meshDir type:#string default:"" animatable:true ui:edtMeshDirSetting
        gfxDir  type:#string default:"" animatable:true ui:edtGfxDirSetting
    )
    
    rollout rOptions "Nebula2 Mesh Options"
    (
        -- mesh types
        group "Mesh Types"
        (
        radiobuttons rdoMeshTypes "" align:#left labels:#("Normal", "Swing", "Shadow", "Collision", "Sky") columns:1
        )
        
        --checkbox chkAttachedNode "Attached Node" align:#left
        
        group "Vertex Options"
        (
        -- vertex options
        --label lblVertexOption "Vertex Options" align:#left
        
        checkbox chkNormal "Normal" align:#left
        checkbox chkColor "Color" align:#left
        checkbox chkUvs "Uvs" align:#left
        checkbox chkTangent "Tangent" align:#left
        )

        --filter out all objects except objects its superclass is GeometryClass:
        fn pickerFilter obj = superClassOf obj == GeometryClass 

        group "SkyNode Link Element"
        (
        edittext edtLinkedElem "Picked Elem: " align:#left
        pickbutton linkElemPicker "Pick Sky Element"  filter:pickerFilter
        )
        on linkElemPicker picked obj do
        (
            if obj != undefined do
            (
                obj.wirecolor = red
                --linkElemPicker.text = obj.name
                linkedElem = obj.name
                edtLinkedElem.text = obj.name
            )
        )

        group "Mesh Path"
        (
        -- destination directory setting
        label lblMeshDirSetting "Dest Directory for a Mesh : " align:#left
        edittext edtMeshDirSetting "" align:#left fieldWidth:100 across:2 
        button btnMeshDirSetting  "<<" align:#right width:20 height:18

        label lblGfxDirSetting "Dest Directory for a Gfx : " align:#left
        edittext edtGfxDirSetting "" align:#left fieldWidth:100 across:2 
        button btnGfxDirSetting  "<<" align:#right width:20 height:18
        )

        on btnMeshDirSetting pressed do
        (
            meshPath = getSavePath caption:"Select a directory where the mesh to be exported."
            if meshPath != undefined then 
            (
                edtMeshDirSetting.text = meshPath
            )
        ) 

        on btnGfxDirSetting pressed do
        (
            gfxPath = getSavePath caption:"Select a directory where the .n2 gfx file to be exported."
            if gfxPath != undefined then 
            (
                edtGfxDirSetting.text = gfxPath
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

                linkElemPicker.enabled = false
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

                linkElemPicker.enabled = false
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

                linkElemPicker.enabled = false
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

                linkElemPicker.enabled = false
            )
            -- 'sky' mesh type
            if state == 5 do
            (
                linkElemPicker.enabled = true
            )
        )         
    )    
)
