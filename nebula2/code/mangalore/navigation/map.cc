//------------------------------------------------------------------------------
//  navigation/map.cc
//  (C) 2005 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "navigation/map.h"
#include "gfx2/nn3d2loader.h"
#include "gfx2/nnvx2loader.h"
#include "foundation/server.h"

namespace Navigation
{
ImplementRtti(Navigation::Map, Foundation::RefCounted);
ImplementFactory(Navigation::Map);

//------------------------------------------------------------------------------
/**
*/
Map::Node::Node():
    position(0.0f, 0.0f, 0.0f),
    neighbors()
{
}

//------------------------------------------------------------------------------
/**
*/
void
Map::Node::AddNeighbor(uint index)
{
    if (0 == this->neighbors.Find(index))
    {
        this->neighbors.Append(index);
    }
}

//------------------------------------------------------------------------------
/**
*/
Map::Map():
    isOpen(false)
{
}

//------------------------------------------------------------------------------
/**
*/
bool
Map::Open(const nString& fname)
{
    n_assert(fname.IsValid());

    if (this->isOpen)
    {
        this->Close();
    }

    this->filename = fname;

    nMeshLoader* meshLoader = 0;
    if (this->filename.CheckExtension("n3d2"))
    {
        meshLoader = n_new(nN3d2Loader);
    }
    else if (this->filename.CheckExtension("nvx2"))
    {
        meshLoader = n_new(nNvx2Loader);
    }
    else
    {
        n_error("Map: No loader for navigation mesh '%s'!", this->filename.Get());
    }

    meshLoader->SetFilename(this->filename.Get());
    meshLoader->SetIndexType(nMeshLoader::Index16); //TODO: what's this? Floh: select 16 or 32 bit indices

    if (!meshLoader->Open())
    {
        n_error("Map: Failed to open file '%s'!", this->filename.Get());
        return false;
    }

    // transfer mesh attributes
    int numVertices      = meshLoader->GetNumVertices();
    int numIndices       = meshLoader->GetNumIndices();
    int numGroups        = meshLoader->GetNumGroups();
    int vertexWidth      = meshLoader->GetVertexWidth();

    // allocate vertex and index buffer
    size_t vbSize = numVertices * vertexWidth * sizeof(float);
    size_t ibSize = numIndices * sizeof(ushort);
    float* vertexBuffer = (float*) n_malloc(vbSize);
    ushort* indexBuffer = (ushort*) n_malloc(ibSize);

    // read vertices and indices
    meshLoader->ReadVertices(vertexBuffer, vbSize);
    meshLoader->ReadIndices(indexBuffer, ibSize);

    // destroy mesh loader
    meshLoader->Close();
    n_delete(meshLoader);
    
    // build temporary corner and edge buffers
    vector3* vertices   = n_new_array(vector3, numVertices);
    Corner* corners     = n_new_array(Corner, numIndices);
    Edge* edges         = n_new_array(Edge, numIndices);
    Triangle* triangles = n_new_array(Triangle, numIndices / 3);

    // vertex positions
    for (int vIndex = 0; vIndex < numVertices; ++vIndex)
    {
        const float* fp = vertexBuffer + vIndex * vertexWidth;

        vector3* vertex = vertices + vIndex;
        vertex->x = fp[0];
        vertex->y = fp[1];
        vertex->z = fp[2];
    }

    // temporary mesh structs (triangles, edges, corners)
    n_assert(numIndices % 3 == 0);
    for (int tIndex = 0; tIndex < (numIndices / 3); ++tIndex)
    {
        const ushort* tri = indexBuffer + 3 * tIndex;

        Triangle* triangle = triangles + tIndex;

        for (int i = 0; i < 3; ++i)
        {
            Corner* corner = corners + 3 * tIndex + i;
            corner->vertex      = tri[i];
            corner->face        = triangle;
            corner->cornerIndex = i;
            corner->index       = -1;

            Edge* edge = edges + 3 * tIndex + i;
            edge->vertices[0] = min(tri[i], tri[(i + 1) % 3]);
            edge->vertices[1] = max(tri[i], tri[(i + 1) % 3]);
            edge->face        = triangle;
            edge->edgeIndex   = i;
            edge->index       = -1;

            triangle->corners[i] = 0;
            triangle->edges[i]   = 0;
        }
    }

    // free vertex format specific buffers from mesh loader
    n_free(vertexBuffer);
    n_free(indexBuffer);

    // sort corner and edge array
    qsort(corners, numIndices, sizeof(Corner), Map::CornerCompare);
    qsort(edges,   numIndices, sizeof(Edge),   Map::EdgeCompare);

    // build face -> edge/corner links
    for (int i = 0; i < numIndices; ++i) {
        Corner* corner = corners + i;
        corner->face->corners[corner->cornerIndex] = corner;

        Edge* edge = edges + i;
        edge->face->edges[edge->edgeIndex] = edge;
    }

    // validate
    for (int i = 0; i < (numIndices / 3); ++i) {
        Triangle* triangle = triangles + i;

        for (int j = 0; j < 3; ++j) {
            n_assert(0 != triangle->corners[j] && 0 != triangle->edges[j]);
        }
    }

    // find valid triangle edges
    int eIndex = 0;
    while (eIndex < numIndices - 1)
    {
        Edge* edge0 = edges + eIndex;
        Edge* edge1 = edges + eIndex + 1;

        // shared edge
        if (edge0->vertices[0] == edge1->vertices[0] && edge0->vertices[1] == edge1->vertices[1])
        {
            // create node
            int nodeIndex = this->nodes.Size();
            Node& node = this->nodes.At(nodeIndex);
            node.position = (vertices[edge0->vertices[0]] + vertices[edge0->vertices[1]]) * 0.5f;

            // save node index in edge structs
            edge0->index = nodeIndex;
            edge1->index = nodeIndex;

            n_assert(edge0->face->edges[edge0->edgeIndex] == edge0);
            n_assert(edge1->face->edges[edge1->edgeIndex] == edge1);

            // connect to other edges of adjacent triangles
            this->Connect(nodeIndex, edge0->face->edges[(edge0->edgeIndex + 1) % 3]->index);
            this->Connect(nodeIndex, edge0->face->edges[(edge0->edgeIndex + 2) % 3]->index);
            this->Connect(nodeIndex, edge1->face->edges[(edge1->edgeIndex + 1) % 3]->index);
            this->Connect(nodeIndex, edge1->face->edges[(edge1->edgeIndex + 2) % 3]->index);

            ++eIndex; // skip second triangle
        }

        ++eIndex;
    }

#if 1
    // find valid triangle corners
    int cIndex = 0;
    while (cIndex < numIndices)
    {
        ushort vertex = corners[cIndex].vertex;
        bool valid    = true;

        int curIndex, lastIndex;

        // check if adjacent edges are shared
        curIndex = cIndex;
        while (cIndex < numIndices && corners[curIndex].vertex == vertex)
        {
            Corner* corner = corners + curIndex;

            valid &= corner->face->edges[(corner->cornerIndex + 0) % 3]->index != -1;
            // assuming that all faces have the same vertex order (CW/CCW)
            // valid &= corner->face->edges[(corner->cornerIndex + 2) % 3]->index != -1;

            ++curIndex;
        }
        lastIndex = curIndex;

        // triangle corner inside mesh
        if (valid)
        {
            // create node
            int nodeIndex = this->nodes.Size();
            Node& node = this->nodes.At(nodeIndex);
            node.position = vertices[vertex];

            // process adjacent faces
            for (curIndex = cIndex; curIndex < lastIndex; ++curIndex)
            {
                Corner* corner = corners + curIndex;

                // mark corner
                corner->index = nodeIndex;

                n_assert(corner->face->edges[(corner->cornerIndex + 0) % 3]->index != -1);
                n_assert(corner->face->edges[(corner->cornerIndex + 2) % 3]->index != -1);

                // connect to face edge midpoint nodes
                this->Connect(nodeIndex, corner->face->edges[(corner->cornerIndex + 0) % 3]->index);
                // assuming that all faces have the same vertex order (CW/CCW)
                // this->Connect(nodeIndex, corner->face->edges[(corner->cornerIndex + 2) % 3]->index);
            }
        }

        cIndex = lastIndex;
    }
#endif

    for (int i = 0; i < this->nodes.Size(); ++i)
    {
        n_assert(this->nodes[i].neighbors.Size() > 0);
    }

    n_delete(vertices);
    n_delete(corners);
    n_delete(edges);
    n_delete(triangles);

    this->isOpen = true;
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
Map::Close()
{
    n_assert(this->isOpen);

    this->filename.Clear();
    this->nodes.Empty();
    this->isOpen = false;
};

//------------------------------------------------------------------------------
/**
*/
void
Map::Connect(int node0, int node1)
{
    if (node0 != -1 && node1 != -1)
    {
        this->nodes[node0].AddNeighbor(node1);
        this->nodes[node1].AddNeighbor(node0);
    }
}

//------------------------------------------------------------------------------
/**
    qsort() hook - sort tempEdges by vertex indicies

    sort by:
    - vertex index
*/
int
__cdecl
Map::CornerCompare(const void* a, const void* b)
{
    const Corner* c0 = (const Corner*)a;
    const Corner* c1 = (const Corner*)b;

    return c0->vertex - c1->vertex;
}

//------------------------------------------------------------------------------
/**
    qsort() hook - sort tempEdges by vertex indicies

    sort by:
    - 1st vertex index
    - 2nd vertex index
*/
int
__cdecl
Map::EdgeCompare(const void* a, const void* b)
{
    const Edge* e0 = (const Edge*)a;
    const Edge* e1 = (const Edge*)b;

    int delta = e0->vertices[0] - e1->vertices[0];
    if (0 == delta)
    {
        delta = e0->vertices[1] - e1->vertices[1];
    }

    return delta;
}

//------------------------------------------------------------------------------
/**
*/
void
Map::RenderDebug()
{
    nGfxServer2* gfxServer = nGfxServer2::Instance();

    gfxServer->BeginShapes();

    for (int i = 0; i < this->nodes.Size(); ++i)
    {
        matrix44 transform;
        transform.scale(vector3(0.1f, 0.1f, 0.1f));
        transform.translate(this->nodes[i].position);
        transform.translate(vector3(0.0f, 0.2f, 0.0f));
		gfxServer->DrawShape(nGfxServer2::Sphere, transform, vector4(0.3f, 0.7f, 0.3f, 0.3f));
    }

	gfxServer->EndShapes();

}

} // namespace Navigation