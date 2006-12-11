-------------------------------------------------------------------------------
--  preferences.ms 
--
--  (c)2006  Kim Hyoun Woo
-------------------------------------------------------------------------------

-------------------------------------------------------------------------------
-- get $3dsmax/plugcfg/ directory
-------------------------------------------------------------------------------
/*
fn nGetNMaxIniFilename = 
(
    -- get $3dsmax/plugcfg/ directory
    nmaxtoolboxIniFile = getDir #plugcfg
    nmaxtoolboxIniFile += "\\"
    nmaxtoolboxIniFile += "nmaxtoolbox.ini" 

    return nmaxtoolboxIniFile
)
*/

-------------------------------------------------------------------------------
-- mesh suffix rollout in preferences
-------------------------------------------------------------------------------
rollout rPrefSuffix "Mesh Suffix" width:319 height:300
(
    edittext edtStatic "static mesh suffix: " pos:[88,17] width:210 height:20
    edittext edtSkinned "skinned mesh suffix: " pos:[76,42] width:222 height:20
    edittext edtStaticShadow "static shadow mesh suffix: " pos:[47,68] width:252 height:20
    edittext edtSkinnedShadow "skinned shadow mesh suffix: " pos:[36,93] width:263 height:20    
    edittext edtSwing "swing mesh suffix: " pos:[86,119] width:213 height:20
    edittext edtCollision "collision mesh suffix: " pos:[76,145] width:223 height:20
    edittext edtParticle2 "particle2 mesh suffix: " pos:[74,172] width:225 height:20
    
    -- retrieve suffixes from INI file.
    on rPrefSuffix open do
    (
        nmaxIniFile = getDir #plugcfg
        nmaxIniFile += "\\"
        nmaxIniFile += "nmaxtoolbox.ini"

        edtStatic.text  = getINISetting nmaxIniFile "MeshSuffix" "static"
        edtSkinned.text = getINISetting nmaxIniFile "MeshSuffix" "skinned"
        edtStaticShadow.text  = getINISetting nmaxIniFile "MeshSuffix" "shadow"
        edtSkinnedShadow.text = getINISetting nmaxIniFile "MeshSuffix" "skinnedshadow"
        edtSwing.text = getINISetting nmaxIniFile "MeshSuffix" "swing"
        edtCollision.text = getINISetting nmaxIniFile "MeshSuffix" "collision"
        edtParticle2.text = getINISetting nmaxIniFile "MeshSuffix" "particle2"
    )

    -- save suffixes to INI file.
    on rPrefSuffix close do
    (
        nmaxIniFile = getDir #plugcfg
        nmaxIniFile += "\\"
        nmaxIniFile += "nmaxtoolbox.ini"

        setINISetting nmaxIniFile "MeshSuffix" "static" edtStatic.text
        setINISetting nmaxIniFile "MeshSuffix" "skinned" edtSkinned.text
        shadowSuffix  = setINISetting nmaxIniFile "MeshSuffix" "shadow" edtStaticShadow.text
        skinnedShadowSuffix = setINISetting nmaxIniFile "MeshSuffix" "skinnedshadow" edtSkinnedShadow.text
        swingSuffix = setINISetting nmaxIniFile "MeshSuffix" "swing" edtSwing.text
        collisionSuffix = setINISetting nmaxIniFile "MeshSuffix" "collision" edtCollision.text
        particle2Suffix = setINISetting nmaxIniFile "MeshSuffix" "particle2" edtParticle2.text
    )
)

-------------------------------------------------------------------------------
-- viewer options rollout in preferences
-------------------------------------------------------------------------------
rollout rPrefViewerOptions "Viewer Options" width:319 height:339
(
    GroupBox grp1 "Default Viewer Options" pos:[10,19] width:300 height:147
    checkbox chk1 "Checkbox" pos:[30,62] width:0 height:0
    checkbox chkFullScreen "Preview in Fullscreen Mode" pos:[24,44] width:159 height:20
    checkbox chkAlwaysOnTop "Always on Top" pos:[24,67] width:167 height:20
    label lblWinPos "Window Position: " pos:[60,94] width:123 height:20
    edittext edtXPos "" pos:[152,93] width:70 height:20
    edittext edtYPos "" pos:[230,93] width:70 height:20
    edittext edtWidth "" pos:[152,122] width:70 height:20
    edittext edtHeight "" pos:[230,122] width:70 height:20
    label lblWinSize "Window Size: " pos:[75,125] width:75 height:19
    GroupBox grp4 "Custom Viewer Options" pos:[10,176] width:300 height:152
    label lbl7 "Specify the location of the custom viewer:" pos:[19,198] width:279 height:20
    edittext edtCustomViewerExe "" pos:[18,220] width:209 height:20
    button btnCustomViewerFileFinder "<<" pos:[240,221] width:59 height:20
    edittext edtCustomViewerOptions "" pos:[18,254] width:279 height:62
    
    -- rollout open event handler
    -- retrieves key values from the "nmaxtoolbox.ini" file
    on rPrefViewerOptions open do
    (
        -- get $3dsmax/plugcfg/ directory
        --nmaxtoolboxIniFile = nGetNMaxIniFilename() 

        nmaxtoolboxIniFile = getDir #plugcfg
        nmaxtoolboxIniFile += "\\"
        nmaxtoolboxIniFile += "nmaxtoolbox.ini" 

        fullscreen   = getINISetting nmaxtoolboxIniFile "DefaultViewerSetting" "-fullscreen"
        alwaysOnTop  = getINISetting nmaxtoolboxIniFile "DefaultViewerSetting" "-alwaysontop"
        edtXPos.text = getINISetting nmaxtoolboxIniFile "DefaultViewerSetting" "-x"
        edtYPos.text = getINISetting nmaxtoolboxIniFile "DefaultViewerSetting" "-y"
        edtWidth.text = getINISetting nmaxtoolboxIniFile "DefaultViewerSetting" "-w"
        edtHeight.text = getINISetting nmaxtoolboxIniFile "DefaultViewerSetting" "-h"

        if fullscreen == "true" then
            chkFullScreen.checked = true
        else
            chkFullScreen.checked = false

        if alwaysOnTop == "true" then
            chkAlwaysOnTop.checked = true
        else
            chkAlwaysOnTop.checked = false

        -- temporary disabled untill we have methods for custom viewer options.
        edtCustomViewerExe.enabled = false
        btnCustomViewerFileFinder.enabled = false
        edtCustomViewerOptions.enabled = false
    )

    -- rollout close event handler
    -- save key values to the "nmaxtoolbox.ini" file
    on rPrefViewerOptions close do
    (
        nmaxtoolboxIniFile = ""

        --nmaxtoolboxIniFile = nGetNMaxIniFilename() 

        nmaxtoolboxIniFile = getDir #plugcfg
        nmaxtoolboxIniFile += "\\"
        nmaxtoolboxIniFile += "nmaxtoolbox.ini" 

        local fullscreen = "false"
        local alwaysontop = "false"

        if chkFullScreen.checked then
            fullscreen = "true"
        else
            fullscreen = "false"

        if chkAlwaysOnTop.checked then
            alwaysontop = "true"
        else
            alwaysontop = "false"

        setINISetting nmaxtoolboxIniFile "DefaultViewerSetting" "-fullscreen" fullscreen
        setINISetting nmaxtoolboxIniFile "DefaultViewerSetting" "-alwaysontop" alwaysontop
        setINISetting nmaxtoolboxIniFile "DefaultViewerSetting" "-x" edtXPos.text
        setINISetting nmaxtoolboxIniFile "DefaultViewerSetting" "-y" edtYPos.text
        setINISetting nmaxtoolboxIniFile "DefaultViewerSetting" "-w" edtWidth.text
        setINISetting nmaxtoolboxIniFile "DefaultViewerSetting" "-h" edtHeight.text
    )
)

-------------------------------------------------------------------------------
--  Run preferences dialog
-------------------------------------------------------------------------------

veiwerOptionsFloater = newRolloutFloater "Preferences" 335 590
addRollout rPrefSuffix veiwerOptionsFloater
addRollout rPrefViewerOptions veiwerOptionsFloater


