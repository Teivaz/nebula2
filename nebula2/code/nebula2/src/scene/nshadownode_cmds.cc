//------------------------------------------------------------------------------
//  nshadownode_cmds.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "scene/nshadownode.h"
#include "kernel/npersistserver.h"

static void n_setmesh(void* slf, nCmd* cmd);
static void n_getmesh(void* slf, nCmd* cmd);
static void n_setgroupindex(void* slf, nCmd* cmd);
static void n_getgroupindex(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nshadownode

    @cppclass
    nShadowNode

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
    cl->AddCmd("v_setgroupindex_i",  'SSMG', n_setgroupindex);
    cl->AddCmd("i_getgroupindex_i",  'GSMG', n_getgroupindex);
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
    nShadowNode* self = (nShadowNode*) slf;
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
    nShadowNode* self = (nShadowNode*) slf;
    cmd->Out()->SetS(self->GetMesh().Get());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setgroupindex
    @input
    i(GroupIndex)
    @output
    v
    @info
    Set the group index inside the mesh resource.
*/
static void
n_setgroupindex(void* slf, nCmd* cmd)
{
    nShadowNode* self = (nShadowNode*) slf;
    self->SetGroupIndex(cmd->In()->GetI());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getgroupindex
    @input
    v
    @output
    i(GroupIndex)
    @info
    Get the group index inside the mesh resource.
*/

static void
n_getgroupindex(void* slf, nCmd* cmd)
{
    nShadowNode* self = (nShadowNode*) slf;
    cmd->Out()->SetI(self->GetGroupIndex());
}

//------------------------------------------------------------------------------
/**
*/
bool
nShadowNode::SaveCmds(nPersistServer* ps)
{
    if (nTransformNode::SaveCmds(ps))
    {
        nCmd* cmd;

        //setshadowmesh
        cmd = ps->GetCmd(this, 'SSHM');
        cmd->In()->SetS(this->GetMesh().Get());
        ps->PutCmd(cmd);

        //setshadowmeshgroup
        cmd = ps->GetCmd(this, 'SSMG');
        cmd->In()->SetI(this->GetGroupIndex());
        ps->PutCmd(cmd);
        return true;
    }
    return false;
}
