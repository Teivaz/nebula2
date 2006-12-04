---------------------------------------------------------------------------------
--  preview.ms
--
--  (C)2005 Kim, Hyoun Woo
---------------------------------------------------------------------------------

---------------------------------------------------------------------------------
--  Preview the scene
--  
--  It export the scene with the scene file name and exported scene is saved as 
--  as ram file.
--    e.g. 'foo.max' is exported with the name 'foo.n2'
--
--  note: exported file does not saved on the disk in preview mode.
---------------------------------------------------------------------------------
fn nPreview = 
(
    needSave = getSaveRequired()

    exportFileName = ""

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

            nExportScene exportFileName -1
         )
    )
    else
    (
        exportFileName = maxFileName
        exportFileName = getFilenameFile exportFileName

        exportFileName += ".n2"
        nExportScene exportFileName -1
    )
)

