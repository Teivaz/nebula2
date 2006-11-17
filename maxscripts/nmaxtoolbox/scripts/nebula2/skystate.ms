-------------------------------------------------------------------------------
--  skystate.ms
--
--  (c)2006 Kim, Hyoun Woo
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
--  custom attributes for nSkyState.
-------------------------------------------------------------------------------
caSkyState = attributes "nSkyState"
(
    parameters SkyStateParam rollout:rSkyState
    (
        skyElement type:#string default:"" animatable:true ui:edtSkyElement 
        stateTime type:#float animatable:true ui:spnStateTime default:0.0
    )

    rollout rSkyState "Nebula2 Sky State"
    (
        --filter out all objects except objects its superclass is GeometryClass:
        fn pickerFilter obj = superClassOf obj == GeometryClass 

        edittext edtSkyElement "Picked Elem: " align:#left
        pickbutton elementPicker "Pick Sky Element" filter:pickerFilter

        on elementPicker picked obj do
        (
            if obj != undefined do
            (
                obj.wirecolor = red
                elementPicker.text = obj.name
                skyElement = obj.name
                -- edtSkyElement.text = obj.name
            )
        )

        spinner spnStateTime "State Time:" type:#float range:[-1000000.000000, 1000000.000000, 0.0] scale:0.1
    )
)

-------------------------------------------------------------------------------
--  scripted plugin for nSkyState.
-------------------------------------------------------------------------------
plugin Helper nSkyStateHelperDummy
    name:"nSkyState" 
    classID:#(0x5cc47cdc, 0x74965915) 
    category:"Standard" 
    extends:dummy 
( 
    local lastSize, meshObj

    -- parameters for nSkyState
    parameters SkyStateParam rollout:rSky
    (
        size type:#float animatable:true ui:amount default:20.0
    )

    -- rollout panel for nSkyState
    rollout rSky "Nebula2 Helper"
    (
        spinner amount "Size:" range:[0, 1e9, 20]

        on rSky open do
        (
            custAttributes.add this caSkyState
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
        -- called when we click viewport to create a skystate dummy
        on mousePoint click do 
        (
            nodeTM.translation = gridPoint;#stop 
        )
    ) 
)

