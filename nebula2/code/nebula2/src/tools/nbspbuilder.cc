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
    Build the bsp tree. This splits the source mesh recursively along the
    world axes until the triangle per node is below maxNumVertices. The 
    splitter doesn't try to be very clever about the 
    clip plane selection. It will try the middle xz, xy and yz plane for the best
    balance between the negative and positive nodes. A tree of 
    nBspBuilder::BspNode object will be built internally which can be obtain
    with the GetBspTree() method after building the tree. The resulting
    mesh geometry will be written to the dstMesh object, with one group
    per leaf node.

    @param  srcMesh         the source mesh
    @param  dstMesh         the destination mesh, with one mesh group per leaf node
    @param  maxNumVertices  the max number of vertices per leaf node
    @return                 true if successful
*/
bool
nBspBuilder::BuildBsp(const nMeshBuilder& srcMesh, nMeshBuilder& dstMesh, int maxNumVertices)
{
    n_assert(0 == dstMesh.GetNumVertices());
    n_assert(0 == dstMesh.GetNumTriangles());
    n_assert(0 == dstMesh.GetNumGroups());
    this->rootNode = this->Split(srcMesh, dstMesh, maxNumVertices);
    dstMesh.Cleanup(0);
    return true;
}

//------------------------------------------------------------------------------
/**
    Appends srcMesh to dstMesh, forming a new group, and returns the group 
    index of the new mesh group.
*/
int
nBspBuilder::AppendMesh(const nMeshBuilder& srcMesh, nMeshBuilder& dstMesh)
{
    nMeshBuilder::Group newGroup;
    newGroup.SetId(dstMesh.GetNumGroups());
    newGroup.SetFirstTriangle(0);
    newGroup.SetNumTriangles(srcMesh.GetNumTriangles());
    
    nMeshBuilder newMesh;
    newMesh.vertexArray = srcMesh.vertexArray;
    newMesh.triangleArray = srcMesh.triangleArray;
    newMesh.AddGroup(newGroup);
    newMesh.FixTriangleGroupIds();

    dstMesh.Append(newMesh);
    
    return dstMesh.GetNumGroups() - 1;
}

//------------------------------------------------------------------------------
/**
    Build a bsp node and return pointer to it. Also recursively builds child 
    nodes if necessary.

    @param  srcMesh             the source mesh
    @param  dstMesh             the dst mesh
    @param  maxVertices         maximum allowed number of vertices per node
    @return                     pointer to a new bsp node
*/
nBspBuilder::BspNode*
nBspBuilder::Split(const nMeshBuilder& srcMesh, nMeshBuilder& dstMesh, int maxVertices)
{
    BspNode* node = new BspNode;

    // check current number of vertices in source mesh against threshold
    if (srcMesh.GetNumVertices() <= maxVertices)
    {
        // we are below the threshold, append srcMesh to dstMesh
        // and initialize node as leaf node
        int groupId = this->AppendMesh(srcMesh, dstMesh);
        node->SetMeshGroupIndex(groupId);
    }
    else
    {
        bbox3 box = srcMesh.ComputeBBox();
        vector3 mid = box.center();

        // check 3 possible splits, and use the split with the 
        // best balance
        int i;
        vector3 v[3];
        plane clipPlane[3];
        int bestBalanceDiff = 10000000;
        int bestBalanceIndex = 0;
        const int numVertices = srcMesh.GetNumVertices();
        for (i = 0; i < 3; i++)
        {
            switch (i)
            {
                case 0:
                    // try yz plane
                    v[0].set(mid.x, box.vmin.y, box.vmin.z); 
                    v[1].set(mid.x, box.vmin.y, box.vmax.z);
                    v[2].set(mid.x, box.vmax.y, box.vmin.z);
                    break;

                case 1:
                    // try xz plane
                    v[0].set(box.vmin.x, mid.y, box.vmin.z);
                    v[1].set(box.vmin.x, mid.y, box.vmax.z);
                    v[2].set(box.vmax.x, mid.y, box.vmin.z);
                    break;

                case 2:
                    // try xy plane
                    v[0].set(box.vmin.x, box.vmin.y, mid.z);
                    v[1].set(box.vmin.x, box.vmax.y, mid.z);
                    v[2].set(box.vmax.x, box.vmin.y, mid.z);
                    break;
            };
            clipPlane[i].set(v[0], v[1], v[2]);
            int numPosVertices = 0;
            int numNegVertices = 0;
            int vertexIndex;
            for (vertexIndex = 0; vertexIndex < numVertices; vertexIndex++)
            {
                float dist = clipPlane[i].distance(srcMesh.GetVertexAt(vertexIndex).GetCoord());
                if (dist >= 0.0f) numPosVertices++;
                else              numNegVertices++;
            }
            int diff = abs(numPosVertices - numNegVertices);
            if (diff < bestBalanceDiff)
            {
                bestBalanceDiff = diff;
                bestBalanceIndex = i;
            }
        }

        // now do the split using the clipPlane with the best balance
        nMeshBuilder posMesh;
        nMeshBuilder negMesh;
        srcMesh.Split(clipPlane[bestBalanceIndex], posMesh, negMesh);

        // and recurse...
        BspNode* posChild = this->Split(posMesh, dstMesh, maxVertices);
        BspNode* negChild = this->Split(negMesh, dstMesh, maxVertices);
        node->SetPosChild(posChild);
        node->SetNegChild(negChild);
        node->SetPlane(clipPlane[bestBalanceIndex]);
    }
    return node;
}
