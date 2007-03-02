-------------------------------------------------------------------------------
--  physxhelper.ms
--
--  Cho, Jun Hyeong 2007(c)
-------------------------------------------------------------------------------


-------------------------------------------------------------------------------
-- @desc
-- rigid body shape 를 max helper 객체로 만들려고 했으나
-- ode shape에 맞는 적절한 helper 객체의 모양이 없는 이유로
-- geometry 객체로 만들게 되었으므로 model export 시 이점 유념해야함.
-------------------------------------------------------------------------------

g_materials = #()

-------------------------------------------------------------------------------
-- @name CollectMaterials
--
-- @bug density 항목을 제대로 읽을수가 없다.(undefined 값으로 읽음. 왜?)
--
-------------------------------------------------------------------------------

-- Collect material info of materials.xml
fn CollectMaterials =
(
    excel = createOLEObject "excel.application"
    excel.workbooks.open "k:\\materials.xml"

    row = 3;
    col = 1;

    cellName = (excel.application.cells row 1)
    --cellDensity = (excel.application.cells row 2)
    while cellName.value != undefined do
    (
        --m = cellName.value + "(" + cellDensity.value as string + ")"
        --append g_materials m

        append g_materials cellName.value
        row += 1

        cellName = excel.application.cells row 1
        --cellDensity = excel.application.cells row 2
    )


    -- test code
    --for i = 1 to g_materials.count do
    --(
    --    format "%\n" g_materials[i]
    --)

    releaseOLEObject excel
)

-- collect materials
CollectMaterials()

-------------------------------------------------------------------------------
-- @name ODEBodyBox
--
--
-------------------------------------------------------------------------------
plugin geometry ODEBox
name:"Box"
category:"ODE Shape (Mangalore)"
classID:#(0x5bfe4c36, 0x4f0521a4)
extends:box
replaceUI:false
invisible:false
--initialRollupState:0x01
(
    parameters paramODEBox rollout: rODEBox
    (
        materialIndex type:#integer ui:ddlMaterials default:1
    )

    rollout rODEBox "ODE Box"
    (
        label lbMaterial "Material(Density):" align:#left
        dropdownlist ddlMaterials items:g_materials align:#left
    )

    on attachedToNode node do
    (
        node.xray = true
        node.wirecolor = red
    )
)

-------------------------------------------------------------------------------
-- @name ODESphere
--
--
-------------------------------------------------------------------------------
plugin geometry ODESphere
name:"Sphere"
category:"ODE Shape (Mangalore)"
classID:#(0x378e132b, 0x684b5a32)
extends:sphere
replaceUI:false
invisible:false
--initialRollupState:0x01
(
    parameters paramODESphere rollout: rODESphere
    (
        materialIndex type:#integer ui:ddlMaterials default:1
    )

    rollout rODESphere "ODE Sphere"
    (
        label lbMaterial "Material(Density):" align:#left
        dropdownlist ddlMaterials items:g_materials align:#left
    )

    on attachedToNode node do
    (
        node.xray = true
        node.wirecolor = red
    )
)

-------------------------------------------------------------------------------
-- @name ODECapsule
--
--
-------------------------------------------------------------------------------
plugin geometry ODECapsule
name:"Capsule"
category:"ODE Shape (Mangalore)"
classID:#(0x48c82dec, 0x26510860)
extends:capsule
replaceUI:false
invisible:false
--initialRollupState:0x01
(
    parameters paramODECapsule rollout: rODECapsule
    (
        materialIndex type:#integer ui:ddlMaterials default:1
    )

    rollout rODECapsule "ODE Capsule"
    (
        label lbMaterial "Material(Density):" align:#left
        dropdownlist ddlMaterials items:g_materials align:#left
    )

    on attachedToNode node do
    (
        node.xray = true
        node.wirecolor = red
    )
)

-------------------------------------------------------------------------------
-- @todo ODEPlane??
--
--
-------------------------------------------------------------------------------


