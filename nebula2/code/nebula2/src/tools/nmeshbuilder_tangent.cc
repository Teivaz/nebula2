//------------------------------------------------------------------------------
//  nmeshbuilder_tangent.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "tools/nmeshbuilder.h"

//------------------------------------------------------------------------------
/**
    Build triangle normals and tangents. The tangents require a valid 
    uv-mapping in texcoord layer 0. A new mesh reduced to coord and uv0 
    components will be used for the computation to ensure proper vertex sharing 
    between triangles.

    02-Sep-03   floh    no longer generates Binormals
*/
void
nMeshBuilder::BuildTriangleNormals()
{
    // compute face normals and tangents
    int triangleIndex;
    int numTriangles = this->GetNumTriangles();
    vector3 v0, v1;
    vector2 uv0, uv1;
    vector3 n, t, b;
    for (triangleIndex = 0; triangleIndex < numTriangles; triangleIndex++)
    {
        Triangle& tri = this->GetTriangleAt(triangleIndex);
        int index[3];
        tri.GetVertexIndices(index[0], index[1], index[2]);

        const Vertex& vertex0 = this->GetVertexAt(index[0]);
        const Vertex& vertex1 = this->GetVertexAt(index[1]);
        const Vertex& vertex2 = this->GetVertexAt(index[2]);

        // compute the face normal
        v0 = vertex1.GetCoord() - vertex0.GetCoord();
        v1 = vertex2.GetCoord() - vertex0.GetCoord();
        n = v0 * v1;
        n.norm();
        tri.SetNormal(n);

        // compute the tangents
        uv0 = vertex1.GetUv(0) - vertex0.GetUv(0);
        uv1 = vertex2.GetUv(0) - vertex0.GetUv(0);
        t = (v0 * uv1.y) - (v1 * uv0.y);
        // b = (v0 * uv1.x) - (v1 * uv0.x);
        t.norm();
        // b.norm();
        tri.SetTangent(t);
        // tri.SetBinormal(b);
    }
}

//------------------------------------------------------------------------------
/**
    Generates the per-vertex tangents by averaging the
    per-triangle tangents and binormals which must be computed
    beforehand. Note that the vertex normals will not be touched!
    Internally, the method will create a clean mesh which contains
    only vertex coordinates and normals, and computes connectivity
    information from the resulting mesh. The result is that 
    tangents and binormals are averaged for smooth edges, as defined
    by the existing normal set.

    02-Sep-03   floh    no longer generates Binormals
*/
void
nMeshBuilder::BuildVertexTangents()
{
    // create a clean coord/normal-only mesh, record the cleanup operation
    // in a collaps map so that we can inflate-copy the new vertex
    // components into the original mesh afterwards
    nArray< nArray<int> > collapsMap(0, 0);
    collapsMap.SetFixedSize(this->GetNumVertices());
    nMeshBuilder cleanMesh = *this;
    cleanMesh.ForceVertexComponents(Vertex::COORD | Vertex::NORMAL);
    cleanMesh.Cleanup(&collapsMap);

    // create a connectivity map which contains for each vertex
    // the triangle indices which share the vertex
    nArray< nArray<int> > vertexTriangleMap(0, 0);
    cleanMesh.BuildVertexTriangleMap(vertexTriangleMap);

    // for each vertex...
    int vertexIndex = 0;
    int numVertices = cleanMesh.GetNumVertices();
    vector3 avgTangent;
    for (vertexIndex = 0; vertexIndex < numVertices; vertexIndex++)
    {
        avgTangent.set(0.0f, 0.0f, 0.0f);
        // avgBinormal.set(0.0f, 0.0f, 0.0f);

        // for each triangle sharing this vertex...
        int numVertexTris = vertexTriangleMap[vertexIndex].Size();
        n_assert(numVertexTris > 0);
        int vertexTriIndex;
        for (vertexTriIndex = 0; vertexTriIndex < numVertexTris; vertexTriIndex++)
        {
            const Triangle& tri = cleanMesh.GetTriangleAt(vertexTriangleMap[vertexIndex][vertexTriIndex]);
            avgTangent += tri.GetTangent();
            // avgBinormal += tri.GetBinormal();
        }

        // renormalize averaged tangent and binormal
        avgTangent.norm();
        // avgBinormal.norm();

        cleanMesh.GetVertexAt(vertexIndex).SetTangent(avgTangent);
        // cleanMesh.GetVertexAt(vertexIndex).SetBinormal(avgBinormal);
    }

    // inflate-copy the generated vertex tangents and binormals to the original mesh
    this->InflateCopyComponents(cleanMesh, collapsMap, Vertex::TANGENT);
}
