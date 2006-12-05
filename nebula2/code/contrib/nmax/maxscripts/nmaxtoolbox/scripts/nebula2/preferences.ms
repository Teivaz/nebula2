-------------------------------------------------------------------------------
--  preferences.ms 
--
--  (c)2006  Kim Hyoun Woo
-------------------------------------------------------------------------------

-------------------------------------------------------------------------------
-- viewer options rollout in preferences
--
-------------------------------------------------------------------------------
rollout rPrefViewerOptions "Viewer Options" width:319 height:339
(
    GroupBox grp1 "nviewer Options" pos:[10,19] width:300 height:147
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
        nmaxtoolboxIniFile = getDir #plugcfg
        nmaxtoolboxIniFile += "\\"
        nmaxtoolboxIniFile += "nmaxtoolbox.ini" 

        fullscreen   = getINISetting nmaxtoolboxIniFile "DefaultViewerSetting" "-fullscreen"
        alwaysOnTop  = getINISetting nmaxtoolboxIniFile "DefaultViewerSetting" "-alwaysontop"
        viewerXPos   = getINISetting nmaxtoolboxIniFile "DefaultViewerSetting" "-x"
        viewerYPos   = getINISetting nmaxtoolboxIniFile "DefaultViewerSetting" "-y"
        viewerWidth  = getINISetting nmaxtoolboxIniFile "DefaultViewerSetting" "-w"
        viewerHeight = getINISetting nmaxtoolboxIniFile "DefaultViewerSetting" "-h"

        if fullscreen == "true" then
            chkFullScreen.checked = true
        else
            chkFullScreen.checked = false

        if alwaysOnTop == "true" then
            chkAlwaysOnTop.checked = true
        else
            chkAlwaysOnTop.checked = false

        edtXPos.text = viewerXPos
        edtYPos.text = viewerYPos
        edtWidth.text = viewerWidth
        edtHeight.text = viewerHeight

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

        -- get $3dsmax/plugcfg/ directory
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

-- veiwerOptionsFloater = newRolloutFloater "Preferences" 335 380
-- addRollout rPrefViewerOptions veiwerOptionsFloater

