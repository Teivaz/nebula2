//------------------------------------------------------------------------------
//  physics/compositeloader.cc
//  (C) 2005 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "physics/compositeloader.h"
#include "physics/rigidbody.h"
#include "physics/server.h"
#include "physics/boxshape.h"
#include "physics/sphereshape.h"
#include "physics/capsuleshape.h"
#include "physics/hingejoint.h"
#include "physics/meshshape.h"
#include "physics/universaljoint.h"
#include "physics/sliderjoint.h"
#include "physics/balljoint.h"
#include "physics/hinge2joint.h"

namespace Physics
{

//------------------------------------------------------------------------------
/**
*/
CompositeLoader::CompositeLoader()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
CompositeLoader::~CompositeLoader()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Read the rigid body definitions from a joint definition and set on
    joint.
*/
void
CompositeLoader::ParseJointRigidBodies(Composite* composite, const nStream& stream, Joint* joint)
{
    n_assert(joint);
    RigidBody* body1 = 0;
    RigidBody* body2 = 0;
    if (stream.HasAttr("body1"))
    {
        body1 = composite->GetBodyByName(stream.GetString("body1"));
    }
    if (stream.HasAttr("body2"))
    {
        body2 = composite->GetBodyByName(stream.GetString("body2"));
    }
    joint->SetBodies(body1, body2);
}

//------------------------------------------------------------------------------
/**
    Constructs a complete Physics::Composite object from a composite
    xml file.

    @param  filename    composite resource name
    @return             pointer to Physics::Composite object or 0 on error

    - 25-Oct-05     floh    added support for static collide shapes
*/
Composite*
CompositeLoader::Load(const nString& filename)
{
    n_assert(!filename.IsEmpty());

    // open stream for writing
    nString path("physics:");
    path.Append(filename);
    path.Append(".xml");
    
    nStream stream(path);
    if (stream.Open(nStream::Read))
    {
        // make sure its a composite file
        if (!stream.HasNode("/Composite"))
        {
            n_error("Not a valid physics composite file: '%s'!", path.Get());
            return 0;
        }
        stream.SetToNode("/Composite");

        // create composite object
        Composite* composite = 0;
        if (stream.HasAttr("type") && (stream.GetString("type") == "ragdoll"))
        {
            composite = (Composite*) Physics::Server::Instance()->CreateRagdoll();
            n_assert(composite->IsA(Composite::RTTI));
        }
        else
        {
            composite = Physics::Server::Instance()->CreateComposite();
        }
        n_assert(composite);

        // get number of bodies and joints in composite
        int numBodies = stream.GetInt("numBodies");
        int numJoints = stream.GetInt("numJoints");
        int numShapes = stream.GetInt("numMeshes");
        if (numBodies > 0)
        {
            composite->BeginBodies(numBodies);
        }
        if (numJoints > 0)
        {
            composite->BeginJoints(numJoints);
        }
        if (numShapes > 0)
        {
            composite->BeginShapes(numShapes);
        }

        // iterate rigid bodies
        if (stream.SetToFirstChild("RigidBody")) do
        {
            // construct a rigid body and add to composite
            Ptr<RigidBody> body = Physics::Server::Instance()->CreateRigidBody();
            n_assert(body != 0);
            body->SetName(stream.GetString("name"));
            if (stream.HasAttr("jointCollide"))
            {
                body->SetConnectedCollision(stream.GetBool("jointCollide"));
            }
            if (stream.HasAttr("model"))
            {
                body->SetLinkName(RigidBody::ModelNode, stream.GetString("model"));
            }
            if (stream.HasAttr("shadow"))
            {
                body->SetLinkName(RigidBody::ShadowNode, stream.GetString("shadow"));
            }
            if (stream.HasAttr("joint"))
            {
                body->SetLinkName(RigidBody::JointNode, stream.GetString("joint"));
            }
            body->BeginShapes(stream.GetInt("numShapes"));
           
            vector3 initialPos;
            vector4 initialRot(0.0f, 0.0f, 0.0f, 1.0f);
            if (stream.HasAttr("pos"))
            {
                initialPos = stream.GetVector3("pos");
            }
            if (stream.HasAttr("rot"))
            {
                initialRot = stream.GetVector4("rot");
            }

            matrix44 initialTransform(quaternion(initialRot.x, initialRot.y, initialRot.z, initialRot.w));
            initialTransform.translate(initialPos);
            body->SetInitialTransform(initialTransform);

            // parse box shapes
            if (stream.SetToFirstChild("BoxShape")) do
            {
                vector3 pos = stream.GetVector3("pos");
                vector4 rot = stream.GetVector4("rot");
                vector3 size = stream.GetVector3("size");
                MaterialType matType = MaterialTable::StringToMaterialType(stream.GetString("mat").Get());

                matrix44 m(quaternion(rot.x, rot.y, rot.z, rot.w));
                m.translate(pos);

                Ptr<BoxShape> shape = Server::Instance()->CreateBoxShape(m, matType, size);
                n_assert(shape != 0);
                body->AddShape(shape);
            }
            while (stream.SetToNextChild("BoxShape"));

            // parse sphere shapes
            if (stream.SetToFirstChild("SphereShape")) do
            {
                vector3 pos  = stream.GetVector3("pos");
                float radius = stream.GetFloat("radius");
                MaterialType matType = MaterialTable::StringToMaterialType(stream.GetString("mat").Get());

                matrix44 m;
                m.translate(pos);

                Ptr<SphereShape> shape = Physics::Server::Instance()->CreateSphereShape(m, matType, radius);
                body->AddShape(shape);
            }
            while (stream.SetToNextChild("SphereShape"));

            // parse capsule shapes
            if (stream.SetToFirstChild("CapsuleShape")) do
            {
                vector3 pos  = stream.GetVector3("pos");
                vector4 rot  = stream.GetVector4("rot");
                float radius = stream.GetFloat("radius");
                float length = stream.GetFloat("length");
                MaterialType matType = MaterialTable::StringToMaterialType(stream.GetString("mat").Get());

                matrix44 m(quaternion(rot.x, rot.y, rot.z, rot.w));
                m.translate(pos);
                
                Ptr<CapsuleShape> shape = Server::Instance()->CreateCapsuleShape(m, matType, radius, length);
                n_assert(shape != 0);
                body->AddShape(shape);
            }
            while (stream.SetToNextChild("CapsuleShape"));

            // the body is ready, add it to the composite
            body->EndShapes();
            composite->AddBody(body);
        }
        while (stream.SetToNextChild("RigidBody"));

        // iterate BallAndSocket joints
        if (stream.SetToFirstChild("BallAndSocket")) do
        {
            // create a Ball-And-Socket joint
            Ptr<BallJoint> joint = Physics::Server::Instance()->CreateBallJoint();

            this->ParseJointRigidBodies(composite, stream, joint);
            if (stream.HasAttr("joint"))
            {
                joint->SetLinkName(stream.GetString("joint"));
            }
            joint->SetAnchor(stream.GetVector3("anchor"));
            composite->AddJoint(joint);
        }
        while (stream.SetToNextChild("BallAndSocket"));

        // iterate Hinge joints
        if (stream.SetToFirstChild("Hinge")) do
        {
            // create a Hinge joint
            Ptr<HingeJoint> joint = Physics::Server::Instance()->CreateHingeJoint();

            this->ParseJointRigidBodies(composite, stream, joint);
            if (stream.HasAttr("joint"))
            {
                joint->SetLinkName(stream.GetString("joint"));
            }
            joint->SetAnchor(stream.GetVector3("anchor"));

            JointAxis& axis = joint->AxisParams();
            axis.SetAxis(stream.GetVector3("axis"));
            if (stream.HasAttr("lostop0"))
            {
                axis.SetLoStopEnabled(true);
                axis.SetLoStop(n_deg2rad(stream.GetFloat("lostop0")));
            }
            if (stream.HasAttr("histop0"))
            {
                axis.SetHiStopEnabled(true);
                axis.SetHiStop(n_deg2rad(stream.GetFloat("histop0")));
            }
            axis.SetVelocity(n_deg2rad(stream.GetFloat("vel0")));
            axis.SetFMax(stream.GetFloat("fmax0"));
            axis.SetFudgeFactor(stream.GetFloat("fudge0"));
            axis.SetBounce(stream.GetFloat("bounce0"));
            axis.SetCFM(stream.GetFloat("cfm0"));
            axis.SetStopERP(stream.GetFloat("stopErp0"));
            axis.SetStopCFM(stream.GetFloat("stopCfm0"));

            composite->AddJoint(joint);
        }
        while (stream.SetToNextChild("Hinge"));

        // iterate Universal joints
        if (stream.SetToFirstChild("UniversalJoint")) do
        {
            // create a Universal joint
            Ptr<UniversalJoint> joint = Physics::Server::Instance()->CreateUniversalJoint();

            this->ParseJointRigidBodies(composite, stream, joint);
            if (stream.HasAttr("joint"))
            {
                joint->SetLinkName(stream.GetString("joint"));
            }
            joint->SetAnchor(stream.GetVector3("anchor"));

            JointAxis& axis0 = joint->AxisParams(0);
            axis0.SetAxis(stream.GetVector3("axis1"));
            if (stream.HasAttr("lostop0"))
            {
                axis0.SetLoStopEnabled(true);
                axis0.SetLoStop(n_deg2rad(stream.GetFloat("lostop0")));
            }
            if (stream.HasAttr("histop0"))
            {
                axis0.SetHiStopEnabled(true);
                axis0.SetHiStop(n_deg2rad(stream.GetFloat("histop0")));
            }
            axis0.SetVelocity(n_deg2rad(stream.GetFloat("vel0")));
            axis0.SetFMax(stream.GetFloat("fmax0"));
            axis0.SetFudgeFactor(stream.GetFloat("fudge0"));
            axis0.SetBounce(stream.GetFloat("bounce0"));
            axis0.SetCFM(stream.GetFloat("cfm0"));
            axis0.SetStopERP(stream.GetFloat("stopErp0"));
            axis0.SetStopCFM(stream.GetFloat("stopCfm0"));

            JointAxis& axis1 = joint->AxisParams(1);
            axis1.SetAxis(stream.GetVector3("axis2"));
            if (stream.HasAttr("lostop1"))
            {
                axis1.SetLoStopEnabled(true);
                axis1.SetLoStop(n_deg2rad(stream.GetFloat("lostop1")));
            }
            if (stream.HasAttr("histop1"))
            {
                axis1.SetHiStopEnabled(true);
                axis1.SetHiStop(n_deg2rad(stream.GetFloat("histop1")));
            }
            axis1.SetVelocity(n_deg2rad(stream.GetFloat("vel1")));
            axis1.SetFMax(stream.GetFloat("fmax1"));
            axis1.SetFudgeFactor(stream.GetFloat("fudge1"));
            axis1.SetBounce(stream.GetFloat("bounce1"));
            axis1.SetCFM(stream.GetFloat("cfm1"));
            axis1.SetStopERP(stream.GetFloat("stopErp1"));
            axis1.SetStopCFM(stream.GetFloat("stopCfm1"));

            composite->AddJoint(joint);
        }
        while (stream.SetToNextChild("UniversalJoint"));

        // iterate slider joints
        if (stream.SetToFirstChild("Slider")) do
        {
            // create a slider joint
            Ptr<SliderJoint> joint = Physics::Server::Instance()->CreateSliderJoint();

            this->ParseJointRigidBodies(composite, stream, joint);
            if (stream.HasAttr("joint"))
            {
                joint->SetLinkName(stream.GetString("joint"));
            }

            JointAxis& axis = joint->AxisParams();
            axis.SetAxis(stream.GetVector3("axis"));
            if (stream.HasAttr("lostop0"))
            {
                axis.SetLoStopEnabled(true);
                axis.SetLoStop(n_deg2rad(stream.GetFloat("lostop0")));
            }
            if (stream.HasAttr("histop0"))
            {
                axis.SetHiStopEnabled(true);
                axis.SetHiStop(n_deg2rad(stream.GetFloat("histop0")));
            }
            axis.SetVelocity(n_deg2rad(stream.GetFloat("vel0")));
            axis.SetFMax(stream.GetFloat("fmax0"));
            axis.SetFudgeFactor(stream.GetFloat("fudge0"));
            axis.SetBounce(stream.GetFloat("bounce0"));
            axis.SetCFM(stream.GetFloat("cfm0"));
            axis.SetStopERP(stream.GetFloat("stopErp0"));
            axis.SetStopCFM(stream.GetFloat("stopCfm0"));

            composite->AddJoint(joint);
        }
        while (stream.SetToNextChild("Slider"));

        // iterate Hinge2 joints
        if (stream.SetToFirstChild("Hinge2")) do
        {
            // create hinge2 joint
            Ptr<Hinge2Joint> joint = Physics::Server::Instance()->CreateHinge2Joint();

            this->ParseJointRigidBodies(composite, stream, joint);
            if (stream.HasAttr("joint"))
            {
                joint->SetLinkName(stream.GetString("joint"));
            }
            joint->SetAnchor(stream.GetVector3("anchor"));

            JointAxis& axis0 = joint->AxisParams(0);            
            axis0.SetAxis(stream.GetVector3("axis1"));            
            if (stream.HasAttr("lostop0"))
            {
                axis0.SetLoStopEnabled(true);
                axis0.SetLoStop(n_deg2rad(stream.GetFloat("lostop0")));
            }
            if (stream.HasAttr("histop0"))
            {
                axis0.SetHiStopEnabled(true);
                axis0.SetHiStop(n_deg2rad(stream.GetFloat("histop0")));
            }
            axis0.SetVelocity(n_deg2rad(stream.GetFloat("vel0")));
            axis0.SetFMax(stream.GetFloat("fmax0"));
            axis0.SetFudgeFactor(stream.GetFloat("fudge0"));
            axis0.SetBounce(stream.GetFloat("bounce0"));
            axis0.SetCFM(stream.GetFloat("cfm0"));
            axis0.SetStopERP(stream.GetFloat("stopErp0"));
            axis0.SetStopCFM(stream.GetFloat("stopCfm0"));

            JointAxis& axis1 = joint->AxisParams(1);
            axis1.SetAxis(stream.GetVector3("axis2"));
            if (stream.HasAttr("lostop1"))
            {
                axis1.SetLoStopEnabled(true);
                axis1.SetLoStop(n_deg2rad(stream.GetFloat("lostop1")));
            }
            if (stream.HasAttr("histop1"))
            {
                axis1.SetHiStopEnabled(true);
                axis1.SetHiStop(n_deg2rad(stream.GetFloat("histop1")));
            }
            axis1.SetVelocity(n_deg2rad(stream.GetFloat("vel1")));
            axis1.SetFMax(stream.GetFloat("fmax1"));
            axis1.SetFudgeFactor(stream.GetFloat("fudge1"));
            axis1.SetBounce(stream.GetFloat("bounce1"));
            axis1.SetCFM(stream.GetFloat("cfm1"));
            axis1.SetStopERP(stream.GetFloat("stopErp1"));
            axis1.SetStopCFM(stream.GetFloat("stopCfm1"));

            joint->SetSuspensionERP(stream.GetFloat("suspERP"));
            joint->SetSuspensionCFM(stream.GetFloat("suspCFM"));

            composite->AddJoint(joint);
        }
        while (stream.SetToNextChild("Hinge2"));

        // iterate shapes
        if (stream.SetToFirstChild("MeshShape"))
        {
            static const matrix44 identity;
            nString filename = stream.GetString("file");
            MaterialType matType = MaterialTable::StringToMaterialType(stream.GetString("mat").Get());
            Ptr<MeshShape> meshShape = Physics::Server::Instance()->CreateMeshShape(identity, matType, filename);
            composite->AddShape(meshShape.get());
        }
        while (stream.SetToNextChild("MeshShape"));

        if (numBodies > 0)
        {
            composite->EndBodies();
        }
        if (numJoints > 0)
        {
            composite->EndJoints();
        }
        if (numShapes > 0)
        {
            composite->EndShapes();
        }

        stream.Close();
        return composite;
    }
    return 0;
}

} // namespace Physics