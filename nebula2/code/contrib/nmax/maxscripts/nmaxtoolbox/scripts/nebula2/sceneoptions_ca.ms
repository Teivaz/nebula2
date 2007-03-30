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
        include "nebula2/utilz.ms"

        projDir = nGetProjDir();
        gfxAssign = nebula2.call "/sys/servers/file2.manglepath" "gfxlib:"
        gfxAssign = nReplaceSlash gfxAssign[1]

        gfxPath = getSavePath initialDir:gfxlibAssign caption:"Select a directory where the .n2 gfx file to be exported."
        if gfxPath != undefined then 
        (
            gfxPath = nReplaceSlash gfxPath
            findIndex = findString gfxPath gfxAssign
            if findIndex != undefined  then 
            (
                subDir = substring gfxPath (gfxAssign.count + 2) gfxPath.count
                exportDir = "gfxlib:" + subDir
                edtGfxDirSetting.text = exportDir
                rootnode.gfxdir = exportDir
            )
            else
            (
                format "(%) is not subdirectory of (%)\n" gfxPath gfxAssign
            )
        )
    )

    on edtGfxDirSetting entered newstr do
    (
        include "nebula2/utilz.ms"

        projDir = nGetProjDir();
        -- NOTE: nebula2.call returns array type
        gfxAssign = nebula2.call "/sys/servers/file2.manglepath" "gfxlib:"
        gfxAssign = nReplaceSlash gfxAssign[1]

        gfxPath = nebula2.call "/sys/servers/file2.manglepath" newstr
        gfxPath = gfxPath[1]
        findIndex = findString gfxPath gfxAssign
        if findIndex != undefined  then 
        (
            subDir = substring gfxPath (gfxAssign.count + 2) gfxPath.count
            exportDir = "gfxlib:" + subDir
            edtGfxDirSetting.text = exportDir
            rootnode.gfxdir = exportDir
        )
        else
        (
            format "(%) is not subdirectory of (%)\n" gfxPath gfxAssign
            edtGfxDirSetting.text = ""
            rootnode.gfxdir = ""
        )
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





