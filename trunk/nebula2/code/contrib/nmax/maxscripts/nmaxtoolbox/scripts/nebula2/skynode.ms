-------------------------------------------------------------------------------
--  skynode.ms
--
--  (c)2006 Kim, Hyoun Woo
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
--  custom attributes for nSkyNode. 
-------------------------------------------------------------------------------
caSkyNode = attributes "nSkyNode"
(
    parameters SkyNodeParam rollout:rSkyNode
    (
        lockviewer type:#boolean default:true ui:chklockviewer
        timefactor type:#float default:0.0 ui:spntimefactor
        timeperiod type:#float default:86400.000000 ui:spntimeperiod
        starttime  type:#float default:0.0  ui:spnstarttime

        --linkFrom type:#stringTab tabSizeVariable:true 
        --linkTo   type:#stringTab tabSizeVariable:true 
    )

    rollout rSkyNode "Nebula2 Sky Node"
    (
        checkbox chklockviewer "lock viewer" checked:true
        spinner spntimefactor "time factor" type:#float range:[-1000000, 1000000, 0] scale:0.000001
        spinner spntimeperiod "time period" type:#float range:[-1000000, 1000000, 86400.000000] scale:0.1
        spinner spnstarttime "start time" type:#float range:[-1000000, 1000000, 0] scale:0.1
    )
)

-------------------------------------------------------------------------------
--  scripted plugin for nSkyNode. 
-------------------------------------------------------------------------------
plugin Helper nSkyNodeHelperDummy
    name:"nSkyNode" 
    classID:#(0x9f66aee, 0x1b2a0086) 
    category:"Standard" 
    extends:dummy 
( 
    local lastSize, meshObj

    --local linkFrom = #()
    --local linkTo = #()

    -- parameters for nSkyNode
    parameters SkyNodeDummyParam rollout:rSky
    (
        size type:#float animatable:true ui:amount default:20.0
    )

    -- rollout panel for nSkyNode
    rollout rSky "Nebula2 Helper"
    (
        spinner amount "Size:" range:[0, 1e9, 20]

        on rSky open do
        (
            custAttributes.add this caSkyNode
        )
    )

    on getDisplayMesh do 
    (
        if (meshObj == undefined) do 
        (
            meshObj = createInstance box length:size \
                width:size height:size mapCoords:false
            lastSize = size
        )
        if size != lastSize do 
        (
            meshObj.length = meshObj.width = meshObj.height = size 
            lastSize = size
        )
        meshObj.mesh
    )

    tool create 
    ( 
        -- called when we click viewport to create a skynode dummy.
        on mousePoint click do 
        (
            nodeTM.translation = gridPoint;#stop 
        )
    )
)

