#ifndef N_OCTREE_VISITOR_H
#define N_OCTREE_VISITOR_H

#include "octree/noctree.h"

//-----------------------------------------------------------------------------
/**
    @class nOctVisitor
    @ingroup NOctreeConribModule
    @brief super class of octree visitor

    All nOctVIsitor derived class should declare DoCulling() member function.
*/
class nOctVisitor
{
public:
    virtual void DoCulling (nOctree* octree) = 0;

};

#endif /*N_OCTREE_VISITOR_H*/
