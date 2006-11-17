-------------------------------------------------------------------------------
--  lod_fn.ms
--
--  (c)2005 Kim, Hyouon Woo
-------------------------------------------------------------------------------

-------------------------------------------------------------------------------
--  Add Nebula2 LOD custom attributes to selected node
--  Be sure that the selected node is 'dummy' helper object
-------------------------------------------------------------------------------
fn nAddLODCustomAttributes = 
(
    include "nebula2/custattrib.ms"

    -- for each selected 3dsmasx helper objects
    for obj in helpers do
    (
        if obj.isSelected do
        (
            if classof obj == Dummy do
            (
                include "nebula2/lod_ca.ms"

                AddCustomAttributes obj nLODCA
            )
        )
    )
)

