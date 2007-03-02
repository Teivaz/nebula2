---------------------------------------------------------------------------------
--  mangalore.ms
--
--  (c)2007 Kim Hyoun Woo
---------------------------------------------------------------------------------

include "mangalore/menu.ms"

-- load custom modifiers for physics
fileIn "mangalore/physxbody.ms"
fileIn "mangalore/physxhelper.ms"

-- load exporter for physics
fileIn "mangalore/physxexport.ms"

---------------------------------------------------------------------------------
-- startup function
---------------------------------------------------------------------------------
fn startUp = 
(
    -- Load 'Mangalore' menu
    mlLoadMenu()

)

startUp()

---------------------------------------------------------------------------------
-- EOF
---------------------------------------------------------------------------------
