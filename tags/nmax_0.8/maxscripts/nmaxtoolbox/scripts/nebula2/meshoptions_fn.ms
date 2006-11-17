-------------------------------------------------------------------------------
--  meshoptions_fn.ms
--
--  (c)2005 Kim, Hyoun Woo
-------------------------------------------------------------------------------

-------------------------------------------------------------------------------
--  Add 'mesh option' custom attribute of selected object to modifier panel.
--  If the selected mesh alread has the custom attribute, it asks to remove
--  exist custom attributes.
--
--  'Custom Attributes' are defined in the 'meshoptions_ca.ms' file
-------------------------------------------------------------------------------
fn AddMeshOptionsCustomAttributes = 
(
    include "nebula2/custattrib.ms"
    include "nebula2/meshoptions_ca.ms"

    for obj in selection do
    (
        -- include "nebula2/meshoptions_ca.ms"

        AddCustomAttributes obj nMeshOptionsCA
    )
)

