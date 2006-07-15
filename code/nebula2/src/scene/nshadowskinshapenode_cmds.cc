//------------------------------------------------------------------------------
//  nshadowskinshapenode_cmds.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "scene/nshadowskinshapenode.h"
#include "kernel/npersistserver.h"

static void n_setmesh(void* slf, nCmd* cmd);
static void n_getmesh(void* slf, nCmd* cmd);
static void n_setskinanimator(void* slf, nCmd* cmd);
static void n_getskinanimator(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nshadownode
    
    @cppclass
    nShadowSkinShapeNode
    
    @superclass
    ntransformnode

    @classinfo
    This one can render shadow from a simple static mesh.
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setmesh_s",        'SSHM', n_setmesh);
    cl->AddCmd("s_getmesh_v",        'GSHM', n_getmesh);
    cl->AddCmd("v_setskinanimator_s",           'SSKA', n_setskinanimator);
    cl->AddCmd("s_getskinanimator_v",           'GSKA', n_getskinanimator);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    setmesh
    @input
    s(MeshResource)
    @output
    v
    @info
    Set the name of the mesh resource.
*/
static void
n_setmesh(void* slf, nCmd* cmd)
{
    nShadowSkinShapeNode* self = (nShadowSkinShapeNode*) slf;
    self->SetMesh(cmd->In()->GetS());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getmesh
    @input
    v
    @output
    s(MeshResource)
    @info
    Get the name of the mesh resource.
*/
static void
n_getmesh(void* slf, nCmd* cmd)
{
    nShadowSkinShapeNode* self = (nShadowSkinShapeNode*) slf;
    cmd->Out()->SetS(self->GetMesh().Get());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setskinanimator
    @input
    s(RelPath)
    @output
    v
    @info
    Set relative path to a skin animator object.
*/
static void
n_setskinanimator(void* slf, nCmd* cmd)
{
    nShadowSkinShapeNode* self = (nShadowSkinShapeNode*) slf;
    self->SetSkinAnimator(cmd->In()->GetS());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getskinanimator
    @input
    v
    @output
    s(RelPath)
    @info
    Get relative path to a skin animator object.
*/
static void
n_getskinanimator(void* slf, nCmd* cmd)
{
    nShadowSkinShapeNode* self = (nShadowSkinShapeNode*) slf;
    cmd->Out()->SetS(self->GetSkinAnimator());
}

//------------------------------------------------------------------------------
/**
*/
bool
nShadowSkinShapeNode::SaveCmds(nPersistServer* ps)
{
    if (nTransformNode::SaveCmds(ps))
    {
        nCmd* cmd;

        // setshadowmesh
        cmd = ps->GetCmd(this, 'SSHM');
        cmd->In()->SetS(this->GetMesh().Get());
        ps->PutCmd(cmd);

        // setskinanimator
        cmd = ps->GetCmd(this, 'SSKA');
        cmd->In()->SetS(this->GetSkinAnimator());
        ps->PutCmd(cmd);

        return true;
    }
    return false;
}
