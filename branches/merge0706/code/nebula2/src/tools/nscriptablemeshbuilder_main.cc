//------------------------------------------------------------------------------
//  nscriptablemeshbuilder_main.cc
//
//  (C)2005 James Mastro / Kim, Hyoun Woo
//------------------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "tools/nscriptablemeshbuilder.h"

nNebulaScriptClass(nScriptableMeshBuilder, "nroot");

//------------------------------------------------------------------------------
/**
*/
nScriptableMeshBuilder::nScriptableMeshBuilder() :
    begin_vadd(false)
{
}

//------------------------------------------------------------------------------
/**
*/
nScriptableMeshBuilder::~nScriptableMeshBuilder()
{
}

//------------------------------------------------------------------------------
/**
    @return vertex index of the added vertex.
*/
int nScriptableMeshBuilder::BeginAddVertex()
{
    n_assert (false == this->begin_vadd);

    nMeshBuilder::Vertex vertex;
    nMeshBuilder::AddVertex(vertex);

    this->begin_vadd = true;

    return (nMeshBuilder::GetNumVertices() - 1);
}

//------------------------------------------------------------------------------
/**
*/
void nScriptableMeshBuilder::AddCoord(int index, float x, float y, float z)
{
    n_assert (this->begin_vadd);

    nMeshBuilder::Vertex& vertex = nMeshBuilder::GetVertexAt(index);
    vertex.SetCoord(vector3(x, y, z));
}

//------------------------------------------------------------------------------
/**
*/
void nScriptableMeshBuilder::AddNormal(int index, float x, float y, float z)
{
    n_assert (this->begin_vadd);

    nMeshBuilder::Vertex& vertex = nMeshBuilder::GetVertexAt(index);
    vertex.SetNormal(vector3(x, y, z));
}

//------------------------------------------------------------------------------
/**
*/
void nScriptableMeshBuilder::AddColor(int index, float r, float g, float b, float a)
{
    n_assert (this->begin_vadd);

    nMeshBuilder::Vertex& vertex = nMeshBuilder::GetVertexAt(index);
    vertex.SetColor(vector4(r, g, b, a));
}

//------------------------------------------------------------------------------
/**
*/
void nScriptableMeshBuilder::AddUv(int index, int layer, float u, float v)
{
    n_assert (this->begin_vadd);

    nMeshBuilder::Vertex& vertex = nMeshBuilder::GetVertexAt(index);
    vertex.SetUv(layer, vector2(u, v));
}

//------------------------------------------------------------------------------
/**
*/
void nScriptableMeshBuilder::AddTangent(int index, float x, float y, float z)
{
    n_assert (this->begin_vadd);

    nMeshBuilder::Vertex& vertex = nMeshBuilder::GetVertexAt(index);
    vertex.SetTangent(vector3(x, y, z));
}

//------------------------------------------------------------------------------
/**
*/
void nScriptableMeshBuilder::AddBinormal(int index, float x, float y, float z)
{
    n_assert (this->begin_vadd);

    nMeshBuilder::Vertex& vertex = nMeshBuilder::GetVertexAt(index);
    vertex.SetBinormal(vector3(x, y, z));
}

//------------------------------------------------------------------------------
/**
*/
void nScriptableMeshBuilder::AddJointIndices(int index, float a, float b, float c, float d)
{
    n_assert (this->begin_vadd);

    nMeshBuilder::Vertex& vertex = nMeshBuilder::GetVertexAt(index);
    vertex.SetJointIndices(vector4(a, b, c, d));
}

//------------------------------------------------------------------------------
/**
*/
void nScriptableMeshBuilder::AddWeights(int index, float a, float b, float c, float d)
{
    n_assert (this->begin_vadd);

    nMeshBuilder::Vertex& vertex = nMeshBuilder::GetVertexAt(index);
    vertex.SetWeights(vector4(a, b, c, d));
}

//------------------------------------------------------------------------------
/**
*/
void nScriptableMeshBuilder::EndAddVertex()
{
    this->begin_vadd = false;
}
//------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------
