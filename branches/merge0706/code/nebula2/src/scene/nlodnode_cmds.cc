//------------------------------------------------------------------------------
//  ntransformnode_cmds.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "scene/nlodnode.h"
#include "kernel/npersistserver.h"

static void n_appendthreshold(void* slf, nCmd* cmd);
static void n_getthreshold(void* slf, nCmd* cmd);
static void n_setmindistance(void* slf, nCmd* cmd);
static void n_getmindistance(void* slf, nCmd* cmd);
static void n_setmaxdistance(void* slf, nCmd* cmd);
static void n_getmaxdistance(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nlodnode

    @cppclass
    nLodNode

    @superclass
    ntransformnode

    @classinfo
    A level of detail scene node.
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_appendthreshold_f",   'ATHR', n_appendthreshold);
    cl->AddCmd("f_getthreshold_i",      'GTHR', n_getthreshold);
    cl->AddCmd("v_setmindistance_f",    'SMID', n_setmindistance);
    cl->AddCmd("f_getmindistance_v",    'GMID', n_getmindistance);
    cl->AddCmd("v_setmaxdistance_f",    'SMAD', n_setmaxdistance);
    cl->AddCmd("f_getmaydistance_v",    'GMAD', n_getmaxdistance);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    appendthreshold
    @input
    f(Threshold)
    @output
    v
    @info
    Append camera distance threshold when the child node at index is switched on and its predecessor switched off
*/
static void
n_appendthreshold(void* slf, nCmd* cmd)
{
    nLodNode* self = (nLodNode*) slf;
    self->AppendThreshold(cmd->In()->GetF());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getthreshold
    @input
    i(Index)
    @output
    f(Threshold)
    @info
    Get camera distance threshold when the child node at index is switched on and its predecessor switched off
*/
static void
n_getthreshold(void* slf, nCmd* cmd)
{
    nLodNode* self = (nLodNode*) slf;
    cmd->Out()->SetF(self->GetThreshold(cmd->In()->GetI()));
}

//------------------------------------------------------------------------------
/**
    @cmd
    setmindistance
    @input
    f(Distance)
    @output
    v
    @info
    Set the minimum camera distance where the node is shown
*/
static void
n_setmindistance(void* slf, nCmd* cmd)
{
    nLodNode* self = (nLodNode*) slf;
    self->SetMinDistance(cmd->In()->GetF());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getmindistance
    @input
    v
    @output
    f(Distance)
    @info
    Get the minimum camera distance where the node is shown
*/
static void
n_getmindistance(void* slf, nCmd* cmd)
{
    nLodNode* self = (nLodNode*) slf;
    cmd->Out()->SetF(self->GetMinDistance());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setmaxdistance
    @input
    f(Distance)
    @output
    v
    @info
    Set the maximum camera distance where the node is shown
*/
static void
n_setmaxdistance(void* slf, nCmd* cmd)
{
    nLodNode* self = (nLodNode*) slf;
    self->SetMaxDistance(cmd->In()->GetF());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getmaxdistance
    @input
    v
    @output
    f(Distance)
    @info
    Get the maximum camera distance where the node is shown
*/
static void
n_getmaxdistance(void* slf, nCmd* cmd)
{
    nLodNode* self = (nLodNode*) slf;
    cmd->Out()->SetF(self->GetMaxDistance());
}

//------------------------------------------------------------------------------
/**
*/
bool
nLodNode::SaveCmds(nPersistServer* ps)
{
    if (nSceneNode::SaveCmds(ps))
    {
        nCmd* cmd;
        static const vector3 oneVec(1.0f, 1.0f, 1.0f);

        //--- setthreshold ---
        int index;
        for (index = 0; index < this->thresholds.Size(); index++)
        {
            cmd = ps->GetCmd(this, 'ATHR');
            cmd->In()->SetF(this->GetThreshold(index));
            ps->PutCmd(cmd);
        }

        //--- setmindistance ---
        cmd = ps->GetCmd(this, 'SMID');
        cmd->In()->SetF(this->GetMinDistance());
        ps->PutCmd(cmd);

        //--- setmaxdistance ---
        cmd = ps->GetCmd(this, 'SMAD');
        cmd->In()->SetF(this->GetMaxDistance());
        ps->PutCmd(cmd);

        return true;
    }
    return false;
}
