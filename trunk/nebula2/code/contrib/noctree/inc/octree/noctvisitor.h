#ifndef N_OCTREE_VISITOR_H
#define N_OCTREE_VISITOR_H

#include "kernel/nroot.h"
#include "octree/noctree.h"

//-----------------------------------------------------------------------------
/**
    @class nOctVisitor
    @ingroup NOctreeContribModule
    @brief super class of octree visitor

    All nOctVisitor derived class should declare DoCulling() member function.
*/
class nOctVisitor : public nRoot
{
public:
    nOctVisitor() {};
    virtual ~nOctVisitor() {}
    virtual void DoCulling(nOctree* oct);

    void SetOctree(nOctree* oct);
    nOctree* GetOctree(void) const;

    void Collect(nOctElement *oe);

protected:
    nOctree *octree;

};

#endif /*N_OCTREE_VISITOR_H*/
