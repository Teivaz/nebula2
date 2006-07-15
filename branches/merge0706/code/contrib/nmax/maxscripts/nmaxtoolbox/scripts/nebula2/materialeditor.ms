---------------------------------------------------------------------------------
--  materialeditor.ms
--
--  (C)2004 Kim, Hyoun Woo
---------------------------------------------------------------------------------

---------------------------------------------------------------------------------
-- Open material editor with Nebula2 custom material.
---------------------------------------------------------------------------------
fn nMaterialEditor = 
(
    -- check that the material editor is opened.
    if MatEditor.isOpen() == false do
    (
        -- no material editor, so we open
        MatEditor.Open()

        /*
        -- get the current specified material.
        curMaterial = medit.GetCurMtl()
        if curMaterial != undefined do
        (
            matType = isKindOf curMaterial Nebula2Material
            if  matType == false do
            (
                -- no nebula custom material, so create it
                -- nSetCustomMaterial "Standard"
                
                -- get the index of the active material slot.
                mtlSlot = medit.GetActiveMtlSlot()
                
                -- assgin the 'Standard' custom material to slot '1'
                meditMaterials[mtlSlot] = Nebula2Material()
                local n2Mat = Nebula2Material()
                if n2Mat != undefined do
                    meditMaterials[mtlSlot] = n2Mat --Nebula2Mat ()
            )
                
            -- no material editor, so we open
            MatEditor.Open()
        )            
        */
    )
)
