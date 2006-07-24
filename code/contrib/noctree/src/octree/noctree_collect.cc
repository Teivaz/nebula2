//-------------------------------------------------------------------
//  noctree_collect.cc
//  (C) 1999 A.Weissflog
//-------------------------------------------------------------------
#include "kernel/nenv.h"
#include "octree/noctvisitor.h"
#include "octree/noctree.h"
#include "gfx2/ngfxserver2.h"

//-------------------------------------------------------------------
/**
    Collect all nodes clipped by given culler.

    @param culler given collection method
    @param ext_array provided collection array
    @param size the maximum number of collected nodes
    @return the number of nodes collected
*/
int nOctree::Collect(nOctVisitor& culler,
                     nOctElement** ext_array, int size)
{
    n_assert(ext_array);
    n_assert(NULL == ext_collect_array);

    this->num_collected     = 0;
    this->ext_collect_array = ext_array;
    this->ext_array_size    = size;

    culler.DoCulling(this);

    this->ext_collect_array = 0;
    this->ext_array_size = 0;
    return num_collected;
}

//-------------------------------------------------------------------
/**
    Trivially collects all elements of the specified node and
    recursively searches its child nodes.

    - 02-Jun-99   floh    created
*/
void nOctree::recurse_collect_nodes_with_flags(nOctNode *on, int c_flags)
{
    nOctElement *oe;
    for (oe = (nOctElement *) on->elm_list.GetHead();
         oe;
         oe = (nOctElement *) oe->GetSucc())
    {
        oe->SetCollectFlags(c_flags);
        // ensures that collect_array doesn't overflow.
        if (this->num_collected < this->ext_array_size)
        {
            this->ext_collect_array[this->num_collected++] = oe;
        }
        else
        {
            n_printf("nOctree::recurse_collect_nodes_with_flags(): Overflow in collect array!\n");
        }
    }
    if (on->c[0]) {
        int i;
        for (i=0; i<8; i++) this->recurse_collect_nodes_with_flags(on->c[i],c_flags);
    }
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
