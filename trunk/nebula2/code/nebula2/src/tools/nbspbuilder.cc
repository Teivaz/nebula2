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
        delete this->rootNode;
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
    @return                 true if successful
*/
bool
nBspBuilder::BuildBsp(nMeshBuilder& srcMesh, int maxDepth)
{
    int nextGroupId = 1;
    this->rootNode = this->Split(srcMesh, 0, maxDepth, 0, nextGroupId);
    return true;
}

//------------------------------------------------------------------------------
/**
    Build a bsp node and return pointer to it. Also recursively builds child 
    nodes if necessary.

    @param  mesh                the source mesh
    @param  groupId             current triangle group
    @param  maxDepth            maximum subdivision depth
    @param  depth               current tree depth
    @param  nextGroupId         [in/out] next free group id
    @return                     pointer to a new bsp node, can be 0
*/
nBspBuilder::BspNode*
nBspBuilder::Split(nMeshBuilder& mesh, int groupId, int maxDepth, int curDepth, int& nextGroupId)
{
    BspNode* node = new BspNode;
    node->SetMeshGroupIndex(groupId);

    // select a split plane
    bbox3 box = mesh.ComputeGroupBBox(groupId);
    vector3 mid = box.center();
    vector3 v[3];
    switch (curDepth % 3)
    {
        case 0:
            // try yz plane
            v[0].set(mid.x, box.vmin.y, box.vmin.z); 
            v[1].set(mid.x, box.vmin.y, box.vmax.z);
            v[2].set(mid.x, box.vmax.y, box.vmin.z);
            break;

        case 1:
            // try xy plane
            v[0].set(box.vmin.x, box.vmin.y, mid.z);
            v[1].set(box.vmin.x, box.vmax.y, mid.z);
            v[2].set(box.vmax.x, box.vmin.y, mid.z);
            break;

        case 2:
            // try xz plane
            v[0].set(box.vmin.x, mid.y, box.vmin.z);
            v[1].set(box.vmin.x, mid.y, box.vmax.z);
            v[2].set(box.vmax.x, mid.y, box.vmin.z);
            break;
    };
    plane clipPlane(v[0], v[1], v[2]);
    node->SetPlane(clipPlane);

    // split mesh using the selected clip plane...
    int posIndex = nextGroupId++;
    int negIndex = nextGroupId++;
    mesh.Split(clipPlane, groupId, posIndex, negIndex);

    // and recurse...
    curDepth++;
    if (curDepth < maxDepth)
    {
        BspNode* posChild = this->Split(mesh, posIndex, maxDepth, curDepth, nextGroupId);
        BspNode* negChild = this->Split(mesh, negIndex, maxDepth, curDepth, nextGroupId);
        node->SetPosChild(posChild);
        node->SetNegChild(negChild);
    }
    return node;
}
