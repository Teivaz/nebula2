---------------------------------------------------------------------------------
--  nmaxtoolbox.ms
--
--  startup script for Nebula2 3DS Max toolkit.
--
--  note:
--      Locate this file under '$3dsmax/scripts/startup' directory.
--
--  (c)2004 Kim, Hyoun Woo
---------------------------------------------------------------------------------

include "nebula2/menu.ms"



---------------------------------------------------------------------------------
-- startup function
---------------------------------------------------------------------------------
fn startUp = 
(
    -- Load 'Nebula2' menu
    nLoadMenu()

    -- create scripted plugins
    fileIn "nebula2/lodnode.ms"
    fileIn "nebula2/skynode.ms"
    fileIn "nebula2/skystate.ms"
    
    -- Generate Nebula2 custom material script pugin script and evaluate. 
    nCreateCustomMaterialPlugin()
)

startUp()
