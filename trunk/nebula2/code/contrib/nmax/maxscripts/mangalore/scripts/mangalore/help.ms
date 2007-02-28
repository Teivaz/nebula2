---------------------------------------------------------------------------------
--  help.ms
--
--  (C)2007 Kim Hyoun Woo
---------------------------------------------------------------------------------

---------------------------------------------------------------------------------
--  Show user reference .chm document.
---------------------------------------------------------------------------------
fn mlShowHelp = 
(
    -- get 'scrips' directory in 3dsmax installed directory
    -- e.g. 'c:/3dsmax7/scripts'
    scriptDir = GetDir #scripts
    chmFilename = scriptDir + "\\mangalore\\mangaloretool.chm"

    -- show up 'nmaxtoolbox.chm' nebula help file.
    ShellLaunch chmFilename ""
)

