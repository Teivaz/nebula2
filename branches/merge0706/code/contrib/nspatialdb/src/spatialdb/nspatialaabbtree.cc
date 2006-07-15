//--------------------------------------------------------
// nspatialaabbtree.cc
// (C) 2004 Gary Haussmann
//
// implementation for spatial AABB tree
//--------------------------------------------------------

#include "spatialdb/nspatialaabbtree.h"

class nSpatialAABBTreeNode {
public:
    nSpatialAABBTreeNode() : m_elementsinnode(0), m_totalelements(0), m_parent(NULL) 
    { m_children[0] = m_children[1] = NULL; }

    // destructor does nothing really
    ~nSpatialAABBTreeNode() { n_assert(!HasChildren()); }

    // returns true if this node haschild nodes
    bool HasChildren() const { return (m_children[0] != NULL); }

    // adds element to this node
    void AddElement(nSpatialElement *addme);

    // finds the element in this node or child nodes; returns the containing tree node
    nSpatialAABBTreeNode *FindElement(nSpatialElement *findme);

    // removes the element from this node only; if it's not in this node, the call fails.
    // for proper use, find the containing treenode using FindElement() and call RemoveElement()
    // on that one
    void RemoveElement(nSpatialElement *removeme);

    // this function will divvy up the elements and stuff them into the child nodes
    void Subdivide(nSpatialAABBTree *nodemanager);

    // recompute the total element count
    void RecomputeElementCount();

    // recompute the bounding box
    void RecomputeBoundingBox();

    void ClearNodes(nSpatialAABBTree *nodemanager);

    // visitor processing
    void Accept(nVisibilityVisitor &visitor, int recursiondepth, VisitorFlags flags);
    void Accept(nSpatialVisitor &visitor, int recursiondepth, VisitorFlags flags);
    void Accept(nOcclusionVisitor &visitor, int recursiondepth, VisitorFlags flags);

    // bbox for this node
    bbox3 m_bbox;

    // split axis and position
    int m_splitaxis;  // 0=x,1=y,2=z
    float m_splitposition;

    enum { MAX_ELEMENTSPERNODE = 10 };

    // spatialelements in this node
    nSpatialElement *m_elements[MAX_ELEMENTSPERNODE];
    int m_elementsinnode;

    // total elements in this node and all of its children
    int m_totalelements;

    // pointers to other nodes
    nSpatialAABBTreeNode *m_children[2];
    nSpatialAABBTreeNode *m_parent;
};

void nSpatialAABBTreeNode::AddElement(nSpatialElement *addme)
{
    n_assert(m_elementsinnode + 1 < nSpatialAABBTreeNode::MAX_ELEMENTSPERNODE);
    n_assert(!HasChildren());
    n_assert(m_elementsinnode == m_totalelements);
    m_elements[m_elementsinnode++] = addme;
    m_totalelements++;
    if (m_parent)
        m_parent->RecomputeElementCount();
    // expand bounding boxes up the tree
    RecomputeBoundingBox();        
}

nSpatialAABBTreeNode *nSpatialAABBTreeNode::FindElement(nSpatialElement *findme)
{
    n_assert(findme->HasAABB());

    // is the element in this node?
    for (int elmix=0; elmix < m_elementsinnode; ++elmix)
    {
        if (m_elements[elmix] == findme)
            return this;
    }
    // are there any children? if so, check the appropriate ones
    if (!HasChildren()) return NULL;

    bbox3 elementbox(findme->GetAABB());
    for (int childix=0; childix<2; childix++)
    {
        // we can't use the bbox::contains() method since it only returns true for
        // 'strict' containment; if a bbox is flush against the inside of another
        // bbox it is not considered contained by the contains() method.
        // so we have to do our own custom containment testing
        bbox3 childbbox(m_children[childix]->m_bbox);
        const float epsilon=1e-6f;
        if ( (m_children[childix]->m_totalelements > 0) &&
             (childbbox.vmin.x - epsilon <= elementbox.vmin.x) &&
             (childbbox.vmin.y - epsilon <= elementbox.vmin.y) &&
             (childbbox.vmin.z - epsilon <= elementbox.vmin.z) &&
             (childbbox.vmax.x + epsilon >= elementbox.vmax.x) &&
             (childbbox.vmax.y + epsilon >= elementbox.vmax.y) &&
             (childbbox.vmax.z + epsilon >= elementbox.vmax.z)
             )
        {
            nSpatialAABBTreeNode *f = m_children[childix]->FindElement(findme);
            if (f) return f;
        }
    }

    // not found
    return NULL;
}

void nSpatialAABBTreeNode::RemoveElement(nSpatialElement *removeme)
{
    // find it and nuke it
    for (int elmix=0; elmix < m_elementsinnode; ++elmix)
    {
        if (removeme == m_elements[elmix])
        {
            // a match, copy the last element to fill it in
            if (elmix != m_elementsinnode - 1)
                m_elements[elmix] = m_elements[m_elementsinnode-1];
            m_elementsinnode -= 1;
            RecomputeElementCount();
            if (m_elementsinnode > 0)
                RecomputeBoundingBox();
            return;
        }
    }
    
    //no match, oops
    n_assert(0);
}

void nSpatialAABBTreeNode::Subdivide(nSpatialAABBTree *nodemanager)
{
    n_assert(!HasChildren());

    int elmix;

    // allocate children
    this->m_children[0] = nodemanager->alloc_node();
    this->m_children[0]->m_parent = this;
    this->m_children[1] = nodemanager->alloc_node();
    this->m_children[1]->m_parent = this;

    // OK, find the splitting point by averaging the element centers.  the axis we choose is the
    // one with the largest range
    bbox3 nodebbox(m_elements[0]->GetAABB());
    vector3 elementmean(nodebbox.center());
    for (elmix=1; elmix < m_elementsinnode; ++elmix)
    {
        bbox3 &elementbbox = m_elements[elmix]->GetAABB();
        nodebbox.extend(elementbbox);
        elementmean += elementbbox.center();
    }
    elementmean *= (1.0f/m_elementsinnode);
    // pick our split axis
    vector3 noderange(nodebbox.extents());
    int splitaxis=0;
    float splitrange = noderange.x;
    if (noderange.x < noderange.y)
    {
        splitaxis=1;
        splitrange = noderange.y;
    }
    if (splitrange < noderange.z)
    {
        splitaxis=2;
        splitrange = noderange.z;
    }

    m_splitaxis = splitaxis;
    float means[3] = { elementmean.x,elementmean.y, elementmean.z };
    m_splitposition = means[m_splitaxis];

    // dump all the elements into the appropriate child: less than splitposision->child 0,
    // otherwise child 1
    vector3 evalposition( (splitaxis==0 ? 1.f : 0.f) , (splitaxis==1 ? 1.f : 0.f), (splitaxis==2 ? 1.f : 0.f));
    for (elmix=0; elmix < m_elementsinnode; ++elmix)
    {
        vector3 elmcenter(m_elements[elmix]->GetAABB().center());
        if (evalposition % elmcenter < m_splitposition)
        {
            m_children[0]->m_elements[m_children[0]->m_elementsinnode++] = m_elements[elmix];
        }
        else
        {
            m_children[1]->m_elements[m_children[1]->m_elementsinnode++] = m_elements[elmix];
        }
    }

    // if one child is empty, then the subdivision did no good, so redo the split by dumping 1/2 of the nodes in each child
    if ( (m_children[0]->m_elementsinnode == 0) || (m_children[1]->m_elementsinnode == 0) )
    {
        m_children[0]->m_elementsinnode = m_children[1]->m_elementsinnode = 0;
        for (elmix=0; elmix < m_elementsinnode; ++elmix)
        {
            int childix = (elmix * 2) / m_elementsinnode;
            m_children[childix]->m_elements[m_children[childix]->m_elementsinnode++] = m_elements[elmix];
        }
    }

    // ok, wipe out our elements-other element counts remain unchanged
    m_elementsinnode = 0;

    // update the bounding boxes of our children and ourselves
    m_children[0]->RecomputeElementCount();
    m_children[0]->RecomputeBoundingBox();
    m_children[1]->RecomputeElementCount();
    m_children[1]->RecomputeBoundingBox();
}

void nSpatialAABBTreeNode::RecomputeElementCount()
{
    m_totalelements = m_elementsinnode;
    if (HasChildren())
    {
        m_totalelements += m_children[0]->m_totalelements;
        m_totalelements += m_children[1]->m_totalelements;
    }
    if (m_parent)
        m_parent->RecomputeElementCount();
}

void nSpatialAABBTreeNode::RecomputeBoundingBox()
{
    // if we have children, just use their bounding boxes
    if (HasChildren())
    {
        m_bbox = m_children[0]->m_bbox;
        m_bbox.extend(m_children[1]->m_bbox);
    }
    else
    {
        // use the bounding boxes of our elements
        m_bbox = m_elements[0]->GetAABB();
        for (int elmix=1; elmix < m_elementsinnode; ++elmix)
            m_bbox.extend(m_elements[elmix]->GetAABB());
    }
    // in either case, tell our parent to update as well
    if (m_parent)
        m_parent->RecomputeBoundingBox();
}

void nSpatialAABBTreeNode::ClearNodes(nSpatialAABBTree *nodemanager)
{
    if (HasChildren())
    {
        for (int cix=0; cix<2; ++cix)
        {
            m_children[cix]->ClearNodes(nodemanager);
            nodemanager->free_node(m_children[cix]);
            m_children[cix] = NULL;
        }
    }
}


void nSpatialAABBTreeNode::Accept(nVisibilityVisitor &visitor,  int recursiondepth, VisitorFlags flags)
{
    // check our bounding box--is it visible?  Also updates the active flags
    flags = visitor.VisibilityTest(this->m_bbox, flags);
    if (!flags.TestResult())
        return;

    // check each element in this node individually
    for (int elix=0; elix < m_elementsinnode; ++elix)
    {
        m_elements[elix]->Accept(visitor, recursiondepth, flags);
    }
    // descend to children--pass down the visitorflags, to save redundant plane testing!
    if (HasChildren())
    for (int childix=0; childix<2; ++childix)
    {
        m_children[childix]->Accept(visitor, recursiondepth, flags);
    }
}

void nSpatialAABBTreeNode::Accept(nSpatialVisitor &visitor,  int recursiondepth, VisitorFlags flags)
{
    // check our bounding box--is it contained?  Also updates the active flags
    flags = visitor.ContainmentTest(this->m_bbox, flags);
    if (!flags.TestResult())
        return;

    // check each element in this node individually
    for (int elix=0; elix < m_elementsinnode; ++elix)
    {
        m_elements[elix]->Accept(visitor, recursiondepth, flags);
    }
    // descend to children--pass down the visitorflags, to save redundant plane testing!
    if (HasChildren())
    for (int childix=0; childix<2; ++childix)
    {
        m_children[childix]->Accept(visitor, recursiondepth, flags);
    }
}

void nSpatialAABBTreeNode::Accept(nOcclusionVisitor &visitor,  int recursiondepth,VisitorFlags flags)
{
    // check our bounding box--is it visible?  Also updates the active flags
    flags = visitor.VisibilityTest(this->m_bbox, flags);
    if (!flags.TestResult())
        return;

    // check each element in this node individually
    for (int elix=0; elix < m_elementsinnode; ++elix)
    {
        m_elements[elix]->Accept(visitor, recursiondepth, flags);
    }
    // descend to children--pass down the visitorflags, to save redundant plane testing!
    if (HasChildren())
    for (int childix=0; childix<2; ++childix)
    {
        m_children[childix]->Accept(visitor, recursiondepth, flags);
    }
}

nSpatialAABBTree::nSpatialAABBTree()
: nSpatialHierarchyElement(), m_treenodes(NULL)
{
    // start with a single empty tree node
    m_treenodes = alloc_node();
}

nSpatialAABBTree::~nSpatialAABBTree()
{
    // trash our nodes
    ClearNodes();
}

void nSpatialAABBTree::ClearNodes()
{
    n_assert(m_treenodes);
    m_treenodes->ClearNodes(this);
    free_node(m_treenodes);
}

nSpatialAABBTreeNode *nSpatialAABBTree::alloc_node()
{
    return n_new(nSpatialAABBTreeNode);
}

void nSpatialAABBTree::free_node(nSpatialAABBTreeNode *freeme)
{
    n_delete(freeme);
}


nSpatialAABBTreeNode *nSpatialAABBTree::FindSuitableNode(const vector3 &v_center)
{
    float elementposcomponents[3] = {v_center.x, v_center.y, v_center.z };

    // walk down until we hit a leaf node
    nSpatialAABBTreeNode *walknode = m_treenodes;
    while (walknode->HasChildren())
    {
        // which child?
        if (elementposcomponents[walknode->m_splitaxis] < walknode->m_splitposition)
        {
            walknode = walknode->m_children[0];
        }
        else
        {
            walknode = walknode->m_children[1];
        }
    }

	return walknode;
}


void nSpatialAABBTree::AddElement(nSpatialElement *appendme)
{
    n_assert(appendme->HasAABB());
	nSpatialAABBTreeNode *walknode = FindSuitableNode(appendme->GetAABB().center());

    // stuff into that leaf node 
    n_assert(walknode->m_elementsinnode < nSpatialAABBTreeNode::MAX_ELEMENTSPERNODE);
    walknode->AddElement(appendme);

    // if it is nearly full, add children and split
    if (walknode->m_elementsinnode > nSpatialAABBTreeNode::MAX_ELEMENTSPERNODE-2)
    {
        walknode->Subdivide(this);
    }
}

void nSpatialAABBTree::RemoveElement(nSpatialElement *removeme)
{
    // find the node in the tree that contains this elements, and remove it
    nSpatialAABBTreeNode *containingnode = m_treenodes->FindElement(removeme);

    n_assert(containingnode);
    containingnode->RemoveElement(removeme);
    containingnode->RecomputeElementCount();
    containingnode->RecomputeBoundingBox();
}


void nSpatialAABBTree::MoveElement(nSpatialElement *moveme, const bbox3 &newbox)
{
	//in which node the element is
	nSpatialAABBTreeNode *actualnode = m_treenodes->FindElement(moveme);
	n_assert(actualnode);

	//in which node element should be
	nSpatialAABBTreeNode *suitablenode = FindSuitableNode(newbox.center());

	//element remains in actual node -> just recalc node bbox
	if (actualnode==suitablenode)
	{
		moveme->SetAABB(newbox);
		actualnode->RecomputeBoundingBox();
	}
	//element moves under another node
	else
	{
	    actualnode->RemoveElement(moveme);
		actualnode->RecomputeElementCount();
		actualnode->RecomputeBoundingBox();

		moveme->SetAABB(newbox);
		suitablenode->AddElement(moveme);
	    if (suitablenode->m_elementsinnode > nSpatialAABBTreeNode::MAX_ELEMENTSPERNODE-2)
		{
			suitablenode->Subdivide(this);
		}
	}
}


void nSpatialAABBTree::Accept(nVisibilityVisitor &visitor, int recursiondepth, VisitorFlags flags)
{
    m_treenodes->Accept(visitor, recursiondepth, flags);
}

void nSpatialAABBTree::Accept(nSpatialVisitor &visitor, int recursiondepth, VisitorFlags flags)
{
    m_treenodes->Accept(visitor, recursiondepth, flags);
}

void nSpatialAABBTree::Accept(nOcclusionVisitor &visitor, int recursiondepth, VisitorFlags flags)
{
    m_treenodes->Accept(visitor, recursiondepth, flags);
}
