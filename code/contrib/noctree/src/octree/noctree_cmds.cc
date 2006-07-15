//-------------------------------------------------------------------
//  misc/noctree_dispatch.cc
//  This file was machine generated.
//  (C) 2000 A.Weissflog/A.Flemming
//-------------------------------------------------------------------
#include "octree/noctree.h"
#include "octree/noctvisitor.h"

static void n_setsubdivnum(void *, nCmd *);
static void n_getsubdivnum(void *, nCmd *);
static void n_setvisualize(void *, nCmd *);
static void n_getvisualize(void *, nCmd *);
static void n_collect(void*, nCmd*);

//------------------------------------------------------------------------------
/**
    @scriptclass
    noctree

    @superclass
    nroot

    @classinfo
    The noctree class is an octree space partitioner. Elements
    defined by position and radius are appended, the octree takes
    care of sorting them into cascading space cubes which enable a more
    optimal search in 3d space.
*/
void n_initcmds(nClass *cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setsubdivnum_i",'SSDN',n_setsubdivnum);
    cl->AddCmd("i_getsubdivnum_v",'GSDN',n_getsubdivnum);
    cl->AddCmd("v_setvisualize_b",'SVIS',n_setvisualize);
    cl->AddCmd("b_getvisualize_v",'GVIS',n_getvisualize);
    cl->AddCmd("i_collect_o",'COLL',n_collect);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    setsubdivnum

    @input
    i (NumElements)

    @output
    v

    @info
    Defines the maximum allowed number of elements in an
    octree node before it will be subdivided and its
    elements spread over the new subcubes.
    Default: 4
*/
static void n_setsubdivnum(void *o, nCmd *cmd)
{
    nOctree *self = (nOctree *) o;
    self->SetSubdivNum(cmd->In()->GetI());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getsubdivnum

    @input
    v

    @output
    i (NumElements)

    @info
    Returns the by 'setsubdivnum' preset value.
*/
static void n_getsubdivnum(void *o, nCmd *cmd)
{
    nOctree *self = (nOctree *) o;
    cmd->Out()->SetI(self->GetSubdivNum());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setvisualize

    @input
    b (Visualize)

    @output
    v

    @info
    Switches the visualize-mode of the octrees on/off.
    That means lines are drawn to visualize the normaly
    invisible octree cubes.
*/
static void n_setvisualize(void *o, nCmd *cmd)
{
    nOctree *self = (nOctree *) o;
    self->SetVisualize(cmd->In()->GetB());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getvisualize

    @input
    v

    @output
    b (Visualize)

    @info
    Queries the actual value of the visualize-flag.
*/
static void n_getvisualize(void *o, nCmd *cmd)
{
    nOctree *self = (nOctree *) o;
    cmd->Out()->SetB(self->GetVisualize());
}
//------------------------------------------------------------------------------
/**
    @cmd
    collect

    @input
    o (nOctVisitor*)

    @output
    i (number of objects collected)

    @info
    Determines the number of objects inside the provided culling area.
*/
static void n_collect(void *o, nCmd *cmd)
{
    nOctree *self = (nOctree *) o;
    nRef<nOctVisitor> rCuller = (nOctVisitor*)cmd->In()->GetO();
    const short maxNumObjs = self->GetRoot()->all_num_elms;
    nOctElement** objectsToRender = n_new_array( nOctElement*, maxNumObjs ); // this could no doubt be made more efficient
    cmd->Out()->SetI( self->Collect( *rCuller.get(), objectsToRender, maxNumObjs ) );
    n_delete_array(objectsToRender);
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
