#ifndef N_OCTREE_VISITOR_H
#define N_OCTREE_VISITOR_H

#include "kernel/nroot.h"
#include "octree/noctree.h"

//-----------------------------------------------------------------------------
/**
    @class nOctVisitor
    @ingroup NOctreeConribModule
    @brief super class of octree visitor

    All nOctVisitor derived class should declare DoCulling() member function.
*/
class nOctVisitor : public nRoot
{
public:
    nOctVisitor() {};
    virtual ~nOctVisitor() {}
    virtual void DoCulling (nOctree* octree) { n_assert( 0 ); } // "pure virtual"
};

#endif /*N_OCTREE_VISITOR_H*/
