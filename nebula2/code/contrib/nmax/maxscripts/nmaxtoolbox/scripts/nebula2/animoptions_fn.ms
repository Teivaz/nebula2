-------------------------------------------------------------------------------
--  animoptions_fn.ms
--
--  (c)2006 Kim, Hyoun Woo
-------------------------------------------------------------------------------

-------------------------------------------------------------------------------
--  Add 'anim option' custom attribute of selected object to modifier panel.
--  If the selected mesh alread has the custom attribute, it asks to remove
--  exist custom attributes.
--
--  An animation 'Custom Attributes' are defined in the 'animoptions_ca.ms' file
-------------------------------------------------------------------------------
fn AddAnimOptionsCustomAttributes = 
(
    include "nebula2/custattrib.ms"
    include "nebula2/animoptions_ca.ms"

    for obj in selection do
    (
        -- TODO: check the object to ensure it is dummy which is used for a bone.
        AddCustomAttributes obj nAnimOptionsCA
    )
)

