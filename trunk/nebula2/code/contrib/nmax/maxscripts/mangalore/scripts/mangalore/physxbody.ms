-------------------------------------------------------------------------------
--  physxbody.ms
--
--  orinal source is yake engine octopus plugin script
--  modified by Cho, Jun Hyeong 2007
-------------------------------------------------------------------------------

plugin modifier mlODEBody
    name:"ODE Body (Mangalore)"
    category:"ODE"
    classID:#(0x5e951b97, 0x4952cbd8)
    version:1
    initialRollupState:0x01
(
/*
        parameters paramDynamic rollout:rRigidBody
        (
            isStatic type:#boolean ui:cbStatic default:false
        bodyShape type:#integer ui:ddlBodyShape
    )
*/
    -- 변수를 배열로 잡지 않은 이유는 명시적으로 코딩하기 위함입니다.
    parameters paramJoint rollout:rJoint
    (
        hasJoint type:#boolean ui:cbJoint default:false -- joint collide
        jointType type:#integer ui:ddlJoint default:1

        -- axis1
        isLoStop0 type:#boolean ui:cbLoStop0
        isHiStop0 type:#boolean ui:cbHiStop0
        loStop0 type:#float ui:spLoStop0
        hiStop0 type:#float ui:spHiStop0
        vel0 type:#float ui:spVel0
        fMax0 type:#float ui:spFMax0
        fudge0 type:#float ui:spFudge0
        bounce0 type:#float ui:spBounce0
        cfm0 type:#float ui:spCfm0
        stopErp0 type:#float ui:spStopErp0
        stopCfm0 type:#float ui:spStopCfm0

        -- axis2
        isLoStop1 type:#boolean ui:cbLoStop1
        isHiStop1 type:#boolean ui:cbHiStop1
        loStop1 type:#float ui:spLoStop1
        hiStop1 type:#float ui:spHiStop1
        vel1 type:#float ui:spVel1
        fMax1 type:#float ui:spFMax1
        fudge1 type:#float ui:spFudge1
        bounce1 type:#float ui:spBounce1
        cfm1 type:#float ui:spCfm1
        stopErp1 type:#float ui:spStopErp1
        stopCfm1 type:#float ui:spStopCfm1

        -- axis3
        isLoStop2 type:#boolean ui:cbLoStop2
        isHiStop2 type:#boolean ui:cbHiStop2
        loStop2 type:#float ui:spLoStop2
        hiStop2 type:#float ui:spHiStop2
        vel2 type:#float ui:spVel2
        fMax2 type:#float ui:spFMax2
        fudge2 type:#float ui:spFudge2
        bounce2 type:#float ui:spBounce2
        cfm2 type:#float ui:spCfm2
        stopErp2 type:#float ui:spStopErp2
        stopCfm2 type:#float ui:spStopCfm2

        -- additional params
        suspERP type:#float ui:spSuspERP
        suspCFM type:#float ui:spSuspCFM
    )

    on attachedToNode node do
    (
/*
        classTyep = (classof node.baseobject)
        if( classTyep == box ) then
        (
            bodyShape = 2
        )
        else if( classTyep == sphere or classTyep == geosphere ) then
        (
            bodyShape = 3
        )
        else if( classTyep == cylinder ) then
        (
            bodyShape = 4
        )
        else if( classTyep == cone ) then
        (
            bodyShape = 5
            node.radius2 = 0.0
        )
        else if( classTyep == plane ) then
        (
            bodyShape = 6
        )
        else -- tri-mesh
        (
            bodyShape = 1
        )
*/

        node.wirecolor = red
    )
/*
    rollout rRigidBody "Rigid Body"
    (
        checkbox cbStatic "Static (rigid body only)"
            label lbBodyShape "Body Shape:" align:#left
        dropdownlist ddlBodyShape items:#("Tri-Mesh","Box","Sphere","Cylinder","Cone","Plane") align:#left
    )
*/
    rollout rJoint "Joint"
    (
        checkbox cbJoint "This object has joint"
        dropdownlist ddlJoint items:#("None","Ball (0)","Hinge (1)","Hinge2 (2)","Slider (1)","Universal (2)","AMotor (3)","Fixed (0)") align:#left

        group "Hinge2"
        (
            spinner spSuspERP "suspERP: " fieldwidth:50 type:#float range:[0.0,1.0,0.2]
            spinner spSuspCFM "suspCFM: " fieldwidth:50 type:#float range:[0.0,9999.9,0.0]
        )
        group "Axis #1 Params"
        (
            checkbox cbLoStop0 "loStop: " checked:false across:2
            checkbox cbHiStop0 "hiStop: " checked:false
            angle angLoStop0 diameter:50 align:#left type:#float range:[-180,180,0] across:2
            angle angHiStop0 diameter:50 align:#right type:#float range:[-180,180,0]
            spinner spLoStop0 fieldwidth:40 align:#left type:#float range:[-180, 180,0] across:2
            spinner spHiStop0 fieldwidth:40 align:#right type:#float range:[-180, 180,0]
            spinner spVel0 "vel: " fieldwidth:50 type:#float range:[-9999.9,9999.9,0.0]
            spinner spFMax0  "fMax: " fieldwidth:50 type:#float range:[0.0,99999.9,0.0]
            spinner spFudge0 "fudge: " fieldwidth:50 type:#float range:[0.0,1.0,1.0]
            spinner spBounce0 "bounce: " fieldwidth:50 type:#float range:[0.0,1.0,0.0]
            spinner spCfm0 "CFM: " fieldwidth:50 type:#float range:[0.0,9999.9,0.0]
            spinner spStopErp0 "stopERP: " fieldwidth:50 type:#float range:[0.0,1.0,0.2]
            spinner spStopCfm0 "stopCFM: " fieldwidth:50 type:#float range:[0.0,9999.9,0.0]
        )
        group "Axis #2 Params"
        (
            checkbox cbLoStop1 "loStop: " checked:false across:2
            checkbox cbHiStop1 "hiStop: " checked:false
                   angle angLoStop1 diameter:50 align:#left type:#float range:[-180,180,0] across:2
            angle angHiStop1 diameter:50 align:#right type:#float range:[-180,180,0]
            spinner spLoStop1 fieldwidth:40 align:#left type:#float range:[-180, 180,0] across:2
            spinner spHiStop1 fieldwidth:40 align:#right type:#float range:[-180, 180,0]
            spinner spVel1 "vel: " fieldwidth:50 type:#float range:[-9999.9,9999.9,0.0]
            spinner spFMax1  "fMax: " fieldwidth:50 type:#float range:[0.0,99999.9,0.0]
            spinner spFudge1 "fudge: " fieldwidth:50 type:#float range:[0.0,1.0,1.0]
            spinner spBounce1 "bounce: " fieldwidth:50 type:#float range:[0.0,1.0,0.0]
            spinner spCfm1 "CFM: " fieldwidth:50 type:#float range:[0.0,9999.9,0.0]
            spinner spStopErp1 "stopERP: " fieldwidth:50 type:#float range:[0.0,1.0,0.2]
            spinner spStopCfm1 "stopCFM: " fieldwidth:50 type:#float range:[0.0,9999.9,0.0]
        )
        group "Axis #3 Params"
        (
            checkbox cbLoStop2 "loStop: " checked:false across:2
            checkbox cbHiStop2 "hiStop: " checked:false
                   angle angLoStop2 diameter:50 align:#left type:#float range:[-180,180,0] across:2
            angle angHiStop2 diameter:50 align:#right type:#float range:[-180,180,0]
            spinner spLoStop2 fieldwidth:40 align:#left type:#float range:[-180, 180,0] across:2
            spinner spHiStop2 fieldwidth:40 align:#right type:#float range:[-180, 180,0]
            spinner spVel2 "vel: " fieldwidth:50 type:#float range:[-9999.9,9999.9,0.0]
            spinner spFMax2  "fMax: " fieldwidth:50 type:#float range:[0.0,99999.9,0.0]
            spinner spFudge2 "fudge: " fieldwidth:50 type:#float range:[0.0,1.0,1.0]
            spinner spBounce2 "bounce: " fieldwidth:50 type:#float range:[0.0,1.0,0.0]
            spinner spCfm2 "CFM: " fieldwidth:50 type:#float range:[0.0,9999.9,0.0]
            spinner spStopErp2 "stopERP: " fieldwidth:50 type:#float range:[0.0,1.0,0.2]
            spinner spStopCfm2 "stopCFM: " fieldwidth:50 type:#float range:[0.0,9999.9,0.0]
        )
    
    
    
        function EnableAxis axis state =
        (
            if (axis == 1) then
            (
                if( state == true ) then
                (
                    cbLoStop0.enabled = state
                    cbHiStop0.enabled = state

                    angLoStop0.enabled = cbLoStop0.checked
                       angHiStop0.enabled = cbHiStop0.checked
                    spLoStop0.enabled = cbLoStop0.checked
                    spHiStop0.enabled = cbHiStop0.checked
                )
                else
                (
                    cbLoStop0.enabled = state
                    cbHiStop0.enabled = state
                            angLoStop0.enabled = state
                    angHiStop0.enabled = state
                    spLoStop0.enabled = state
                    spHiStop0.enabled = state
                )

                spVel0.enabled = state
                spFMax0.enabled = state
                spFudge0.enabled = state
                spBounce0.enabled = state
                spCfm0.enabled = state
                spStopErp0.enabled = state
                spStopCfm0.enabled = state
            )
            else if (axis == 2) then
            (
                     if( state == true ) then
                (
                    cbLoStop1.enabled = state
                    cbHiStop1.enabled = state

                    angLoStop1.enabled = cbLoStop1.checked
                       angHiStop1.enabled = cbHiStop1.checked
                    spLoStop1.enabled = cbLoStop1.checked
                    spHiStop1.enabled = cbHiStop1.checked
                )
                else
                (
                    cbLoStop1.enabled = state
                    cbHiStop1.enabled = state
                            angLoStop1.enabled = state
                    angHiStop1.enabled = state
                    spLoStop1.enabled = state
                    spHiStop1.enabled = state
                )

                spVel1.enabled = state
                spFMax1.enabled = state
                spFudge1.enabled = state
                spBounce1.enabled = state
                spCfm1.enabled = state
                spStopErp1.enabled = state
                spStopCfm1.enabled = state
            )
            else if (axis == 3) then
            (
                     if( state == true ) then
                (
                    cbLoStop2.enabled = state
                    cbHiStop2.enabled = state

                    angLoStop2.enabled = cbLoStop2.checked
                       angHiStop2.enabled = cbHiStop2.checked
                    spLoStop2.enabled = cbLoStop2.checked
                    spHiStop2.enabled = cbHiStop2.checked
                )
                else
                (
                    cbLoStop2.enabled = state
                    cbHiStop2.enabled = state
                            angLoStop2.enabled = state
                    angHiStop2.enabled = state
                    spLoStop2.enabled = state
                    spHiStop2.enabled = state
                )

                spVel2.enabled = state
                spFMax2.enabled = state
                spFudge2.enabled = state
                spBounce2.enabled = state
                spCfm2.enabled = state
                spStopErp2.enabled = state
                spStopCfm2.enabled = state
            )
        )

        function SetJoint type =
        (
            case type of
            (
                1:-- none
                (
                    EnableAxis 1 false
                    EnableAxis 2 false
                    EnableAxis 3 false
                )
                2:-- ball
                (
                    EnableAxis 1 false
                    EnableAxis 2 false
                    EnableAxis 3 false
                )
                3:-- hinge
                (
                    EnableAxis 1 true
                    EnableAxis 2 false
                    EnableAxis 3 false
                )
                4:-- hinge2
                (
                    EnableAxis 1 true
                    EnableAxis 2 true
                    EnableAxis 3 false
                )
                5:-- slider
                (
                    EnableAxis 1 true
                    EnableAxis 2 false
                    EnableAxis 3 false
                )
                6:-- universal
                (
                    EnableAxis 1 true
                    EnableAxis 2 true
                    EnableAxis 3 false
                )
                7:-- angular motor
                (
                    EnableAxis 1 true
                    EnableAxis 2 true
                    EnableAxis 3 true
                )
                8:-- fxied
                (
                    EnableAxis 1 false
                    EnableAxis 2 false
                    EnableAxis 3 false
                )
            )
    
            -- if type is hinge2,
            if( type == 4 ) then
            (
                spSuspERP.enabled = true
                spSuspCFM.enabled = true
            )
            else
            (
                spSuspERP.enabled = false
                spSuspCFM.enabled = false
            )
        )

        -- axis #1
        on spLoStop0     changed val do angLoStop0.degrees = val
        on spHiStop0     changed val do angHiStop0.degrees = val
        on angLoStop0     changed val do spLoStop0.value = val
        on angHiStop0     changed val do spHiStop0.value = val
        -- axis #2
        on spLoStop1     changed val do angLoStop1.degrees = val
        on spHiStop1     changed val do angHiStop1.degrees = val
        on angLoStop1     changed val do spLoStop1.value = val
        on angHiStop1     changed val do spHiStop1.value = val
        -- axis #3
        on spLoStop2     changed val do angLoStop2.degrees = val
        on spHiStop2     changed val do angHiStop2.degrees = val
        on angLoStop2     changed val do spLoStop2.value = val
        on angHiStop2     changed val do spHiStop2.value = val

        -- axis #1
        on cbLoStop0    changed val do
        (
            angLoStop0.enabled = val;
            spLoStop0.enabled = val;
        )
        on cbHiStop0    changed val do
        (
            angHiStop0.enabled = val;
            spHiStop0.enabled = val;
        )

        -- axis #2
        on cbLoStop1    changed val do
        (
            angLoStop1.enabled = val;
            spLoStop1.enabled = val;
        )
        on cbHiStop1    changed val do
        (
            angHiStop1.enabled = val;
            spHiStop1.enabled = val;
        )

        -- axis #3
        on cbLoStop2    changed val do
        (
            angLoStop2.enabled = val;
            spLoStop2.enabled = val;
        )
        on cbHiStop2    changed val do
        (
            angHiStop2.enabled = val;
            spHiStop2.enabled = val;
        )

        on cbJoint changed val do
        (
            ddlJoint.enabled = val
            if ((val == true) and (ddlJoint.selection != undefined)) then
            (
                SetJoint (ddlJoint.selection)
            )
            else
            (
                SetJoint 1    -- default: none
            )
        )
    
        on ddlJoint selected sel do
        (
            SetJoint sel
        )

        --
        on rJoint open do
        (
            ddlJoint.enabled = hasJoint
            if((hasJoint == true) and (ddlJoint.selection != undefined)) then
            (
                SetJoint (ddlJoint.selection)
            )
            else
            (
                SetJoint 1
            )
        )
    )
)
