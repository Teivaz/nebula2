#define N_IMPLEMENTS nMesh2
//------------------------------------------------------------------------------
//	nmesh2_main.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gfx2/nmesh2.h"
#include "kernel/nkernelserver.h"
#include "gfx2/nwireframerenderer.h"

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
    primType(TRIANGLELIST),
    groups(0),
    vertexBufferByteSize(0),
    indexBufferByteSize(0)
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
nMesh2::Unload()
{
    if (this->groups)
    {
        delete[] this->groups;
        this->groups = 0;
    }
    this->numVertices = 0;
    this->numIndices  = 0;
    this->numGroups   = 0;
    this->vertexWidth = 0;
    this->primType    = TRIANGLELIST;
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

//------------------------------------------------------------------------------
/**
    Render the mesh as wireframe in debug visualization mode.
*/
void
nMesh2::RenderWireframe(nGfxServer2* gfxServer, nVariableServer* varServer, const char* shaderName, int groupIndex)
{
    n_assert(this->GetPrimitiveType() == TRIANGLELIST);

    float* vertexPtr = this->LockVertices();
    ushort* indexPtr = this->LockIndices();
    n_assert(vertexPtr && indexPtr);

    nWireframeRenderer renderer(gfxServer, varServer, shaderName);
    const int indexStart = this->GetGroup(groupIndex).GetFirstIndex();
    const int indexEnd = indexStart + this->GetGroup(groupIndex).GetNumIndices();
    const vector4 triColor(0.0f, 1.0f, 0.0f, 1.0f);
    const vector4 normalColor(1.0f, 0.0f, 0.0f, 1.0f);
    const vector4 tangentColor(1.0f, 1.0f, 0.0f, 1.0f);
    const vector4 binormalColor(0.0f, 0.0f, 1.0f, 1.0f);
    const float normalLength = 0.025f;
    
    // render triangles
    if ((TRIANGLELIST == this->primType) && (Coord & this->vertexComponentMask))
    {
        vector3 v0, v1, v2;

        renderer.Begin(triColor.x, triColor.y, triColor.z, triColor.w);
        int i;
        for (i = indexStart; i < indexEnd;)
        {
            float* p0 = vertexPtr + (indexPtr[i++] * this->vertexWidth);
            v0.set(p0[0], p0[1], p0[2]);
            float* p1 = vertexPtr + (indexPtr[i++] * this->vertexWidth);
            v1.set(p1[0], p1[1], p1[2]);
            float* p2 = vertexPtr + (indexPtr[i++] * this->vertexWidth);
            v2.set(p2[0], p2[1], p2[2]);

            renderer.Line(v0, v1);
            renderer.Line(v1, v2);
            renderer.Line(v2, v0);
        }
        renderer.End();
    }

/*
    // render normals
    if ((Coord|Normal) == ((Coord|Normal) & this->vertexComponentMask))
    {
        vector3 pos, normal;
        renderer.Begin(normalColor.x, normalColor.y, normalColor.z, normalColor.w);
        int i;
        for (i = indexStart; i < indexEnd; i++)
        {
            float* v = vertexPtr + (indexPtr[i] * this->vertexWidth);
            pos.set(v[0], v[1], v[2]);      // read position of current vertex
            normal.set(v[3], v[4], v[5]);   // read normal of current vertex
            renderer.Line(pos, pos + (normal * normalLength));
        }
        renderer.End();
    }

    // render tangents
    if ((Coord | Tangent) == ((Coord | Tangent) & this->vertexComponentMask))
    {
        vector3 pos, tangent;
        renderer.Begin(tangentColor.x, tangentColor.y, tangentColor.z, tangentColor.w);
        int i;
        for (i = indexStart; i < indexEnd; i++)
        {
            float* v = vertexPtr + (indexPtr[i] * this->vertexWidth);
            pos.set(v[0], v[1], v[2]);      // read position of current vertex
            tangent.set(v[6], v[7], v[8]);  // read tangent of current vertex
            renderer.Line(pos, pos + (tangent * normalLength));
        }
        renderer.End();
    }

    // render binormals
    if ((Coord|Normal|Tangent) == ((Coord|Normal|Tangent) & this->vertexComponentMask))
    {
        vector3 pos, normal, tangent, binormal;
        renderer.Begin(binormalColor.x, binormalColor.y, binormalColor.z, binormalColor.w);
        int i;
        for (i = indexStart; i < indexEnd; i++)
        {
            float* v = vertexPtr + (indexPtr[i] * this->vertexWidth);
            pos.set(v[0], v[1], v[2]);      // read position of current vertex
            normal.set(v[3], v[4], v[5]);   // read normal of current vertex
            tangent.set(v[6], v[7], v[8]);  // read tangent of current vertex
            binormal = normal * tangent;

            renderer.Line(pos, pos + (binormal * normalLength));
        }
        renderer.End();
    }
*/
}

