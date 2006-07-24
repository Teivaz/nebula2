#include "octree/noctvisitor.h"

nNebulaClass(nOctVisitor, "nroot");

//-----------------------------------------------------------------------------
/**
*/
void nOctVisitor::DoCulling(nOctree* oct)
{
    this->SetOctree(oct);
}

//-----------------------------------------------------------------------------
/**
    Specify octree to visit for culling.
*/
void nOctVisitor::SetOctree(nOctree* oct)
{
    octree = oct;
}

//-----------------------------------------------------------------------------
/**
    Retrieves octree.
*/
nOctree* nOctVisitor::GetOctree() const
{
    return octree;
}

//-----------------------------------------------------------------------------
/**
*/
void nOctVisitor::Collect(nOctElement *oe)
{
    if (octree->num_collected < octree->ext_array_size)
    {
        octree->ext_collect_array[octree->num_collected++] = oe;
    }
    else
    {
        n_printf("nOctVisitor::collect(): Overflow in collect array!\n");
    }
}
