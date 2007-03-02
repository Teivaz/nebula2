-------------------------------------------------------------------------------
--  physxexport.ms
--
--  (c)2007 Cho, Jun Hyeong
-------------------------------------------------------------------------------

include "mangalore/physxxmlutil.ms"


-- ODE Body 를 collecting 하기 위한 구조체
struct ODEBody ( o, m );


-------------------------------------------------------------------------------
-- @name nPoint3
--
-- nebula 좌표계로 변환한다.
-------------------------------------------------------------------------------
fn nPoint3 p =
(
    local str = ""
    str += ((-p.x) as string)
    str += " "
    str += (p.z as string)
    str += " "
    str += (p.y as string)
    return str;
)

-------------------------------------------------------------------------------
-- @name nQuat
--
-- nebula 좌표계로 변환한다.
-------------------------------------------------------------------------------
fn nQuat q =
(
    local str = ""
    str += ((-q.x) as string)
    str += " "
    str += (q.z as string)
    str += " "
    str += (q.y as string)
    str += " "
    str += ((-q.w) as string)
    return str;
)

-------------------------------------------------------------------------------
-- @name ExportODEBox
--
-- x = o.width
-- y = o.height
-- z = o.length
-------------------------------------------------------------------------------
fn ExportODEBox o =
(
    pos = in coordsys parent o.pos;
    rot = in coordsys parent o.rotation;
     size = ""
    size += (o.delegate.width as string);
    size = " "
    size += (o.delegate.height as string);
    size = " "
    size += (o.delegate.length as string);

    xmlElementBegin "BoxShape"
    xmlAddAttribute "pos" (nPoint3 pos)
    xmlAddAttribute "rot" (nQuat rot)
    xmlAddAttribute "size" size;
    xmlAddAttribute "mat" g_materials[o.materialIndex];
    xmlElementEnd() -- BoxShape

)

-------------------------------------------------------------------------------
-- @name ExportODESphere
--
-------------------------------------------------------------------------------
fn ExportODESphere o =
(
    pos = in coordsys parent o.pos;

    xmlElementBegin "SphereBody"
    xmlAddAttribute "pos" (nPoint3 pos)
    xmlAddAttribute "radius" o.delegate.radius;
    xmlAddAttribute "mat" g_materials[o.materialIndex];
    xmlElementEnd() -- SphereBody

)

-------------------------------------------------------------------------------
-- @name ExportODECapsule
--
-------------------------------------------------------------------------------
fn ExportODECapsule o =
(
    pos = in coordsys parent o.pos;
    rot = in coordsys parent o.rotation;

    xmlElementBegin "CapsuleBody"
    xmlAddAttribute "pos" (nPoint3 pos)
    xmlAddAttribute "rot" (nQuat rot)
    xmlAddAttribute "radius" o.delegate.radius;
    xmlAddAttribute "length" o.delegate.height;
    xmlAddAttribute "mat" g_materials[o.materialIndex];
    xmlElementEnd() -- CapsuleBody
)

-------------------------------------------------------------------------------
-- @name ExportODEShape
--
-------------------------------------------------------------------------------
fn ExportODEShape o =
(
    c = (classof o.baseobject);

    if( c == ODEBox ) then
    (
        ExportODEBox o;
    )
    else if( c == ODESphere ) then
    (
        ExportODESphere o;
    )
    else if( c == ODECapsule ) then
    (
        ExportODECapsule o;
    )
)

-------------------------------------------------------------------------------
-- @name IsODEShape
--
-------------------------------------------------------------------------------
fn IsODEShape o =
(
    c = (classof o);

    if( c == ODEBox ) then return true;
    else if( c == ODESphere ) then return true;
    else if( c == ODECapsule ) then return true;
    else return false;
)

-------------------------------------------------------------------------------
-- @name ExportODEBody
--
-- @desc
-- ODE Body(Rigid Body)는 실제로 화면상에 렌더링될 오브젝트에 설정해야한다.
-- ODE Body로 설정된 오브젝트는 ODE Shape를 자식으로 가질수 있다.
--
-- @todo
-- rigidbody 의 pos 는 pivot 점이 아니라 무게 중심점 일까?
--
-- @tip
-- RigidBody class 의 attach 함수에서
-- RigidBody 가 가지는 Shape 들의 POR(질량 중심점)을 계산하므로
-- RigidBody 의 위치는 상관이 없을듯하다.
--
-- 그래서 physics1.xml 예처럼
-- ode body 와 joint 들을 따로 구성해서 링크하는 형식의 방법을 따르지 않아도 될듯하다.
-- 이러한 방식은 작업방식이 더 번거러울듯 하다.
--
-- 최대한 추가적인 helper나 dummy 오브젝트의 작업이 없이 data 를 구성할수 있게 하여
-- 작업상 편의성을 추구한다.
--
-- ragdoll 의 경우의 rigidbody 는 model을 지정하지 않는다.
-- biped 에 강체를 설정한다.
-- skim mesh 는 biped 를 따라 갈것이기 때문이다.
-------------------------------------------------------------------------------
fn ExportODEBody o m isRagDoll =
(
    pos = in coordsys world o.pos;
    --pos = in coordsys world o.center;
    rot = in coordsys world o.rotation;

    xmlElementBegin "RigidBody"
    xmlAddAttribute "name" o.name;
    xmlAddAttribute "pos" (nPoint3 pos)
        xmlAddAttribute "rot" (nQuat rot)

    --if( m.hasJoint == true )
    --    xmlAddAttribute "jointCollide" "true";

    if( (classof o.baseobject) == Editable_mesh ) then
    (
        xmlAddAttribute "model" o.name
    )

    --xmlAddAttribute "shadow" "?";

    -- if physic type is rag doll(2),
    if( isRagDoll ) then
    (
       xmlAddAttribute "joint" o.name
    )

    -- collect ode shape
    local shapeArray = #();
    for c in o.children do
    (
        if( IsODEShape(c) == true ) then
        (
            append shapeArray c;
        )
    )

    xmlAddAttribute "numShape" shapeArray.count;

    -- export ode shape
    for shape in shapeArray do
    (
        ExportODEShape shape;
    )

    xmlElementEnd() -- RigidBody
)

-------------------------------------------------------------------------------
-- @name ExportODEJoint
--
-- object 에 ode body modifier 가 적용이 되고 joint 가 설정이 되었을 경우.
-- 이 조인트에 링크된 body1, body2 는 자신과 자신의 부모이다.
-- 즉 자신과 자신의 부모는 ode body 가 설정되어야 한다.
-- 부모가 없을 경우 정적환경에 링크된다.
-------------------------------------------------------------------------------
fn ExportODEJoint o m =
(
    if( m.hasJoint == false ) then return;

    -- ball
    if( m.jointType == 2 ) then
    (
        ExportJointBallAndSocket o m
    )
    -- hinge
    else if( m.jointType == 3 ) then
    (
        ExportJointHinge o m
    )
    -- hinge2
    else if( m.jointType == 4 ) then
    (
        ExportJointHinge2 o m
    )
    -- slider
    else if( m.jointType == 5 ) then
    (
        ExportJointSlider o m
    )
    -- universal
    else if( m.jointType == 6 ) then
    (
        ExportJointUniversal o m
    )
    -- amotor
    else if( m.jointType == 7 ) then
    (
        ExportJointAMotor o m
    )
    -- fixed
    else if( m.jointType == 8 ) then
    (
        -- not implemented
    )
)

-------------------------------------------------------------------------------
-- @name ExportJointBallAndSocket
--
-------------------------------------------------------------------------------
fn ExportJointBallAndSocket o m =
(
    xmlElementBegin "BallAndSocket"
    xmlAddAttribute "body1" o.name

    p = o.parent
    for m in p.modifiers do
    (
        if( m.name == "ODE Body (Mangalore)" ) then
        (
            xmlAddAttribute "body2" p.name
        )
    )

    --xmlAddAttribute "joint" ?
    xmlAddAttribute "anchor" o.pos
    xmlElementEnd() -- BallAndSocket
)

-------------------------------------------------------------------------------
-- @name ExportJointHinge
--
-------------------------------------------------------------------------------
fn ExportJointHinge o m =
(
    xmlElementBegin "Hinge"
    xmlAddAttribute "body1" o.name

    p = o.parent
    for m in p.modifiers do
    (
        if( m.name == "ODE Body (Mangalore)" ) then
        (
            xmlAddAttribute "body2" p.name
        )
    )

    --xmlAddAttribute "joint" ?
    xmlAddAttribute "anchor" o.pos
    
    -- axis
    xmlAddAttribute "axis" o.pos        -- default: axis(0.0, 1.0, 0.0)
    if( m.isLoStop0 ) then
    (
        xmlAddAttribute "lostop0" m.loStop0
    )
    if( m.isHiStop0 ) then
    (
        xmlAddAttribute "histop0" m.hiStop0
    )
    xmlAddAttribute "vel0" m.vel0
    xmlAddAttribute "fmax0" m.fMax0
    xmlAddAttribute "fudge0" m.fudge0
    xmlAddAttribute "bounce0" m.bounce0
    xmlAddAttribute "cfm0" m.cfm0
    xmlAddAttribute "stopErp0" m.stopErp0
    xmlAddAttribute "stopCfm0" m.stopCfm0

    xmlElementEnd() -- Hinge
)

-------------------------------------------------------------------------------
-- @name ExportJointHinge2
--
-------------------------------------------------------------------------------
fn ExportJointHinge2 o m =
(
    xmlElementBegin "Hinge2"
    xmlAddAttribute "body1" o.name

    p = o.parent
    for m in p.modifiers do
    (
        if( m.name == "ODE Body (Mangalore)" ) then
        (
            xmlAddAttribute "body2" p.name
        )
    )

    --xmlAddAttribute "joint" ?
    xmlAddAttribute "anchor" o.pos

    -- axis #1
    xmlAddAttribute "axis1" o.pos        -- default: axis(0.0, 1.0, 0.0)
    if( m.isLoStop0 ) then
    (
        xmlAddAttribute "lostop0" m.loStop0
    )
    if( m.isHiStop0 ) then
    (
        xmlAddAttribute "histop0" m.hiStop0
    )
    xmlAddAttribute "vel0" m.vel0
    xmlAddAttribute "fmax0" m.fMax0
    xmlAddAttribute "fudge0" m.fudge0
    xmlAddAttribute "bounce0" m.bounce0
    xmlAddAttribute "cfm0" m.cfm0
    xmlAddAttribute "stopErp0" m.stopErp0
    xmlAddAttribute "stopCfm0" m.stopCfm0

    -- axis #2
    xmlAddAttribute "axis2" o.pos        -- default: axis(0.0, 1.0, 0.0)
    if( m.isLoStop1 ) then
    (
        xmlAddAttribute "lostop1" m.loStop1
    )
    if( m.isHiStop1 ) then 
    (
        xmlAddAttribute "histop1" m.hiStop1
    )
    xmlAddAttribute "vel1" m.vel1
    xmlAddAttribute "fmax1" m.fMax1
    xmlAddAttribute "fudge1" m.fudge1
    xmlAddAttribute "bounce1" m.bounce1
    xmlAddAttribute "cfm1" m.cfm1
    xmlAddAttribute "stopErp1" m.stopErp1
    xmlAddAttribute "stopCfm1" m.stopCfm1

    xmlAddAttribute "suspERP" m.suspERP
    xmlAddAttribute "suspCFM" m.suspCFM

    xmlElementEnd() -- Hinge2
)

-------------------------------------------------------------------------------
-- @name ExportJointSlider
--
-------------------------------------------------------------------------------
fn ExportJointSlider o m =
(
    xmlElementBegin "Slider"
    xmlAddAttribute "body1" o.name

    p = o.parent
    for m in p.modifiers do
    (
        if( m.name == "ODE Body (Mangalore)" ) then
        (
            xmlAddAttribute "body2" p.name
        )
    )

    --xmlAddAttribute "joint" ?
    xmlAddAttribute "anchor" o.pos
    
    xmlAddAttribute "axis" o.pos        -- default: axis(0.0, 1.0, 0.0)
    if( m.isLoStop0 ) then
    (
        xmlAddAttribute "lostop0" m.loStop0
    )
    if( m.isHiStop0 ) then
    (
        xmlAddAttribute "histop0" m.hiStop0
    )
    xmlAddAttribute "vel0" m.vel0
    xmlAddAttribute "fmax0" m.fMax0
    xmlAddAttribute "fudge0" m.fudge0
    xmlAddAttribute "bounce0" m.bounce0
    xmlAddAttribute "cfm0" m.cfm0
    xmlAddAttribute "stopErp0" m.stopErp0
    xmlAddAttribute "stopCfm0" m.stopCfm0

    xmlElementEnd() -- Hinge
)

-------------------------------------------------------------------------------
-- @name ExportJointUniversal
--
-------------------------------------------------------------------------------
fn ExportJointUniversal o m =
(
    xmlElementBegin "UniversalJoint"
    xmlAddAttribute "body1" o.name

    p = o.parent
    for m in p.modifiers do
    (
        if( m.name == "ODE Body (Mangalore)" ) then
        (
            xmlAddAttribute "body2" p.name
        )
    )

    --xmlAddAttribute "joint" ?
    xmlAddAttribute "anchor" o.pos

    -- axis #1
    xmlAddAttribute "axis1" o.pos        -- default: axis(0.0, 1.0, 0.0)
    if( m.isLoStop0 ) then
    (
        xmlAddAttribute "lostop0" m.loStop0
    )
    if( m.isHiStop0 ) then
    (
        xmlAddAttribute "histop0" m.hiStop0
    )
    xmlAddAttribute "vel0" m.vel0
    xmlAddAttribute "fmax0" m.fMax0
    xmlAddAttribute "fudge0" m.fudge0
    xmlAddAttribute "bounce0" m.bounce0
    xmlAddAttribute "cfm0" m.cfm0
    xmlAddAttribute "stopErp0" m.stopErp0
    xmlAddAttribute "stopCfm0" m.stopCfm0

    -- axis #2
    xmlAddAttribute "axis2" o.pos        -- default: axis(0.0, 1.0, 0.0)
    if( m.isLoStop1 ) then
    (
        xmlAddAttribute "lostop1" m.loStop1
    )
    if( m.isHiStop1 ) then
    (
        xmlAddAttribute "histop1" m.hiStop1
    )
    xmlAddAttribute "vel1" m.vel1
    xmlAddAttribute "fmax1" m.fMax1
    xmlAddAttribute "fudge1" m.fudge1
    xmlAddAttribute "bounce1" m.bounce1
    xmlAddAttribute "cfm1" m.cfm1
    xmlAddAttribute "stopErp1" m.stopErp1
    xmlAddAttribute "stopCfm1" m.stopCfm1

    xmlElementEnd() -- UniversalJoint
)

-------------------------------------------------------------------------------
-- @name ExportJointAMotor
--
-------------------------------------------------------------------------------
fn ExportJointAMotor o m =
(
    xmlElementBegin "AMotor"
    xmlAddAttribute "body1" o.name

    p = o.parent
    for m in p.modifiers do
    (
        if( m.name == "ODE Body (Mangalore)" ) then
        (
            xmlAddAttribute "body2" p.name
        )
    )

    --xmlAddAttribute "joint" ?
    xmlAddAttribute "anchor" o.pos

    -- compositeloader.cc 를 보면 무조건 2개이다.
    xmlAddAttribute "numAxis" 2

    -- axis #1
    xmlAddAttribute "axis1" o.pos        -- default: axis(0.0, 1.0, 0.0)
    if( m.isLoStop0 ) then
    (
        xmlAddAttribute "lostop0" m.loStop0
    )
    if( m.isHiStop0 ) then
    (
        xmlAddAttribute "histop0" m.hiStop0
    )
    xmlAddAttribute "vel0" m.vel0
    xmlAddAttribute "fmax0" m.fMax0
    xmlAddAttribute "fudge0" m.fudge0
    xmlAddAttribute "bounce0" m.bounce0
    xmlAddAttribute "cfm0" m.cfm0
    xmlAddAttribute "stopErp0" m.stopErp0
    xmlAddAttribute "stopCfm0" m.stopCfm0

    -- axis #2
    xmlAddAttribute "axis2" o.pos        -- default: axis(0.0, 1.0, 0.0)
    if( m.isLoStop1 ) then
    (
        xmlAddAttribute "lostop1" m.loStop1
    )
    if( m.isHiStop1 ) then
    (
        xmlAddAttribute "histop1" m.hiStop1
    )
    xmlAddAttribute "vel1" m.vel1
    xmlAddAttribute "fmax1" m.fMax1
    xmlAddAttribute "fudge1" m.fudge1
    xmlAddAttribute "bounce1" m.bounce1
    xmlAddAttribute "cfm1" m.cfm1
    xmlAddAttribute "stopErp1" m.stopErp1
    xmlAddAttribute "stopCfm1" m.stopCfm1

    xmlElementEnd() -- AMotor
)


-------------------------------------------------------------------------------
-- @name ExportODE
--
-- @todo
--
-------------------------------------------------------------------------------
fn ExportODE filename physicType =
(
    -- physics type
    isRagDoll = false;
    if( physicType == 2 )    then isRagDoll = true;

    -- create file
    outfile = createFile filename;
    if( outfile == undefined ) then
    (
        messageBox("Error: cannot create file \"" + filename + "\"");
        return false;
    )

    local odeBodyArray = #();
    local odeJointArray = #();

    -- collect ode body
    for o in objects do
    (
        if( o != undefined ) then
        (
            for m in o.modifiers do
            (
                if( m.name == "ODE Body (Mangalore)" ) then
                (
                    odebody = ODEBody o m;
                    append odeBodyArray odebody;
                )
            )
        )
    )

    -- collect ode joint
    for odeBody in odeBodyArray do
    (
        if( odeBody.m.hasJoint ) then
        (
            append odeJointArray odeBody
        )
    )

    -- XML Begin
    xmlBegin outfile;

    xmlElementBegin "Composite"
    if( isRagDoll ) then
    (
        xmlAddAttribute "type" "ragdoll"
    )
    else
    (
        xmlAddAttribute "type" "standard"
    )
    xmlAddAttribute "numBodies" odeBodyArray.count;
    xmlAddAttribute "numJoints" odeJointArray.count;
    xmlAddAttribute "numMeshes" 0;
    --xmlAddAttribute "numShapes" 0;
        
    -- todo : count shape
    -- todo : mesh ?

    -- Export Rigid Body
    for odeBody in odeBodyArray do
    (
        ExportODEBody odeBody.o odeBody.m isRagDoll
    )

        -- Export Joint
    for odeJoint in odeJointArray do
    (
        ExportODEJoint odeJoint.o odeJoint.m
    )

    xmlElementEnd() -- Composite

    xmlEnd() -- XML End
)

-------------------------------------------------------------------------------
-- @name ODEExporter
--
-- @todo
-- 1. viewer
-------------------------------------------------------------------------------
utility ODEExporter "Mangalore Physics Exporter" rolledup:true
(
    group "Physic Type"
    (
       radiobuttons physicType labels:#("Standard", "Rag Doll") default:1  align:#left
    )

    button btnExport "Export" --height:15 width:150

    on btnExport pressed do
    (
        filename = getSaveFileName caption:"Select File to Export" types:"XML (*.xml)|*.xml"

        if(filename == undefined) then return 0;

                -- export file
        ExportODE filename physicType.state;

        -- todo : launcher viewer
    )
)

max utility mode
addrollout ODEExporter
max create mode

-------------------------------------------------------------------------------
-- EOF
-------------------------------------------------------------------------------
