-------------------------------------------------------------------------------
--  sceneoptions_fn.ms
--
--  (c)2006 Kim, Hyoun Woo
-------------------------------------------------------------------------------

include "nebula2/custattrib.ms"
include "nebula2/sceneoptions_ca.ms"

-------------------------------------------------------------------------------
-- show scene options rollout to the utility panel.
-------------------------------------------------------------------------------
fn AddSceneOptionsCustomAttributes = 
(
	removerollout nUtilityOption
        result = AddCustomAttributes rootnode nSceneOptionsCA
        if (result == true) then
            addrollout nUtilityOption
)

-------------------------------------------------------------------------------
--  remove scene option rollout
-------------------------------------------------------------------------------
fn nRemoveSceneOptionPanel =
(
    removerollout nUtilityOption
)

-------------------------------------------------------------------------------
--  register callbacks
-- 
--  Call nRemoveSceneOptionPanel function when a scene is opened or newly created.
--
--  Whenever a user open max file or makes a new scene, the value of previous scene  
--  option was not correctly updated.
--  Even the scene option is opened there is no custom attribute for it so it made 
--  an error. The following callbacks solve the above problems.
--  
--  -09-Mar-07  Cho Jun Heung
-------------------------------------------------------------------------------
global gRemoveSceneOptionFunc = nRemoveSceneOptionPanel

callbacks.addscript #systemPostNew "gRemoveSceneOptionFunc()" id:#nSceneOption
callbacks.addscript #filePreOpen "gRemoveSceneOptionFunc()" id:#nSceneOption 

