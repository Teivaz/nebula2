//------------------------------------------------------------------------------
//  nbspbuilder.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "tools/nbspbuilder.h"

//------------------------------------------------------------------------------
/**
*/
nBspBuilder::nBspBuilder() :
    rootNode(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nBspBuilder::~nBspBuilder()
{
    if (this->rootNode)
    {
        n_delete(this->rootNode);
        this->rootNode = 0;
    }
}

//------------------------------------------------------------------------------
/**
*/
nBspBuilder::BspNode*
nBspBuilder::GetBspTree() const
{
    return this->rootNode;
}

//------------------------------------------------------------------------------
/**
    Recursively split the mesh.

    @param  srcMesh         the source mesh
    @param  maxNodeSize     the maximum node size
    @param  box             the initial bounding box
    @return                 true if successful
*/
bool
nBspBuilder::BuildBsp(nMeshBuilder& srcMesh, float maxNodeSize, const bbox3& box)
{
    // transfer the triangle group id into the material id
    int numTris = srcMesh.GetNumTriangles();
    int triIndex;
    for (triIndex = 0; triIndex < numTris; triIndex++)
    {
        nMeshBuilder::Triangle& tri = srcMesh.GetTriangleAt(triIndex);
        tri.SetMaterialId(tri.GetGroupId());
        tri.SetGroupId(0);
    }

    // do the split
    int nextGroupId = 1;
    this->rootNode = this->Split(srcMesh, box, 0, maxNodeSize, 0, nextGroupId);

    // sort the result by group and material id
    srcMesh.SortTriangles();
    return true;
}

//------------------------------------------------------------------------------
/**
    Build a bsp node and return pointer to it. Also recursively builds child
    nodes if necessary.

    @param  mesh                the source mesh
    @param  box                 the current node's bounding box
    @param  groupId             current triangle group
    @param  maxDepth            maximum subdivision depth
    @param  depth               current tree depth
    @param  nextGroupId         [in/out] next free group id
    @return                     pointer to a new bsp node, can be 0
*/
nBspBuilder::BspNode*
nBspBuilder::Split(nMeshBuilder& mesh, const bbox3& box, int groupId, float maxNodeSize, int curDepth, int& nextGroupId)
{
    BspNode* node = n_new(BspNode);

    // select a split plane (split the longest dimension by half,
    // stop splitting if all dimensions are less then maxLength
    float xLen = box.vmax.x - box.vmin.x;
    float yLen = box.vmax.y - box.vmin.y;
    float zLen = box.vmax.z - box.vmin.z;
    vector3 mid = box.center();
    static vector3 v[3];
    bbox3 negBox(box);
    bbox3 posBox(box);
    if ((xLen < maxNodeSize) && (yLen < maxNodeSize) && (zLen < maxNodeSize))
    {
        // minimum node side length reached, stop splitting
        // a leaf node, write a valid group id
        node->SetMeshGroupIndex(groupId);
        node->SetBox(box);
        return node;
    }
    else if ((xLen > yLen) && (xLen > zLen))
    {
        // yz split plane
        v[0].set(mid.x, box.vmin.y, box.vmin.z);
        v[1].set(mid.x, box.vmin.y, box.vmax.z);
        v[2].set(mid.x, box.vmax.y, box.vmin.z);
        negBox.vmax.x = mid.x;
        posBox.vmin.x = mid.x;
    }
    else if ((yLen > xLen) && (yLen > zLen))
    {
        // xz split plane
        v[0].set(box.vmin.x, mid.y, box.vmin.z);
        v[1].set(box.vmin.x, mid.y, box.vmax.z);
        v[2].set(box.vmax.x, mid.y, box.vmin.z);
        negBox.vmax.y = mid.y;
        posBox.vmin.y = mid.y;
    }
    else
    {
        // xy split plane
        v[0].set(box.vmin.x, box.vmin.y, mid.z);
        v[1].set(box.vmin.x, box.vmax.y, mid.z);
        v[2].set(box.vmax.x, box.vmin.y, mid.z);
        negBox.vmax.z = mid.z;
        posBox.vmin.z = mid.z;
    }
    plane clipPlane(v[0], v[1], v[2]);
    node->SetPlane(clipPlane);

    // a non-leaf-node: split mesh using the selected clip plane...
    int posGroupId = nextGroupId++;
    int negGroupId = nextGroupId++;
    int numPosTriangles, numNegTriangles;
    mesh.Split(clipPlane, groupId, posGroupId, negGroupId, numPosTriangles, numNegTriangles);

    // and recurse...
    if (0 == numPosTriangles)
    {
        posGroupId = -1;
    }
    if (0 == numNegTriangles)
    {
        negGroupId = -1;
    }
    BspNode* posChild = this->Split(mesh, posBox, posGroupId, maxNodeSize, curDepth, nextGroupId);
    BspNode* negChild = this->Split(mesh, negBox, negGroupId, maxNodeSize, curDepth, nextGroupId);
    node->SetPosChild(posChild);
    node->SetNegChild(negChild);
    return node;
}
