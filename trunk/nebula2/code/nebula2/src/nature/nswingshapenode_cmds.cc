//------------------------------------------------------------------------------
//  nswingshapenode_cmds.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "nature/nswingshapenode.h"
#include "kernel/npersistserver.h"

static void n_setswingangle(void* slf, nCmd* cmd);
static void n_getswingangle(void* slf, nCmd* cmd);
static void n_setswingtime(void* slf, nCmd* cmd);
static void n_getswingtime(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nswingshapenode

    @cppclass
    nSwingShapeNode

    @superclass
    nshapenode

    @classinfo
    Provides the extra rotation matrix to the shader needed for swinging
    geometry (like trees, grass and leaved).
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setswingangle_f", 'SSWA', n_setswingangle);
    cl->AddCmd("f_getswingangle_v", 'GSWA', n_getswingangle);
    cl->AddCmd("v_setswingtime_f",  'SSWT', n_setswingtime);
    cl->AddCmd("f_getswingtime_v",  'GSWT', n_getswingtime);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    setswingangle
    @input
    f(SwingAngle)
    @output
    v
    @info
    Set the maximum swinging angle in degree.
*/
static void
n_setswingangle(void* slf, nCmd* cmd)
{
    nSwingShapeNode* self = (nSwingShapeNode*) slf;
    self->SetSwingAngle(n_deg2rad(cmd->In()->GetF()));
}

//------------------------------------------------------------------------------
/**
    @cmd
    getswingangle
    @input
    v
    @output
    f(SwingAngle)
    @info
    Get the max swing angle
*/
static void
n_getswingangle(void* slf, nCmd* cmd)
{
    nSwingShapeNode* self = (nSwingShapeNode*) slf;
    cmd->Out()->SetF(n_rad2deg(self->GetSwingAngle()));
}

//------------------------------------------------------------------------------
/**
    @cmd
    setswingtime
    @input
    f(SwingTime)
    @output
    v
    @info
    Set the average swinging time.
*/
static void
n_setswingtime(void* slf, nCmd* cmd)
{
    nSwingShapeNode* self = (nSwingShapeNode*) slf;
    self->SetSwingTime(cmd->In()->GetF());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getswingtime
    @input
    v
    @output
    f(SwingTime)
    @info
    Get the average time angle
*/
static void
n_getswingtime(void* slf, nCmd* cmd)
{
    nSwingShapeNode* self = (nSwingShapeNode*) slf;
    cmd->Out()->SetF(self->GetSwingTime());
}

//------------------------------------------------------------------------------
/**
*/
bool
nSwingShapeNode::SaveCmds(nPersistServer* ps)
{
    if (nShapeNode::SaveCmds(ps))
    {
        nCmd* cmd;

        //--- setswingangle ---
        cmd = ps->GetCmd(this, 'SSWA');
        cmd->In()->SetF(n_rad2deg(this->GetSwingAngle()));
        ps->PutCmd(cmd);

        //--- setswingtime ---
        cmd = ps->GetCmd(this, 'SSWT');
        cmd->In()->SetF(this->GetSwingTime());
        ps->PutCmd(cmd);

        return true;
    }
    return false;
}
