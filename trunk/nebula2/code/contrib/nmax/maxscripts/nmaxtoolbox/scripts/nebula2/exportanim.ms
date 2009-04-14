---------------------------------------------------------------------------------
--  preview.ms
--
--  (C)2009 Kim, Hyoun Woo
---------------------------------------------------------------------------------

---------------------------------------------------------------------------------
--  Export animations
--    : It exports only animation data of the given scene.
--
---------------------------------------------------------------------------------
fn nExportAnim = 
(
    exportFileName = maxFileName
    exportFileName = getFilenameFile exportFileName

    -- File extension is only used for temparary.
    -- It will be changed when the plugin exports refered by exporter option.
    exportFileName += ".n2"

    nExportScene exportFileName 1
)
