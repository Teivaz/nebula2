-------------------------------------------------------------------------------
--  lod_ca.ms
--
--  (c)2005 Kim, Hyoun Woo
-------------------------------------------------------------------------------

-------------------------------------------------------------------------------
--  custom attributes for LOD node.
-- 
--  The specified custom attributes are shown in the modifier panel.
-------------------------------------------------------------------------------
nLODCA = attributes "nLODNode"
(
    -- LOD custom attributes
    parameters LodParam rollout:rLOD
    (
        threshold type:#float default:100.0 align:#left ui:spnTreshold 
        mindistance type:#float default:-100.0 align:#left ui:spnMinDistance
        maxdistance type:#float default:100.0 align:#left ui:spnMaxDistance
    )

    -- rollout panel for LOD node
    rollout rLOD "Nebula2 LOD Node"
    (
        spinner spnTreshold "Threshold"  type:#float range:[-1000000, 1000000, -100] scale:1.0 
        spinner spnMinDistance "Min Distance" type:#float range:[-1000000, 1000000, -100] scale:1.0 
        spinner spnMaxDistance "Max Distance" type:#float range:[-1000000, 1000000, -100] scale:1.0
    )
)
