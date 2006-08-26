//------------------------------------------------------------------------------
//  nquadtree.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "util/nquadtree.h"
#include "gfx2/ngfxserver2.h"

//------------------------------------------------------------------------------
/**
    nQuadTree constructor.
*/
nQuadTree::nQuadTree() :
    treeDepth(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
    nQuadTree destructor.
*/
nQuadTree::~nQuadTree()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Initialize the quad tree.
*/
void
nQuadTree::Initialize(uchar depth, const bbox3& box)
{
    n_assert(depth > 0);
    this->treeDepth = depth;
    this->boundingBox = box;

    int baseDimension = 1 << (depth - 1);
    this->baseNodeSize.x = this->boundingBox.size().x / baseDimension;
    this->baseNodeSize.y = this->boundingBox.size().y;
    this->baseNodeSize.z = this->boundingBox.size().z / baseDimension;

    int numNodes = this->GetNumNodes(depth);
    this->nodeArray.SetSize(numNodes);
    this->nodeArray[0].Initialize(this, 0, 0, 0);

    // make sure all nodes have been initialized
    int i;
    int num = this->nodeArray.Size();
    for (i = 0; i < num; i++)
    {
        n_assert(this->nodeArray[i].Level() >= 0);
    }
}

//------------------------------------------------------------------------------
/**
    This inserts an element into the quad tree.
*/
void
nQuadTree::Insert(Element* elm, const bbox3& box)
{
    n_assert(elm && (!elm->IsLinked()));

    // find the smallest node which completely encloses the element's bbox
    Node* node = this->nodeArray[0].FindContainmentNode(box);
    if (node)
    {
        node->AddElement(elm);
    }
    else
    {
        // NOTE: technically, the node is outside of the world,
        // just attach it to the toplevel node
        this->nodeArray[0].AddElement(elm);
    }
}

//------------------------------------------------------------------------------
/**
    This updates an element in the quad tree. Call this method if the
    bounding box of the element changes.
*/
void
nQuadTree::Update(Element* elm, const bbox3& box)
{
    n_assert(elm && elm->IsLinked());

    // NOTE: this could be made more efficient by walking from the current
    // element's node upward to the first containing parent node, then
    // downward to the smallest containing child cell...
    // instead we just start at the root
    this->Remove(elm);
    this->Insert(elm, box);
}

//------------------------------------------------------------------------------
/**
    Removes an element from the quad tree.
*/
void
nQuadTree::Remove(Element* elm)
{
    n_assert(elm && elm->IsLinked());
    n_assert(0 != elm->node);
    elm->node->RemElement(elm);
}

//------------------------------------------------------------------------------
/**
    Call render on all visible elements.
*/
void
nQuadTree::Render(const matrix44& viewProj)
{
    this->nodeArray[0].Render(viewProj, bbox3::Clipped);
}

//------------------------------------------------------------------------------
/**
    Render a debug visualization for the quad tree.
*/
void
nQuadTree::RenderDebug(const matrix44& viewProj)
{
    // we're working in global space...
    nGfxServer2* gfxServer = nGfxServer2::Instance();
    gfxServer->BeginShapes();
    this->nodeArray[0].RenderDebug(viewProj, bbox3::Clipped);
    gfxServer->EndShapes();
}

//------------------------------------------------------------------------------
/**
    nQuadTree::Element destructor.
*/
nQuadTree::Element::~Element()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Render a quad tree element. This virtual method should be overwritten
    in a subclass.

    The method is called with the current viewProjection matrix and a clip
    status as arguments. The clip status is either Inside or Clipped. If the
    clip status is inside, the element is guaranteed to be visible and can be
    rendered immediately. When clip status is Clipped, the element is not
    guaranteed to be visible, and a further Clipping check using the viewProj
    matrix and the element's bounding box should be performed.
*/
void
nQuadTree::Element::Render(const matrix44& /*viewProj*/, bbox3::ClipStatus /*clipStatus*/)
{
    // empty
}

//------------------------------------------------------------------------------
/**
    This is a clone of Element::Render(), but for the debug visualization.
    The gfx server is already inside BeginLines()/EndLines() when this
    method is called.
*/
void
nQuadTree::Element::RenderDebug(const matrix44& /*viewProj*/, bbox3::ClipStatus /*clipStatus*/)
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Recursively initialize a quad tree node.
*/
void
nQuadTree::Node::Initialize(nQuadTree* tree, uchar _level, ushort _col, ushort _row)
{
    n_assert(tree);
    n_assert(this->level == -1);
    n_assert(_level >= 0);
    n_assert((_col >= 0) && (_col < (1 << _level)));
    n_assert((_row >= 0) && (_row < (1 << _level)));

    // store address
    this->level = _level;
    this->col   = _col;
    this->row   = _row;

    // update my bounding box
    float levelFactor = float(1 << (tree->treeDepth - 1 - this->level));
    static vector3 center;
    static vector3 extent;
    const vector3& baseSize = tree->baseNodeSize;
    const bbox3& treeBox = tree->boundingBox;
    vector3 treeSize = treeBox.size();

    center.x = ((this->col + 0.5f) * levelFactor * baseSize.x) - (treeSize.x * 0.5f);
    center.y = treeBox.center().y;
    center.z = ((this->row + 0.5f) * levelFactor * baseSize.z) - (treeSize.z * 0.5f);

    extent.x = levelFactor * baseSize.x * 0.5f;
    extent.y = treeBox.extents().y;
    extent.z = levelFactor * baseSize.z * 0.5f;

    this->box.set(center, extent);

    // recurse into children
    uchar childLevel = this->level + 1;
    if (childLevel < tree->treeDepth)
    {
        int i;
        for (i = 0; i < 4; i++)
        {
            ushort childCol = 2 * this->col + (i & 1);
            ushort childRow = 2 * this->row + ((i & 2) >> 1);
            int childIndex = tree->GetNodeIndex(childLevel, childCol, childRow);
            this->children[i] = &(tree->nodeArray[childIndex]);
            this->children[i]->Initialize(tree, childLevel, childCol, childRow);
        }
    }
}

//------------------------------------------------------------------------------
/**
    This finds the smallest child node which completely contains the
    given bounding box. Calls itself recursively.
*/
nQuadTree::Node*
nQuadTree::Node::FindContainmentNode(const bbox3& checkBox)
{
    if (this->box.contains(checkBox))
    {
        // recurse into children
        if (this->children[0] != 0)
        {
            int i;
            for (i = 0; i < 4; i++)
            {
                Node* containNode = this->children[i]->FindContainmentNode(checkBox);
                if (containNode)
                {
                    return containNode;
                }
            }
        }

        // not contained in children, but still contained in this
        return this;
    }
    else
    {
        // not contained in this, break recursion
        return 0;
    }
}

//------------------------------------------------------------------------------
/**
    Recursively renders visible node elements by invoking the Element::Render()
    method on visible elements.
*/
void
nQuadTree::Node::Render(const matrix44& viewProj, bbox3::ClipStatus clipStatus)
{
    n_assert(clipStatus != bbox3::Outside);

    // view frustum culling (we don't need to check for culling if one
    // of our parents was already fully inside)
    if (bbox3::Clipped == clipStatus)
    {
        clipStatus = this->box.clipstatus(viewProj);
        if (bbox3::Outside == clipStatus)
        {
            // fully outside, return and break recursion
            return;
        }
    }

    // render own elements...
    Element* elm;
    for (elm = (Element*) this->elmList.GetHead(); elm; elm = (Element*) elm->GetSucc())
    {
        elm->Render(viewProj, clipStatus);
    }

    // ...and recurse into children
    if (0 != this->children[0])
    {
        int i;
        for (i = 0; i < 4; i++)
        {
            this->children[i]->Render(viewProj, clipStatus);
        }
    }
}

//------------------------------------------------------------------------------
/**
    Implement the debug renderer. This renders a color-coded box for each
    quad tree node.
*/
void
nQuadTree::Node::RenderDebug(const matrix44& viewProj, bbox3::ClipStatus clipStatus)
{
    n_assert(clipStatus != bbox3::Outside);

    // view frustum culling (we don't need to check for culling if one
    // of our parents was already fully inside)
    if (bbox3::Clipped == clipStatus)
    {
        clipStatus = this->box.clipstatus(viewProj);
        if (bbox3::Outside == clipStatus)
        {
            // fully outside, return and break recursion
            return;
        }
    }

    // render own elements...
    Element* elm;
    for (elm = (Element*) this->elmList.GetHead(); elm; elm = (Element*) elm->GetSucc())
    {
        elm->RenderDebug(viewProj, clipStatus);
    }

    // ...recurse into children, or...
    if (0 != this->children[0])
    {
        int i;
        for (i = 0; i < 4; i++)
        {
            this->children[i]->RenderDebug(viewProj, clipStatus);
        }
    }
    else
    {
        // render debug visualization
        static vector4 color;
        switch (clipStatus)
        {
            case bbox3::Clipped:    color.set(1.0f, 1.0f, 0.0f, 0.2f); break;   // clipped: yellow
            case bbox3::Inside:     color.set(1.0f, 0.0f, 0.0f, 0.2f); break;   // inside: red
            default:                n_assert(false);                            // can't happen
        }
        nGfxServer2::Instance()->DrawShape(nGfxServer2::Box, this->box.to_matrix44(), color);
    }
}
