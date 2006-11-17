-------------------------------------------------------------------------------
--  sceneoptions_fn.ms
--
--  (c)2006 Kim, Hyoun Woo
-------------------------------------------------------------------------------

-------------------------------------------------------------------------------
-- show scene options rollout to the utility panel.
-------------------------------------------------------------------------------
fn AddSceneOptionsCustomAttributes = 
(
    include "nebula2/custattrib.ms"
    include "nebula2/sceneoptions_ca.ms"

    for obj in selection do
    (
        AddCustomAttributes rootnode nSceneOptionsCA
        addrollout nUtilityOption
    )
)

