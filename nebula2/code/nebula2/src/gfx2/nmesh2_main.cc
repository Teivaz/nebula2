//------------------------------------------------------------------------------
//  nmesh2_main.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gfx2/nmesh2.h"
#include "kernel/nkernelserver.h"

nNebulaClass(nMesh2, "nresource");

//------------------------------------------------------------------------------
/**
*/
nMesh2::nMesh2() :
    usage(WriteOnce),
    vertexComponentMask(0),
    vertexWidth(0),
    numVertices(0),
    numIndices(0),
    numGroups(0),
    groups(0),
    vertexBufferByteSize(0),
    indexBufferByteSize(0),
    refillBuffersMode(Disabled)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nMesh2::~nMesh2()
{
    if (this->IsValid())
    {
        this->Unload();
    }
}

//------------------------------------------------------------------------------
/**
    Unload everything.
*/
void
nMesh2::UnloadResource()
{
    if (this->groups)
    {
        delete[] this->groups;
        this->groups = 0;
    }

    // NOTE: do not clear numVertices, numIndices, vertexWidth, 
    // etc. Those values may be needed in a following call to Load()!
    this->vertexBufferByteSize = 0;
    this->indexBufferByteSize  = 0;
}

//------------------------------------------------------------------------------
/**
    Lock vertices in mesh. Return pointer to beginning of vertex buffer.
*/
float*
nMesh2::LockVertices()
{
    // empty
    return 0;
}

//------------------------------------------------------------------------------
/**
    Unlock vertices in mesh.
*/
void
nMesh2::UnlockVertices()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Lock indices in mesh. Return pointer to start of indices.
*/
ushort*
nMesh2::LockIndices()
{
    // empty
    return 0;
}

//------------------------------------------------------------------------------
/**
    Unlock indices in mesh.
*/
void
nMesh2::UnlockIndices()
{
    // empty
}
