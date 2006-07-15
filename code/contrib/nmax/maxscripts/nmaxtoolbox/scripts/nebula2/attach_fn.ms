-------------------------------------------------------------------------------
--  attach_fn.ms
--
--  (C)2005 Kim, Hyoun Woo
-------------------------------------------------------------------------------

fn nAddAttachCustomAttributes =
(
    include "nebula2/custattrib.ms"

    for obj in helpers do
    (
        if obj.isSelected do
        (
            if classof obj == Dummy do
            (
                include "nebula2/attach_ca.ms"

                AddCustomAttributes obj nAttachCA
            )
        )
    )
)
