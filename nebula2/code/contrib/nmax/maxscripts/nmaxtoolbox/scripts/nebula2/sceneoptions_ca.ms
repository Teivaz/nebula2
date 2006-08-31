-------------------------------------------------------------------------------
--  sceneoptions_ca.ms
--
--  (c)2006 Kim, Hyoun Woo
-------------------------------------------------------------------------------

-------------------------------------------------------------------------------
-- custom attribute for scene options.
-------------------------------------------------------------------------------
nSceneOptionsCA = attributes "nSceneOptions"
(
    parameters SceneDirSetting rollout:rOptions 
    (
        gfxDir  type:#string animatable:false 
    )
   
    rollout rOptions "Nebula2 Scene Options"
    (
    )
)

-------------------------------------------------------------------------------
--  utility panel rollout.
-------------------------------------------------------------------------------
utility nUtilityOption "Nebula2 Scene Options" rolledup:true
(
    label lblGfxDirSetting "Dest Directory for a Gfx : " align:#left
    edittext edtGfxDirSetting "" align:#left fieldWidth:100 across:2 readonly:true 
    button btnGfxDirSetting  "<<" align:#right width:20 height:18
    on btnGfxDirSetting pressed do
    (
        gfxPath = getSavePath caption:"Select a directory where the .n2 gfx file to be exported."
        if gfxPath != undefined then 
        (
            edtGfxDirSetting.text = gfxPath
            rootnode.gfxdir = gfxPath
        )
    )
		
    on nUtilityOption open do
    (
        if (custAttributes.count rootnode > 0 )do
	(
	    if (rootnode.gfxDir != undefined) then
                edtGfxDirSetting.text = rootnode.gfxDir
	)
    )
)

