#ifndef N_BSPBUILDER_H
#define N_BSPBUILDER_H
//------------------------------------------------------------------------------
/**
    A simple BSP builder. Recursively splits a mesh along the world axes 
    until a defined recursion depth has been reached.
    
    (C) 2003 RadonLabs GmbH
*/
#ifndef N_MESHBUILDER_H
#include "tools/nmeshbuilder.h"
#endif

//------------------------------------------------------------------------------
class nBspBuilder
{
public:
    /// a bsp node in a bsp tree
    class BspNode
    {
    public:
        /// constructor
        BspNode();
        /// destructor
        ~BspNode();
        /// set negative child
        void SetNegChild(BspNode* c);
        /// get negative child
        BspNode* GetNegChild() const;
        /// set positive child
        void SetPosChild(BspNode* c);
        /// get positive child
        BspNode* GetPosChild() const;
        /// set plane used to split this node
        void SetPlane(const plane& p);
        /// get plane used to split this node
        const plane& GetPlane() const;
        /// return true if this is a leaf node (contains geometry)
        bool IsLeaf() const;
        /// set mesh group index containing node geometry
        void SetMeshGroupIndex(int i);
        /// get mesh group index containing node geometry
        int GetMeshGroupIndex() const;

    private:
        BspNode* negChild;
        BspNode* posChild;
        plane splitPlane;
        int meshGroupIndex;
    };

    /// constructor
    nBspBuilder();
    /// destructor
    ~nBspBuilder();
    /// build bsp tree
    bool BuildBsp(nMeshBuilder& srcMesh, int maxDepth);
    /// get pointer to resulting bsp tree
    BspNode* GetBspTree() const;

private:
    /// append dstMesh to srcMesh, and update groups in dstMesh
    int AppendMesh(nMeshBuilder& srcMesh, nMeshBuilder& dstMesh);
    /// recursively create new bsp nodes
    BspNode* Split(nMeshBuilder& srcMesh, int groupId, int maxDepth, int depth, int& nextGroupId);

    BspNode* rootNode;      // root node of bsp tree
};

//------------------------------------------------------------------------------
/**
*/
inline
nBspBuilder::BspNode::BspNode() :
    negChild(0),
    posChild(0),
    meshGroupIndex(-1)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nBspBuilder::BspNode::~BspNode()
{
    if (this->negChild)
    {
        delete this->negChild;
    }
    if (this->posChild)
    {
        delete this->posChild;
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nBspBuilder::BspNode::SetNegChild(BspNode* c)
{
    n_assert(c);
    n_assert(0 == this->negChild);
    this->negChild = c;
}

//------------------------------------------------------------------------------
/**
*/
inline
nBspBuilder::BspNode*
nBspBuilder::BspNode::GetNegChild() const
{
    return this->negChild;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nBspBuilder::BspNode::SetPosChild(BspNode* c)
{
    n_assert(c);
    n_assert(0 == this->posChild);
    this->posChild = c;
}

//------------------------------------------------------------------------------
/**
*/
inline
nBspBuilder::BspNode*
nBspBuilder::BspNode::GetPosChild() const
{
    return this->posChild;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nBspBuilder::BspNode::SetPlane(const plane& p)
{
    this->splitPlane = p;
}

//------------------------------------------------------------------------------
/**
*/
inline
const plane&
nBspBuilder::BspNode::GetPlane() const
{
    return this->splitPlane;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nBspBuilder::BspNode::IsLeaf() const
{
    return (this->posChild == 0);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nBspBuilder::BspNode::SetMeshGroupIndex(int i)
{
    this->meshGroupIndex = i;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nBspBuilder::BspNode::GetMeshGroupIndex() const
{
    return this->meshGroupIndex;
}

//------------------------------------------------------------------------------
#endif
