#ifndef N_MESHGROUP_H
#define N_MESHGROUP_H
//------------------------------------------------------------------------------
/**
    A nMeshGroup object defines a group of vertices and indices inside
    a mesh.
    
    (C) 2002 RadonLabs GmbH
*/
#ifndef N_TYPES_H
#include "kernel/ntypes.h"
#endif

#ifndef N_BBOX_H
#include "mathlib/bbox.h"
#endif

//------------------------------------------------------------------------------
class nMeshGroup
{
public:
    /// constructor
    nMeshGroup();
    /// destructor
    ~nMeshGroup();
    /// set first vertex
    void SetFirstVertex(int i);
    /// get index of first vertex
    int GetFirstVertex() const;
    /// set number of vertices
    void SetNumVertices(int i);
    /// get number of vertices
    int GetNumVertices() const;
    /// set first index
    void SetFirstIndex(int i);
    /// get index of first primitve index
    int GetFirstIndex() const;
    /// set number of indices
    void SetNumIndices(int i);
    /// get number of indices
    int GetNumIndices() const;
    /// set bounding box
    void SetBoundingBox(const bbox3& b);
    /// get bounding box
    const bbox3& GetBoundingBox() const;

private:
    int firstVertex;
    int numVertices;
    int firstIndex;
    int numIndices;
    bbox3 box;
};
    
//------------------------------------------------------------------------------
/**
*/
inline
nMeshGroup::nMeshGroup() :
    firstVertex(0),
    numVertices(0),
    firstIndex(0),
    numIndices(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nMeshGroup::~nMeshGroup()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nMeshGroup::SetFirstVertex(int i)
{
    this->firstVertex = i;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nMeshGroup::GetFirstVertex() const
{
    return this->firstVertex;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nMeshGroup::SetNumVertices(int i)
{
    this->numVertices = i;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nMeshGroup::GetNumVertices() const
{
    return this->numVertices;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nMeshGroup::SetFirstIndex(int i)
{
    this->firstIndex = i;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nMeshGroup::GetFirstIndex() const
{
    return this->firstIndex;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nMeshGroup::SetNumIndices(int i)
{
    this->numIndices = i;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nMeshGroup::GetNumIndices() const
{
    return this->numIndices;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nMeshGroup::SetBoundingBox(const bbox3& b)
{
    this->box = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
const bbox3&
nMeshGroup::GetBoundingBox() const
{
    return this->box;
}

//------------------------------------------------------------------------------
#endif

