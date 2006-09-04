#ifndef N_QUADTREE_H
#define N_QUADTREE_H
//------------------------------------------------------------------------------
/**
    @class nQuadTree
    @ingroup Util

    A simple quad tree. nQuadTree elements have a user data member, and
    are inserted and removed from a quadtree by bounding box.

    (C) 2004 RadonLabs GmbH
*/
#include "util/nfixedarray.h"
#include "util/narray.h"
#include "mathlib/bbox.h"
#include "util/nlist.h"
#include "util/nnode.h"

//------------------------------------------------------------------------------
class nQuadTree
{
public:
    class Node;
    class Element;

    /// constructor
    nQuadTree();
    /// destructor
    ~nQuadTree();
    /// initialize quad tree
    void Initialize(uchar depth, const bbox3& box);
    /// get the tree depth
    uchar GetDepth() const;
    /// get the top level bounding box
    const bbox3& GetBoundingBox() const;
    /// render the quadtree (calls Element::Render() on visible elements)
    void Render(const matrix44& viewProj);
    /// render debug visualization
    void RenderDebug(const matrix44& viewProj);
    /// insert element into quad tree
    void Insert(Element* elm, const bbox3& box);
    /// update an element in the quadtree
    void Update(Element* elm, const bbox3& box);
    /// remove an element from the quad tree
    void Remove(Element* elm);
    /// compute number of nodes in a level, including its children
    int GetNumNodes(uchar level) const;
    /// compute linear chunk index from level, col and row
    int GetNodeIndex(uchar level, ushort col, ushort row) const;
    /// get pointer to node by index
    const Node& GetNodeByIndex(int i) const;

    /// an element in the tree, derive subclass for customized Render() method
    class Element : public nNode
    {
    public:
        /// constructor
        Element();
        /// destructor
        virtual ~Element();
        /// render the element (override in subclass)
        virtual void Render(const matrix44& viewProj, bbox3::ClipStatus clipStatus);
        /// render debug visualization (override in subclass)
        virtual void RenderDebug(const matrix44& viewProj, bbox3::ClipStatus clipStatus);

    private:
        friend class nQuadTree;
        friend class Node;

        Node* node;
    };

    /// node in quad tree
    class Node
    {
    public:
        /// constructor
        Node();
        /// destructor
        ~Node();
        /// recursively initialize the node
        void Initialize(nQuadTree* tree, uchar _level, ushort _col, ushort _row);
        /// get the node's level
        char Level() const;
        /// get the node's column
        ushort Column() const;
        /// get the node's row
        ushort Row() const;
        /// access to element array
        nArray<Element>& ElementArray();
        /// compute the node's bounding box
        const bbox3& GetBoundingBox() const;
        /// recursively find the smallest child node which contains the bounding box
        Node* FindContainmentNode(const bbox3& box);
        /// add element to node
        void AddElement(Element* elm);
        /// remove element from node
        void RemElement(Element* elm);
        /// recursively render visible elements
        void Render(const matrix44& viewProj, bbox3::ClipStatus clipStatus);
        /// recursively render debug visualization
        void RenderDebug(const matrix44& viewProj, bbox3::ClipStatus clipStatus);

    private:
        friend class nQuadTree;

        Node* children[4];
        char level;
        ushort col;
        ushort row;
        bbox3 box;
        nList elmList;
    };

private:
    friend class Node;

    uchar treeDepth;
    bbox3 boundingBox;                  // global bounding box
    vector3 baseNodeSize;               // base chunk bounding box
    nFixedArray<Node> nodeArray;
};

//------------------------------------------------------------------------------
/**
*/
inline
nQuadTree::Node::Node() :
    level(-1),
    col(0),
    row(0)
{
    int i;
    for (i = 0; i < 4; i++)
    {
        this->children[i] = 0;
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
nQuadTree::Node::~Node()
{
    // unlink elements
    nNode* elm;
    while (elm = this->elmList.RemHead());
}

//------------------------------------------------------------------------------
/**
*/
inline
const nQuadTree::Node&
nQuadTree::GetNodeByIndex(int index) const
{
    return this->nodeArray[index];
}

//------------------------------------------------------------------------------
/**
*/
inline
const bbox3&
nQuadTree::Node::GetBoundingBox() const
{
    return this->box;
}

//------------------------------------------------------------------------------
/**
*/
inline
char
nQuadTree::Node::Level() const
{
    return this->level;
}

//------------------------------------------------------------------------------
/**
*/
inline
ushort
nQuadTree::Node::Column() const
{
    return this->col;
}

//------------------------------------------------------------------------------
/**
*/
inline
ushort
nQuadTree::Node::Row() const
{
    return this->row;
}

//------------------------------------------------------------------------------
/**
    Returns depth of quad tree.
*/
inline
uchar
nQuadTree::GetDepth() const
{
    return this->treeDepth;
}

//------------------------------------------------------------------------------
/**
    Returns top level bounding box of quad tree.
*/
inline
const bbox3&
nQuadTree::GetBoundingBox() const
{
    return this->boundingBox;
}

//------------------------------------------------------------------------------
/**
    Computes number of nodes in a level, including its child nodes.
*/
inline
int
nQuadTree::GetNumNodes(uchar level) const
{
    return 0x55555555 & ((1 << level * 2) - 1);
}

//------------------------------------------------------------------------------
/**
    Computes a linear chunk index for a chunk address consisting of
    level, col and row.
*/
inline
int
nQuadTree::GetNodeIndex(uchar level, ushort col, ushort row) const
{
    n_assert((col >= 0) && (col < (1 << level)));
    n_assert((row >= 0) && (row < (1 << level)));
    return this->GetNumNodes(level) + (row << level) + col;
}

//------------------------------------------------------------------------------
/**
    Adds an element to this node.
*/
inline
void
nQuadTree::Node::AddElement(Element* elm)
{
    n_assert(elm);
    elm->node = this;
    this->elmList.AddTail(elm);
}

//------------------------------------------------------------------------------
/**
    Removes element from this node.
*/
inline
void
nQuadTree::Node::RemElement(Element* elm)
{
    n_assert(elm && (elm->node == this));
    elm->node = 0;
    elm->Remove();
}

//------------------------------------------------------------------------------
/**
*/
inline
nQuadTree::Element::Element() :
    node(0)
{
    // empty
}

//------------------------------------------------------------------------------
#endif
