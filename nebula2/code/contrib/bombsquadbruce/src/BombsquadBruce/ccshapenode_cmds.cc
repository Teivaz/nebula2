//------------------------------------------------------------------------------
//  ccshapenode_cmds.cc
//  (C) 2004 Rafael Van Daele-Hunt
//------------------------------------------------------------------------------
#include "BombsquadBruce/ccshapenode.h"
#include "kernel/npersistserver.h"

static void n_setlockviewerxz(void* slf, nCmd* cmd);
static void n_getlockviewerxz(void* slf, nCmd* cmd);
static void n_settexcoordscale(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    ccshapenode

    @cppclass
    CCShapeNode
    
    @superclass
    nshapenode

    @classinfo
    Custom Crazy Chipmunk version of nShapeNode
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setlockviewerxz_b", 'SLVX', n_setlockviewerxz);
    cl->AddCmd("v_getlockviewerxz_b", 'GLVX', n_getlockviewerxz);
    cl->AddCmd("v_settexcoordscale_f", 'STCS', n_settexcoordscale);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    setlockviewerxz
    @input
    b(LockViewerXZ)
    @output
    v
    @info
    Set to true if this node's (x,z) position is locked to the viewer.
*/
static void
n_setlockviewerxz(void* slf, nCmd* cmd)
{
    CCShapeNode* self = (CCShapeNode*) slf;
    self->SetLockViewerXZ(cmd->In()->GetB());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getlockviewerxz
    @input
    @output
    b(LockViewerXZ)
    @info
    Return lockViewerXZ flag.
*/
static void
n_getlockviewerxz(void* slf, nCmd* cmd)
{
    CCShapeNode* self = (CCShapeNode*) slf;
    cmd->Out()->SetB(self->GetLockViewerXZ());
}

//------------------------------------------------------------------------------
/**
    @cmd
    settexcoordscale
    @input
    f(Scale)
    @output
    v
    @info
    The factor by which the texture's (u,v) must be multiplied
    to get world coordinates (that is, the size of the square
    (I assume a square, although it would be easy to extend to
    rectangles) covered by one instance of the texture).
    So a 100x100 mesh on which the texture is tiled 5x5 times
    would have a scale of 100/5 = 20
*/
static void
n_settexcoordscale(void* slf, nCmd* cmd)
{
    CCShapeNode* self = (CCShapeNode*) slf;
    self->SetTexCoordScale(cmd->In()->GetF());
}
//------------------------------------------------------------------------------
/**
*/
bool
CCShapeNode::SaveCmds(nPersistServer* ps)
{
    if (nShapeNode::SaveCmds(ps))
    {
        nCmd* cmd;

        //--- setlockviewerxz ---
        cmd = ps->GetCmd(this, 'SLVX');
        cmd->In()->SetB(this->GetLockViewerXZ());
        ps->PutCmd(cmd);

        //--- settexcoordscale ---
        cmd = ps->GetCmd(this, 'STCS');
        cmd->In()->SetF(this->m_TexCoordScale);
        ps->PutCmd(cmd);

        return true;
    }
    return false;
}

