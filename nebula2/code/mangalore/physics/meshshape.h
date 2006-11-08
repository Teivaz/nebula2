#ifndef PHYSICS_MESHSHAPE_H
#define PHYSICS_MESHSHAPE_H
//------------------------------------------------------------------------------
/**
    @class Physics::MeshShape

    An shape containing a triangle mesh.

    (C) 2003 RadonLabs GmbH
*/
#include "physics/shape.h"
#include "mathlib/sphere.h"
#include "mathlib/line.h"
#define BAN_OPCODE_AUTOLINK
#include "opcode/Opcode.h"

//------------------------------------------------------------------------------
namespace Physics
{
class MeshShape : public Shape
{
    DeclareRtti;
	DeclareFactory(MeshShape);

public:
    /// constructor
    MeshShape();
    /// destructor
    virtual ~MeshShape();
    /// render debug visualization
    virtual void RenderDebug(const matrix44& t);
    /// attach the shape to the world
    virtual bool Attach(dSpaceID spaceId);
    /// detach the shape from the world
    virtual void Detach();
    /// set the mesh filename
    void SetFilename(const nString& name);
    /// get the mesh filename
    const nString& GetFilename() const;
    /// access to vertex buffer
    const float* GetVertexBuffer() const;
    /// access to index buffer
    const int* GetIndexBuffer() const;
    /// get number of vertices
    int GetNumVertices() const;
    /// get vertex width
    int GetVertexWidth() const;
    /// get number of indices
    int GetNumIndices() const;
    /// do a sphere collide check, returns number of and indices of contained faces
    int DoSphereCollide(const sphere& s, uint*& outFaceIndices);
    /// do a first-hit raycheck
    bool DoRayCheck(const line3& l, vector3& contact);

protected:
    friend class Ray;

    /// get ODE TriMeshDataID
    dTriMeshDataID GetOdeTriMeshDataId() const;
    /// get ODE TriMeshID
    dGeomID GetOdeTriMeshID() const;
    /// load vertices and indices from n3d2 file
    bool LoadN3d2();
    /// load vertices and indices from nvx2 file
    bool LoadNvx2();

    nString filename;
    float* vertexBuffer;
    int* indexBuffer;
    int numVertices;
    int vertexWidth;
    int numIndices;
    dTriMeshDataID odeTriMeshDataId;
    dGeomID odeTriMeshID;
    static Opcode::SphereCollider opcSphereCollider;    // an OPCODE sphere collider
    Opcode::SphereCache opcSphereCache;                 // an OPCODE sphere cache
    static Opcode::RayCollider opcRayCollider;          // an OPCODE ray collider
    static Opcode::CollisionFaces opcFaces;             // face cache for ray collider
};

RegisterFactory(MeshShape);

//------------------------------------------------------------------------------
/**
*/
inline
dTriMeshDataID
MeshShape::GetOdeTriMeshDataId() const
{
    return this->odeTriMeshDataId;
}

//------------------------------------------------------------------------------
/**
*/
inline
dGeomID
MeshShape::GetOdeTriMeshID() const
{
    return this->odeTriMeshID;
}

//------------------------------------------------------------------------------
/**
*/
inline
const float*
MeshShape::GetVertexBuffer() const
{
    return this->vertexBuffer;
}

//------------------------------------------------------------------------------
/**
*/
inline
const int*
MeshShape::GetIndexBuffer() const
{
    return this->indexBuffer;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
MeshShape::GetNumVertices() const
{
    return this->numVertices;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
MeshShape::GetVertexWidth() const
{
    return this->vertexWidth;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
MeshShape::GetNumIndices() const
{
    return this->numIndices;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
MeshShape::SetFilename(const nString& name)
{
    this->filename = name;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
MeshShape::GetFilename() const
{
    return this->filename;
}

} // namespace Physics
//------------------------------------------------------------------------------
#endif
