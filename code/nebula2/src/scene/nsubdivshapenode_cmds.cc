//------------------------------------------------------------------------------
//  nsubdivshapenode_cmds.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "scene/nsubdivshapenode.h"
#include "kernel/npersistserver.h"

static void n_setsegmentsize(void* slf, nCmd* cmd);
static void n_getsegmentsize(void* slf, nCmd* cmd);
static void n_setmaxdistance(void* slf, nCmd* cmd);
static void n_getmaxdistance(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nsubdivshapenode

    @cppclass
    nSubdivShapeNode

    @superclass
    nshapenode

    @classinfo
    Take input mesh and render subdivided output mesh
    (currently as pointlist only!).
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setsegmentsize_f", 'SSGS', n_setsegmentsize);
    cl->AddCmd("f_getsegmentsize_v", 'GSGS', n_getsegmentsize);
    cl->AddCmd("v_setmaxdistance_f", 'SMXD', n_setmaxdistance);
    cl->AddCmd("f_getmaxdistance_v", 'GMXD', n_getmaxdistance);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    setsegmentsize
    @input
    f(SegmentSize)
    @output
    v
    @info
    Set the desired size of a subdivision segment.
*/
static void
n_setsegmentsize(void* slf, nCmd* cmd)
{
    nSubdivShapeNode* self = (nSubdivShapeNode*) slf;
    self->SetSegmentSize(cmd->In()->GetF());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getsegmentsize
    @input
    v
    @output
    f(SegmentSize)
    @info
    Get the desired size of a subdivision segment.
*/
static void
n_getsegmentsize(void* slf, nCmd* cmd)
{
    nSubdivShapeNode* self = (nSubdivShapeNode*) slf;
    cmd->Out()->SetF(self->GetSegmentSize());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setmaxdistance
    @input
    f(MaxDistance)
    @output
    v
    @info
    Set maximum distance for subdivision. Triangles which are farther away
    will be ignored.
*/
static void
n_setmaxdistance(void* slf, nCmd* cmd)
{
    nSubdivShapeNode* self = (nSubdivShapeNode*) slf;
    self->SetMaxDistance(cmd->In()->GetF());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getmaxdistance
    @input
    v
    @output
    f(MaxDistance)
    @info
    Get maximum distance for subdivision.
*/
static void
n_getmaxdistance(void* slf, nCmd* cmd)
{
    nSubdivShapeNode* self = (nSubdivShapeNode*) slf;
    cmd->Out()->SetF(self->GetMaxDistance());
}

//------------------------------------------------------------------------------
/**
*/
bool
nSubdivShapeNode::SaveCmds(nPersistServer* ps)
{
    if (nShapeNode::SaveCmds(ps))
    {
        nCmd* cmd;

        //--- setsegmentsize ---
        cmd = ps->GetCmd(this, 'SSGS');
        cmd->In()->SetF(this->GetSegmentSize());
        ps->PutCmd(cmd);

        //--- setmaxdistance ---
        cmd = ps->GetCmd(this, 'SMXD');
        cmd->In()->SetF(this->GetMaxDistance());
        ps->PutCmd(cmd);

        return true;
    }
    return false;
}
