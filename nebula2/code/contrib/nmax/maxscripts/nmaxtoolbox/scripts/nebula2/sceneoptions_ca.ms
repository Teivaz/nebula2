-------------------------------------------------------------------------------
--  sceneoptions_ca.ms
--
--  (c)2006 Kim, Hyoun Woo
-------------------------------------------------------------------------------

-------------------------------------------------------------------------------
-- custom attribute for scene options.
-- 
-- Currently the scene option custom attribute only has 'gfxDir' parameter which 
-- where nebula2 gfx objects are to be put. 
-- 
-- Any custom attributes which can be used for whole scene options are placed here.
-------------------------------------------------------------------------------
nSceneOptionsCA = attributes "nSceneOptions"
(
    parameters SceneDirSetting
    (
        -- scene options parameter does not have default value
        -- because that there is no way to make it animatable with specified
        -- ui control even it is not possible to connect the parameter to ui.
        gfxDir  type:#string default:"" animatable:false
        useSameDir type:#boolean default:true
    )
)

-------------------------------------------------------------------------------
--  utility panel rollout.
-------------------------------------------------------------------------------
utility nUtilityOption "Nebula2 Scene Options" rolledup:false
(
    label lblGfxDirSetting "Dest Directory for a Gfx : " align:#left
    edittext edtGfxDirSetting "" align:#left fieldWidth:100 across:2 
    button btnGfxDirSetting  "<<" align:#right width:20 height:18
    checkbox chkUseSameDirSetting "Use same dir" checked:true align:#left 
    
    on btnGfxDirSetting pressed do
    (
        gfxPath = getSavePath caption:"Select a directory where the .n2 gfx file to be exported."
        if gfxPath != undefined then 
        (
            edtGfxDirSetting.text = gfxPath
            rootnode.gfxdir = gfxPath
        )
    )
    on edtGfxDirSetting changed newstr do
    (
        edtGfxDirSetting.text = newstr
        rootnode.gfxdir = newstr
    )

    on chkUseSameDirSetting changed value do
    (
        if isProperty rootnode "useSameDir" then
            rootnode.useSameDir = value
    )
        
    on nUtilityOption open do
    (
        if (custAttributes.count rootnode > 0 )do
        (
            if (rootnode.gfxDir != undefined) then
            (
                edtGfxDirSetting.text = rootnode.gfxDir
                chkUseSameDirSetting.checked = rootnode.useSameDir
            )
        )
    )
)





