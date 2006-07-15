#ifndef N_SPATIALAABBTREE_H
#define N_SPATIALAABBTREE_H

#include "nspatialelements.h"

class nSpatialAABBTreeNode;

/**
 *  @class nSpatialAABBTREE
 *  @ingroup NSpatialDBContribModule
 *  @brief A collection of elements stored in an AABB tree
 *
 * This is the SpatialDB representation of an AABB tree, which is a spatial
 * data structure that manages objects using a set of nested bounding boxes.
 * Any node in the tree has a bounding box that completely contains all of its
 * children nodes, which makes it easy and fast to cull away large sections of
 * the tree without having to check every object.
 *
 * Typically a node contains some spatial objects and several child nodes.  There
 * exist many methods of splitting up object sets into child nodes; see
 * "Dynamic Spatial Partitioning for Real-Time Visibility Determination," by
 * Joshua Shagam and Joseph J. Pfeiffer, Jr. for a good survey of the various
 * nesting heuristics.  This tree simply picks a single splitting point for each
 * node and then sorts objects into the child nodes by examining their center
 * relative to that split point.
 */
class nSpatialAABBTree : public nSpatialHierarchyElement
{
public:
    nSpatialAABBTree();
    ~nSpatialAABBTree();

    /// Clear out spatial element referencesw.  Elements are not deleted!
    void ClearNodes();

    /// Add an element to the set of elements
    void AddElement(nSpatialElement *appendme);
    /// Remove a given element
    void RemoveElement(nSpatialElement *removeme);
    /// Move a given element
    void MoveElement(nSpatialElement *moveme, const bbox3 &newbox);

    /// Process the list of elements
    virtual void Accept(nVisibilityVisitor &visitor, int recursiondepth, VisitorFlags flags);
    /// Process the list of elements
    virtual void Accept(nSpatialVisitor &visitor, int recursiondepth, VisitorFlags flags);
    /// Process the list of elements
    virtual void Accept(nOcclusionVisitor &visitor, int recursiondepth, VisitorFlags flags);

    // node management functions--should break this out into a separate class or something...
    // allocate a tree node
    nSpatialAABBTreeNode *alloc_node();

    // free a tree node
    void free_node(nSpatialAABBTreeNode *freeme);

protected:
    // the tree root
    nSpatialAABBTreeNode *m_treenodes;

    // other tree operations
	nSpatialAABBTreeNode *FindSuitableNode(const vector3 &v_center);
    void collapse_node(nSpatialAABBTreeNode *collapseme);
    void split_node(nSpatialAABBTreeNode *splitme);
    void balance_tree(nSpatialAABBTreeNode *treeroot);
};

#endif