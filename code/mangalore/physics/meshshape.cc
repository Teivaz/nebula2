//------------------------------------------------------------------------------
//  physics/meshshape.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "physics/meshshape.h"
#include "gfx2/nn3d2loader.h"
#include "gfx2/nnvx2loader.h"
#include "foundation/server.h"

namespace Physics
{
ImplementRtti(Physics::MeshShape, Physics::Shape);
ImplementFactory(Physics::MeshShape);

Opcode::SphereCollider MeshShape::opcSphereCollider;
Opcode::RayCollider MeshShape::opcRayCollider;
Opcode::CollisionFaces MeshShape::opcFaces;

//------------------------------------------------------------------------------
/**
*/
MeshShape::MeshShape() :
    Shape(Mesh),
    vertexBuffer(0),
    indexBuffer(0),
    numVertices(0),
    vertexWidth(0),
    numIndices(0),
    odeTriMeshDataId(0)
{
    // initialize the OPCODE sphere collider
    opcSphereCollider.SetFirstContact(false);       // report all contacts
    opcSphereCollider.SetTemporalCoherence(true);   // use temporal coherence

    // initialize the OPCODE ray collider
    opcRayCollider.SetFirstContact(false);                 // report all contacts
    opcRayCollider.SetTemporalCoherence(false);            // no temporal coherence
    opcRayCollider.SetClosestHit(true);                    // return only closest hit
    opcRayCollider.SetCulling(true);                       // with backface culling
    opcRayCollider.SetDestination(&opcFaces);              // detected hits go here
}

//------------------------------------------------------------------------------
/**
*/
MeshShape::~MeshShape()
{
    if (this->IsAttached())
    {
        this->Detach();
    }
}

//------------------------------------------------------------------------------
/**
    Create a mesh shape object, add it to ODE's collision space, and initialize
    the mass member.
*/
bool
MeshShape::Attach(dSpaceID spaceId)
{
    if (Shape::Attach(spaceId))
    {
        n_assert(!filename.IsEmpty());
        n_assert(0 == this->vertexBuffer);
        n_assert(0 == this->indexBuffer);

        // fix my collide bits, we don't need to collide against other static and disabled entities
        this->SetCategoryBits(Static);
        this->SetCollideBits(~Static);

        // load the vertices and indices
        bool loadResult = false;
        if (this->filename.CheckExtension("nvx2"))
        {
            loadResult = this->LoadNvx2();
        }
        else if (this->filename.CheckExtension("n3d2"))
        {
            loadResult = this->LoadN3d2();
        }
        else
        {
            n_error("MeshShape: invalid file extension in '%s'", this->filename.Get());
            return false;
        }
        if (!loadResult)
        {
            n_error("MeshShape: Failed to load mesh file '%s'", this->filename.Get());
            return false;
        }

        // create an ODE TriMeshData object from the loaded vertices and indices
        this->odeTriMeshDataId = dGeomTriMeshDataCreate();
        dGeomTriMeshDataBuildSingle(this->odeTriMeshDataId,
                                    this->vertexBuffer,
                                    this->vertexWidth * sizeof(float),
                                    this->numVertices,
                                    this->indexBuffer,
                                    this->numIndices,
                                    3 * sizeof(int));

        this->odeTriMeshID = dCreateTriMesh(0, odeTriMeshDataId, 0, 0, 0);
        this->AttachGeom(this->odeTriMeshID, spaceId);

        // FIXME: apply shape mass here!
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    - 14-Jan-05 floh    memleak fixed, TriMeshData was not released
*/
void
MeshShape::Detach()
{
    n_assert(this->IsAttached());
    n_assert(0 != this->vertexBuffer);
    n_assert(0 != this->indexBuffer);

    dGeomTriMeshDataDestroy(this->odeTriMeshDataId);

    n_free(this->vertexBuffer);
    n_free(this->indexBuffer);
    this->vertexBuffer = 0;
    this->indexBuffer = 0;

    Shape::Detach();
};

//------------------------------------------------------------------------------
/**
    Load vertex and index buffer from a n3d2 file.

    @return             true if success
*/
bool
MeshShape::LoadN3d2()
{
    n_assert(!this->filename.IsEmpty());
    n_assert(0 == this->vertexBuffer);
    n_assert(0 == this->indexBuffer);

    // open n3d2 file and read header data
    nN3d2Loader meshLoader;
    meshLoader.SetFilename(this->filename.Get());
    meshLoader.SetIndexType(nMeshLoader::Index32);
    if (!meshLoader.Open())
    {
        n_error("MeshShape: Failed to open n3d2 file '%s'!", this->filename.Get());
        return false;
    }

    // transfer mesh attributes
    this->numVertices = meshLoader.GetNumVertices();
    this->numIndices  = meshLoader.GetNumIndices();
    this->vertexWidth = meshLoader.GetVertexWidth();

    // allocate vertex and index buffer
    int vbSize = meshLoader.GetNumVertices() * meshLoader.GetVertexWidth() * sizeof(float);
    int ibSize = meshLoader.GetNumIndices() * sizeof(uint);
    this->vertexBuffer = (float*) n_malloc(vbSize);
    this->indexBuffer  = (int*) n_malloc(ibSize);

    // read vertices and indices
    meshLoader.ReadVertices(this->vertexBuffer, vbSize);
    meshLoader.ReadIndices(this->indexBuffer, ibSize);

    // close mesh loader
    meshLoader.Close();

    return true;
}

//------------------------------------------------------------------------------
/**
    Load vertex and index buffer from a nvx2 file.

    @return             true if success
*/
bool
MeshShape::LoadNvx2()
{
    n_assert(!this->filename.IsEmpty());
    n_assert(0 == this->vertexBuffer);
    n_assert(0 == this->indexBuffer);

    // open nvx2 file and read header data
    nNvx2Loader meshLoader;
    meshLoader.SetFilename(filename.Get());
    meshLoader.SetIndexType(nMeshLoader::Index32);
    if (!meshLoader.Open())
    {
        n_error("MeshShape: Failed to open nvx2 file '%s'!", this->filename.Get());
        return false;
    }

    // transfer mesh attributes
    this->numVertices = meshLoader.GetNumVertices();
    this->numIndices  = meshLoader.GetNumIndices();
    this->vertexWidth = meshLoader.GetVertexWidth();

    // allocate vertex and index buffer
    int vbSize = meshLoader.GetNumVertices() * meshLoader.GetVertexWidth() * sizeof(float);
    int ibSize = meshLoader.GetNumIndices() * sizeof(uint);
    this->vertexBuffer = (float*) n_malloc(vbSize);
    this->indexBuffer  = (int*) n_malloc(ibSize);

    // read vertices and indices
    meshLoader.ReadVertices(this->vertexBuffer, vbSize);
    meshLoader.ReadIndices(this->indexBuffer, ibSize);

    // close mesh loader
    meshLoader.Close();

    return true;
}

//------------------------------------------------------------------------------
/**
    Does a sphere collision check. Returns the number of and the indices
    of all faces inside or intersecting the sphere.

    @param  s                   [in] the sphere in world space
    @param  outFaceIndices      [out] will be filled with pointer to face indices
    @return numFaces            number of collision faces
*/
int
MeshShape::DoSphereCollide(const sphere& s, uint*& outFaceIndices)
{
    // setup sphere
    IceMaths::Sphere opcSphere(IceMaths::Point(s.p.x, s.p.y, s.p.z), s.r);

    // get pointer to our opc model
    Opcode::Model* opcModel = dGeomTriMeshGetOpcodeModel(this->odeTriMeshID);
    n_assert(opcModel);

    // setup identity matrix for sphere
    IceMaths::Matrix4x4 sphereTransform;
    sphereTransform.Identity();

    // get matrix for our model
    IceMaths::Matrix4x4* modelTransform = (IceMaths::Matrix4x4*) &(this->transform);

    // do sphere collision check
    bool success = opcSphereCollider.Collide(this->opcSphereCache, opcSphere, *opcModel, &sphereTransform, modelTransform);
    n_assert(success);
    int numFaces = opcSphereCollider.GetNbTouchedPrimitives();
    if (numFaces > 0)
    {
        outFaceIndices = (uint*) opcSphereCollider.GetTouchedPrimitives();
    }
    else
    {
        outFaceIndices = 0;
    }
    return numFaces;
}

//------------------------------------------------------------------------------
/**
    Do a closest-hit ray check on the shape.
*/
bool
MeshShape::DoRayCheck(const line3& l, vector3& contact)
{
    // get pointer to our opc model
    Opcode::Model* opcModel = dGeomTriMeshGetOpcodeModel(this->odeTriMeshID);
    n_assert(opcModel);

    // setup OPCODE ray
    static IceMaths::Ray opcRay;
    opcRay.mOrig.Set(l.b.x, l.b.y, l.b.z);
    opcRay.mDir.Set(l.m.x, l.m.y, l.m.z);

    // setup ray collider
    opcRayCollider.SetMaxDist(l.m.len());

    // get matrix for our model
    IceMaths::Matrix4x4* opcMatrix = (IceMaths::Matrix4x4*) &(this->transform);

    // do ray check
    opcRayCollider.Collide(opcRay, *opcModel, opcMatrix);
    if (opcRayCollider.GetContactStatus())
    {
        n_assert(opcFaces.GetNbFaces() == 1);
        const Opcode::CollisionFace* face = opcFaces.GetFaces();
        contact = l.ipol(face->mDistance);
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    Render debug visualization of the mesh.
*/
void
MeshShape::RenderDebug(const matrix44& t)
{
    if (this->IsAttached())
    {
        matrix44 m = this->GetTransform() * t;
        nGfxServer2::Instance()->DrawShapeIndexedPrimitives(nGfxServer2::TriangleList,
                                                            this->numIndices / 3,
                                                            (vector3*) this->vertexBuffer,
                                                            this->numVertices,
                                                            this->vertexWidth,
                                                            this->indexBuffer,
                                                            nGfxServer2::Index32,
                                                            m,
                                                            this->GetDebugVisualizationColor());
    }
}

} // namespace Physics
