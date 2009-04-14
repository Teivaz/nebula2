---------------------------------------------------------------------------------
--  quickexport.ms
--
--  (C)2009 Kim, Hyoun Woo
---------------------------------------------------------------------------------

---------------------------------------------------------------------------------
--  Force export the given scene.
--  It is convenient method when user want to export the scene quickly.
--------------------------------------------------------------------------------- 
fn nQuickExport =
(
    exportFileName = maxFileName
    exportFileName = getFilenameFile exportFileName
    exportFileName += ".n2"

    exportFile exportFileName
)
