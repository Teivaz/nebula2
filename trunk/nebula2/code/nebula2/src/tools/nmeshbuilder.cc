//------------------------------------------------------------------------------
//  nmeshbuilder.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "tools/nmeshbuilder.h"

//------------------------------------------------------------------------------
/**
*/
nMeshBuilder::nMeshBuilder() :
    vertexArray((1<<16), (1<<16)),
    triangleArray((1<<16), (1<<16)),
    groupArray(66, 64)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nMeshBuilder::~nMeshBuilder()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Transform vertices in mesh, affects coordinates, normals and tangents.
*/
void
nMeshBuilder::Transform(const matrix44& m44)
{
    // build a normal transformation matrix (rotation only)
    vector3 xrot = m44.x_component();
    vector3 yrot = m44.y_component();
    vector3 zrot = m44.z_component();
    xrot.norm();
    yrot.norm();
    zrot.norm();
    matrix33 m33(xrot, yrot, zrot);

    vector3 v;
    int num = this->vertexArray.Size();
    int i;

    // transform coordinates
    for (i = 0; i < num; i++)
    {
        Vertex& vertex = this->vertexArray[i];

        if (vertex.HasComponent(Vertex::COORD))
        {
            vertex.coord = m44 * vertex.coord;
        }
        if (vertex.HasComponent(Vertex::NORMAL))
        {
            vertex.normal = m33 * vertex.normal;
            vertex.normal.norm();
        }
        if (vertex.HasComponent(Vertex::TANGENT))
        {
            vertex.tangent = m33 * vertex.tangent;
            vertex.tangent.norm();
        }
    }
}

//------------------------------------------------------------------------------
/**
    qsort() hook for Cleanup() method.
*/
nMeshBuilder* nMeshBuilder::qsortData = 0;

int
__cdecl
nMeshBuilder::VertexSorter(const void* elm0, const void* elm1)
{
    nMeshBuilder* meshBuilder = qsortData;
    int i0 = *(int*)elm0;
    int i1 = *(int*)elm1;
    Vertex& v0 = meshBuilder->GetVertexAt(i0);
    Vertex& v1 = meshBuilder->GetVertexAt(i1);
    return v0.Compare(v1);
}

//------------------------------------------------------------------------------
/**
    Cleanup the mesh. This removes redundant vertices and optionally record
    the collaps history into a client-provided collapsMap. The collaps map
    contains at each new vertex index the 'old' vertex indices which have
    been collapsed into the new vertex.

    30-Jan-03   floh    optimizations
*/
void
nMeshBuilder::Cleanup(nArray< nArray<int> >* collapsMap)
{
    int numVertices = this->vertexArray.Size();

    // generate a index remapping table and sorted vertex array
    int* indexMap = new int[numVertices];
    int* sortMap  = new int[numVertices];
    int* shiftMap = new int[numVertices];
    int i;
    for (i = 0; i < numVertices; i++)
    {
        indexMap[i] = i;
        sortMap[i]  = i;
    }

    // generate a sorted index map (sort by X coordinate)
    qsortData = this;
    qsort(sortMap, numVertices, sizeof(int), nMeshBuilder::VertexSorter);    

    // search sorted array for redundant vertices
    int baseIndex = 0;
    for (baseIndex = 0; baseIndex < (numVertices - 1);)
    {
        int nextIndex = baseIndex + 1;
        while ((nextIndex < numVertices) && 
               (this->vertexArray[sortMap[baseIndex]] == this->vertexArray[sortMap[nextIndex]]))
        {
            // mark the vertex as invalid
            this->vertexArray[sortMap[nextIndex]].SetFlag(Vertex::REDUNDANT);

            // put the new valid index into the index remapping table
            indexMap[sortMap[nextIndex]] = sortMap[baseIndex];
            nextIndex++;
        }
        baseIndex = nextIndex;
    }

    // fill the shiftMap, this contains for each vertex index the number
    // of invalid vertices in front of it
    int numInvalid = 0;
    int vertexIndex;
    for (vertexIndex = 0; vertexIndex < numVertices; vertexIndex++)
    {
        if (this->vertexArray[vertexIndex].CheckFlag(Vertex::REDUNDANT))
        {
            numInvalid++;
        }
        shiftMap[vertexIndex] = numInvalid;
    }

    // fix the triangle's vertex indices, first, remap the old index to a
    // valid index from the indexMap, then decrement by the shiftMap entry
    // at that index (which contains the number of invalid vertices in front
    // of that index)
    // fix vertex indices in triangles
    int numTriangles = this->triangleArray.Size();
    int curTriangle;
    for (curTriangle = 0; curTriangle < numTriangles; curTriangle++)
    {
        Triangle& t = this->triangleArray[curTriangle];
        for (i = 0; i < 3; i++)
        {
            int newIndex = indexMap[t.vertexIndex[i]];
            t.vertexIndex[i] = newIndex - shiftMap[newIndex];
        }
    }

    // initialize the collaps map so that for each new (collapsed)
    // index it contains a list of old vertex indices which have been
    // collapsed into the new vertex 
    if (collapsMap)
    {
        collapsMap->Clear();
        for (i = 0; i < numVertices; i++)
        {
            int newIndex = indexMap[i];
            int collapsedIndex = newIndex - shiftMap[newIndex];
            collapsMap->At(collapsedIndex).PushBack(i);
        }
    }

	// finally, remove the redundant vertices
    numVertices = this->vertexArray.Size();
    nArray<Vertex> newArray(numVertices, numVertices);
    for (vertexIndex = 0; vertexIndex < numVertices; vertexIndex++)
    {
        if (!this->vertexArray[vertexIndex].CheckFlag(Vertex::REDUNDANT))
        {
            newArray.PushBack(vertexArray[vertexIndex]);
        }
    }
    this->vertexArray = newArray;

    // cleanup
    delete[] indexMap;
    delete[] sortMap;
    delete[] shiftMap;
}

//------------------------------------------------------------------------------
/**
    qsort() hook for PackTrianglesByGroup().
*/
int
__cdecl
nMeshBuilder::TriangleGroupSorter(const void* elm0, const void* elm1)
{
    Triangle* t0 = (Triangle*) elm0;
    Triangle* t1 = (Triangle*) elm1;
    return t0->GetGroupId() - t1->GetGroupId();
}

//------------------------------------------------------------------------------
/**
    Sort triangles by group.
*/
void
nMeshBuilder::PackTrianglesByGroup()
{
    // first, sort triangles by their group id
    qsort(&(this->triangleArray[0]), this->triangleArray.Size(), sizeof(Triangle), nMeshBuilder::TriangleGroupSorter);

    // fix values
    int groupIndex;
    for (groupIndex = 0; groupIndex < this->GetNumGroups(); groupIndex++)
    {
        Group& group = this->GetGroupAt(groupIndex);
        int groupId = group.GetId();
        group.SetFirstTriangle(this->GetFirstTriangleInGroup(groupId));
        group.SetNumTriangles(this->GetNumTrianglesInGroup(groupId));
    }
}

//------------------------------------------------------------------------------
/**
    Build triangle neighbour data.
*/
void
nMeshBuilder::BuildAdjacency()
{
    // TODO!
}

//------------------------------------------------------------------------------
/**
    Optimize mesh for better HT&L cache locality.
*/
void
nMeshBuilder::Optimize()
{
    // TODO!
}

//------------------------------------------------------------------------------
/**
    Append mesh to this. Note: the group id's and group names are not touched! 
    It is advisable to fix them in the source mesh before appending. But this 
    may not always be desired. 
*/
void
nMeshBuilder::Append(const nMeshBuilder& source)
{
    int baseVertexIndex = this->GetNumVertices();
    int baseTriangleIndex = this->GetNumTriangles();

    // add vertices
    int numVertices = source.GetNumVertices();
    int vertexIndex;
    for (vertexIndex = 0; vertexIndex < numVertices; vertexIndex++)
    {
        this->AddVertex(source.GetVertexAt(vertexIndex));
    }

    // add triangles
    Triangle triangle;
    int numTriangles = source.GetNumTriangles();
    int triangleIndex;
    for (triangleIndex = 0; triangleIndex < numTriangles; triangleIndex++)
    {
        triangle = source.GetTriangleAt(triangleIndex);

        // fix vertex indices
        triangle.vertexIndex[0] += baseVertexIndex;
        triangle.vertexIndex[1] += baseVertexIndex;
        triangle.vertexIndex[2] += baseVertexIndex;

        // fix neighbor id's
        if (triangle.HasComponent(Triangle::NEIGHBOURINDICES))
        {
            triangle.neighbourIndex[0] += baseTriangleIndex;
            triangle.neighbourIndex[1] += baseTriangleIndex;
            triangle.neighbourIndex[2] += baseTriangleIndex;
        }
        this->AddTriangle(triangle);
    }

    // add groups
    Group group;
    int numGroups = source.GetNumGroups();
    int groupIndex;
    for (groupIndex = 0; groupIndex < numGroups; groupIndex++)
    {
        group = source.GetGroupAt(groupIndex);
        group.SetFirstTriangle(group.GetFirstTriangle() + baseTriangleIndex);
        this->AddGroup(group);
    }
}

//------------------------------------------------------------------------------
/**
    Replace content of this mesh with source mesh.
*/
void
nMeshBuilder::Copy(const nMeshBuilder& source)
{
    this->vertexArray         = source.vertexArray;
    this->triangleArray       = source.triangleArray;
    this->groupArray          = source.groupArray;
}

//------------------------------------------------------------------------------
/**
    Fill a nMeshBuilder object with extracted group data. Note: this leaves
    the group id's intact!
*/
void
nMeshBuilder::ExtractGroup(int groupIndex, nMeshBuilder& dest)
{
    n_assert(dest.GetNumVertices() == 0);
    n_assert(dest.GetNumTriangles() == 0);
    n_assert(dest.GetNumGroups() == 0);

    Group group = this->GetGroupAt(groupIndex);

    // a remap table for each possible vertex in the group
    int numVertices = this->GetNumVertices();
    int* remap = new int[numVertices];
    int curVertex;
    for (curVertex = 0; curVertex < numVertices; curVertex++)
    {
        remap[curVertex] = -1;
    }

    // transfer triangles and vertices
    int srcTriangleIndex;
    int curVertexIndex = 0;
    for (srcTriangleIndex = group.GetFirstTriangle(); 
         srcTriangleIndex < (group.GetFirstTriangle() + group.GetNumTriangles());
         srcTriangleIndex++)
    {
        Triangle tri = this->GetTriangleAt(srcTriangleIndex);
        int i[3];
        tri.GetVertexIndices(i[0], i[1], i[2]);

        // check if vertices already exist in target object, if
        // not, add vertices, and update remap table
        int triPoint;
        for (triPoint = 0; triPoint < 3; triPoint++)
        {
            if (-1 == remap[i[triPoint]])
            {
                remap[i[triPoint]] = curVertexIndex++;
                dest.AddVertex(this->GetVertexAt(i[triPoint]));
            }
        }

        // modify triangle attributes and add to destination object
        tri.SetVertexIndices(remap[i[0]], remap[i[1]], remap[i[2]]);
        dest.AddTriangle(tri);
    }

    // modify group and add to destination object
    group.SetFirstTriangle(0);
    dest.AddGroup(group);

    // cleanup
    delete remap;
}

//------------------------------------------------------------------------------
/**
    Get the smallest vertex index referenced by the triangles in a group.

    @param  groupId             [in] group id of group to check
    @param  minVertexIndex      [out] filled with minimal vertex index
    @param  maxVertexIndex      [out] filled with maximal vertex index
*/
bool
nMeshBuilder::GetGroupVertexRange(int groupId, int& minVertexIndex, int& maxVertexIndex)
{
    const Group* group = this->GetGroupById(groupId);
    if (group)
    {
        minVertexIndex = this->GetNumVertices();
        maxVertexIndex = 0;
        int curTri;
	    for (curTri = group->GetFirstTriangle(); curTri < group->GetNumTriangles() + group->GetFirstTriangle(); curTri++)
        {
            const Triangle tri = this->GetTriangleAt(curTri);
            int i;
            int vertexIndex[3];
            tri.GetVertexIndices(vertexIndex[0], vertexIndex[1], vertexIndex[2]);

            for (i = 0; i < 3; i++)
            {
                if (vertexIndex[i] < minVertexIndex)
                {
                    minVertexIndex = vertexIndex[i];
                }
                if (vertexIndex[i] > maxVertexIndex)
                {
                    maxVertexIndex = vertexIndex[i];
                }
            }
        }
        return true;
    }
    else
    {
        minVertexIndex = 0;
        maxVertexIndex = 0;
        return false;
    }
}

//------------------------------------------------------------------------------
/**
    This method will erase or create empty vertex component arrays.
    New vertex component arrays will always be set to zeros.
    The method can be used to make sure that a mesh file has the same
    vertex size as expected by a vertex shader program.
*/
void
nMeshBuilder::ForceVertexComponents(int wantedMask)
{
    int hasMask = this->GetVertexAt(0).GetComponentMask();
    int numVertices = this->GetNumVertices();
    int curCompIndex;
    for (curCompIndex = 0; curCompIndex < Vertex::NUM_VERTEX_COMPONENTS; curCompIndex++)
    {
        int curMask = (1<<curCompIndex);

        if ((hasMask & curMask) && (!(wantedMask & curMask)))
        {
            // delete the component array
            int curVertex;
            for (curVertex = 0; curVertex < numVertices; curVertex++)
            {
                this->GetVertexAt(curVertex).DelComponent((Vertex::Component) curMask);
            }
        }
        else if ((!(hasMask & curMask)) && (wantedMask & curMask))
        {
            int curVertex;
            for (curVertex = 0; curVertex < numVertices; curVertex++)
            {
                this->GetVertexAt(curVertex).ZeroComponent((Vertex::Component) curMask);
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
    Does an inflated component copy from a cleaned up source mesh to 
    this mesh.
*/
void
nMeshBuilder::InflateCopyComponents(const nMeshBuilder& src, const nArray< nArray<int> >& collapsMap, int compMask)
{
    int srcIndex;
    int srcNum = src.GetNumVertices();
    for (srcIndex = 0; srcIndex < srcNum; srcIndex++)
    {
        const Vertex& srcVertex = src.GetVertexAt(srcIndex);
        int dstIndex;
        int dstNum = collapsMap[srcIndex].Size();
        for (dstIndex = 0; dstIndex < dstNum; dstIndex++)
        {
            Vertex& dstVertex = this->GetVertexAt(collapsMap[srcIndex][dstIndex]);
            if (compMask & Vertex::COORD)
            {
                dstVertex.SetCoord(srcVertex.GetCoord());
            }
            if (compMask & Vertex::NORMAL)
            {
                dstVertex.SetNormal(srcVertex.GetNormal());
            }
            if (compMask & Vertex::TANGENT)
            {
                dstVertex.SetTangent(srcVertex.GetTangent());
            }
            if (compMask & Vertex::COLOR)
            {
                dstVertex.SetColor(srcVertex.GetColor());
            }
            if (compMask & Vertex::UV0)
            {
                dstVertex.SetUv(0, srcVertex.GetUv(0));
            }
            if (compMask & Vertex::UV1)
            {
                dstVertex.SetUv(1, srcVertex.GetUv(1));
            }
            if (compMask & Vertex::UV2)
            {
                dstVertex.SetUv(2, srcVertex.GetUv(2));
            }
            if (compMask & Vertex::UV3)
            {
                dstVertex.SetUv(3, srcVertex.GetUv(3));
            }
            if (compMask & Vertex::WEIGHTS)
            {
                dstVertex.SetWeights(srcVertex.GetWeights());
            }
            if (compMask & Vertex::JINDICES)
            {
                dstVertex.SetJointIndices(srcVertex.GetJointIndices());
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
    Compute the bounding box of the mesh.
*/
bbox3
nMeshBuilder::ComputeBBox() const
{
    bbox3 box;
    box.begin_extend();
    int numVertices = this->GetNumVertices();
    int vertexIndex;
    for (vertexIndex = 0; vertexIndex < numVertices; vertexIndex++)
    {
        box.extend(this->GetVertexAt(vertexIndex).GetCoord());
    }
    return box;
}

//------------------------------------------------------------------------------
/**
    Count number of vertices inside bounding box.
*/
int
nMeshBuilder::CountVerticesInBBox(const bbox3& box) const
{
    int numVertices = this->GetNumVertices();
    int vertexIndex = 0;
    int numInside = 0;
    for (vertexIndex = 0; vertexIndex < numVertices; vertexIndex++)
    {
        if (box.contains(this->GetVertexAt(vertexIndex).GetCoord()))
        {
            numInside++;
        }
    }
    return numInside;
}

//------------------------------------------------------------------------------
/**
    Split mesh using clip plane. Fill the 2 meshes with the resulting triangles
    left and right of the clip plane. NOTE: this will NOT consider or preserve
    triangle groups! The resulting triangles will be cleaned before they
    are returned.

    @param  clipPlane   [in] a clip plane
    @param  posMesh     [out] filled with triangles on positive side of plane
    @param  negMesh     [out] filled with triangles on negative side of plane
*/
void
nMeshBuilder::Split(const plane& clipPlane, nMeshBuilder& posMesh, nMeshBuilder& negMesh) const
{
    int numTriangles = this->GetNumTriangles();
    int triangleIndex;
    for (triangleIndex = 0; triangleIndex < numTriangles; triangleIndex++)
    {
        Triangle triangle = this->GetTriangleAt(triangleIndex);
        int i0, i1, i2;
        triangle.GetVertexIndices(i0, i1, i2);
        const Vertex& v0 = this->GetVertexAt(i0);
        const Vertex& v1 = this->GetVertexAt(i1);
        const Vertex& v2 = this->GetVertexAt(i2);

        // classify vertex positions
        float dist[3];
        dist[0] = clipPlane.distance(v0.GetCoord());
        dist[1] = clipPlane.distance(v1.GetCoord());
        dist[2] = clipPlane.distance(v2.GetCoord());
        if ((dist[0] >= 0.0f) && (dist[1] >= 0.0f) && (dist[2] >= 0.0f))
        {
            // triangle entirely on positive side of clipPlane
            int vi = posMesh.GetNumVertices();
            posMesh.AddVertex(v0);
            posMesh.AddVertex(v1);
            posMesh.AddVertex(v2);
            triangle.SetVertexIndices(vi + 0, vi + 1, vi + 2);
            posMesh.AddTriangle(triangle);
        }
        else if ((dist[0] < 0.0f) && (dist[1] < 0.0f) && (dist[2] < 0.0f))
        {
            // triangle entirely on negative side of clipPlane
            int vi = negMesh.GetNumVertices();
            negMesh.AddVertex(v0);
            negMesh.AddVertex(v1);
            negMesh.AddVertex(v2);
            triangle.SetVertexIndices(vi + 0, vi + 1, vi + 2);
            negMesh.AddTriangle(triangle);
        }
        else
        {
            // triangle is clipped by clipPlane
            nArray<Vertex> posVertices(4, 0);
            nArray<Vertex> negVertices(4, 0);

            // get clip positions (t01, t12, t20) on the 3 edges (edge01, edge12, edge20) of the triangle
            line3 edge01(v0.GetCoord(), v1.GetCoord());
            line3 edge12(v1.GetCoord(), v2.GetCoord());
            line3 edge20(v2.GetCoord(), v0.GetCoord());
            float t01, t12, t20;
            bool t01Valid = clipPlane.intersect(edge01, t01);
            bool t12Valid = clipPlane.intersect(edge12, t12);
            bool t20Valid = clipPlane.intersect(edge20, t20);

            // build ordered polygon array for posSide and negSide vertices
            if (dist[0] >= 0.0f) posVertices.Append(v0);
            else                 negVertices.Append(v0);
            if (t01Valid && (t01 >= 0.0f) && (t01 < 1.0f))
            {
                Vertex v01;
                v01.Interpolate(v0, v1, t01);
                posVertices.Append(v01);
                negVertices.Append(v01);
            }
            if (dist[1] >= 0.0f) posVertices.Append(v1);
            else                 negVertices.Append(v1);
            if (t12Valid && (t12 >= 0.0f) && (t12 < 1.0f))
            {
                Vertex v12;
                v12.Interpolate(v1, v2, t12);
                posVertices.Append(v12);
                negVertices.Append(v12);
            }
            if (dist[2] >= 0.0f) posVertices.Append(v2);
            else                 negVertices.Append(v2);
            if (t20Valid && (t20 >= 0.0f) && (t20 < 1.0f))
            {
                Vertex v20;
                v20.Interpolate(v2, v0, t20);
                posVertices.Append(v20);
                negVertices.Append(v20);
            }

            // add clipped triangles to their respective meshes
            // (apply fan triangulation to polygons)
            if (posVertices.Size() >= 3)
            {
                int i;
                for (i = 0; i < (posVertices.Size() - 2); i++)
                {
                    int vi = posMesh.GetNumVertices();
                    posMesh.AddVertex(posVertices[0]);
                    posMesh.AddVertex(posVertices[i + 1]);
                    posMesh.AddVertex(posVertices[i + 2]);
                    triangle.SetVertexIndices(vi + 0, vi + 1, vi + 2);
                }
            }
            if (negVertices.Size() >= 3)
            {
                int i;
                for (i = 0; i < (negVertices.Size() - 2); i++)
                {
                    int vi = negMesh.GetNumVertices();
                    negMesh.AddVertex(negVertices[0]);
                    negMesh.AddVertex(negVertices[i + 1]);
                    negMesh.AddVertex(negVertices[i + 2]);
                    triangle.SetVertexIndices(vi + 0, vi + 1, vi + 2);
                }
            }
        }
    }
    
    // clean resuting meshes
    posMesh.Cleanup(0);
    negMesh.Cleanup(0);
}

//------------------------------------------------------------------------------
/**
    Makes sure that the group id stored in the triangles are consistent
    with the group id of the groups they are part of.
*/
void
nMeshBuilder::FixTriangleGroupIds()
{
    int numGroups = this->GetNumGroups();
    int groupIndex;
    for (groupIndex = 0; groupIndex < numGroups; groupIndex++)
    {
        const Group& group = this->GetGroupAt(groupIndex);
        int triangleIndex = group.GetFirstTriangle();
        int lastTriangleIndex  = group.GetNumTriangles() + triangleIndex;
        for (triangleIndex; triangleIndex < lastTriangleIndex; triangleIndex++)
        {
            this->GetTriangleAt(triangleIndex).SetGroupId(group.GetId());
        }
    }
}
