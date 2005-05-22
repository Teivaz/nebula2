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
    exportFileName = maxFileName
    exportFileName = getFilenameFile exportFileName
    exportFileName += ".n2"

    nExportScene exportFileName -1
)

