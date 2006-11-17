---------------------------------------------------------------------------------
--  export.ms
--
--  (C)2004 Kim, Hyoun Woo
---------------------------------------------------------------------------------

---------------------------------------------------------------------------------
--  Export the scene
--  
--  It export the scene with the scene file name.
--    e.g. 'foo.max' is exported with the name 'foo.n2'
--
--  If the scene is aleady saved it just export and overwrite any exist file 
--  otherwise display save file
---------------------------------------------------------------------------------
fn nExport = 
(
    include "nebula2/utilz.ms"

    -- see if the scene has multiple objects which has same name.
    nCheckObjectName()

    -- check the 3ds max system "dirty" flag.
    -- 'ture' if the scene needs to save.
    needSave = getSaveRequired()
        
    if needSave == true then
    (
        -- displays standard 3ds max file save dialog.
        filename = getMAXSaveFileName()
        if filename != undefined do
        (
            -- save the scene with the given filename
            -- and specify system 'dirty' flag to false
            saveMaxFile filename clearNeedSaveFlag:true
                
            -- now, try to export it.
            exportFileName = getFilenameFile filename
            -- specify the extension.
            exportFileName += ".n2"

            exportFile exportFileName
        )
    )
    else
    (
        -- no need to save the scene (it is aleady saved)
        -- so, we export it with its saved filename.
        exportFileName = maxFileName
        exportFileName = getFilenameFile exportFileName
        exportFileName += ".n2"

        exportFile exportFileName
    )
)

