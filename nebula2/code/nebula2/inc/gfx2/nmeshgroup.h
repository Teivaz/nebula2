#ifndef N_MESHGROUP_H
#define N_MESHGROUP_H
//------------------------------------------------------------------------------
/**
    @class nMeshGroup
    @ingroup Gfx2

    A nMeshGroup object defines a group of vertices and indices inside
    a mesh.

    (C) 2002 RadonLabs GmbH
*/
#include "kernel/ntypes.h"
#include "mathlib/bbox.h"

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
    /// get index of first primitive index
    int GetFirstIndex() const;
    /// set number of indices
    void SetNumIndices(int i);
    /// get number of indices
    int GetNumIndices() const;
    /// set first edge
    void SetFirstEdge(int i);
    /// get index of first edge
    int GetFirstEdge() const;
    /// set number of edges
    void SetNumEdges(int i);
    /// get number of edges
    int GetNumEdges() const;
    /// set bounding box
    void SetBoundingBox(const bbox3& b);
    /// get bounding box
    const bbox3& GetBoundingBox() const;

private:
    int firstVertex;
    int numVertices;
    int firstIndex;
    int numIndices;
    int firstEdge;
    int numEdges;
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
    numIndices(0),
    firstEdge(0),
    numEdges(0)
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
nMeshGroup::SetFirstEdge(int i)
{
    this->firstEdge = i;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nMeshGroup::GetFirstEdge() const
{
    return this->firstEdge;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nMeshGroup::SetNumEdges(int i)
{
    this->numEdges = i;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nMeshGroup::GetNumEdges() const
{
    return this->numEdges;
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
