#define N_IMPLEMENTS nShapeNode
//------------------------------------------------------------------------------
//  nshapenode_cmds.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "scene/nshapenode.h"
#include "kernel/npersistserver.h"

static void n_setmesh(void* slf, nCmd* cmd);
static void n_getmesh(void* slf, nCmd* cmd);
static void n_setgroupindex(void* slf, nCmd* cmd);
static void n_getgroupindex(void* slf, nCmd* cmd);
static void n_setrenderwireframe(void* slf, nCmd* cmd);
static void n_getrenderwireframe(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nshapenode
    
    @superclass
    nmaterialnode

    @classinfo
    This one can render a simple static mesh.
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setmesh_s",            'SMSH', n_setmesh);
    cl->AddCmd("s_getmesh_v",            'GMSH', n_getmesh);
    cl->AddCmd("v_setgroupindex_i",      'SGRI', n_setgroupindex);
    cl->AddCmd("i_getgroupindex_v",      'GGRI', n_getgroupindex);
    cl->AddCmd("v_setrenderwireframe_b", 'SRWF', n_setrenderwireframe);
    cl->AddCmd("b_getrenderwireframe_v", 'GRWF', n_getrenderwireframe);
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
    nShapeNode* self = (nShapeNode*) slf;
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
    nShapeNode* self = (nShapeNode*) slf;
    cmd->Out()->SetS(self->GetMesh());
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
    nShapeNode* self = (nShapeNode*) slf;
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
    nShapeNode* self = (nShapeNode*) slf;
    cmd->Out()->SetI(self->GetGroupIndex());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setrenderwireframe
    @input
    b(WireframeFlag)
    @output
    v
    @info
    Turn wireframe rendering on/off.
*/
static void
n_setrenderwireframe(void* slf, nCmd* cmd)
{
    nShapeNode* self = (nShapeNode*) slf;
    self->SetRenderWireframe(cmd->In()->GetB());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getrenderwireframe
    @input
    v
    @output
    b
    @info
    Get the wireframe render status.
*/
static void
n_getrenderwireframe(void* slf, nCmd* cmd)
{
    nShapeNode* self = (nShapeNode*) slf;
    cmd->Out()->SetB(self->GetRenderWireframe());
}

//------------------------------------------------------------------------------
/**
*/
bool
nShapeNode::SaveCmds(nPersistServer* ps)
{
    if (nMaterialNode::SaveCmds(ps))
    {
        nCmd* cmd;

        //--- setmesh ---
        cmd = ps->GetCmd(this, 'SMSH');
        cmd->In()->SetS(this->GetMesh());
        ps->PutCmd(cmd);

        //--- setgroupindex ---
        cmd = ps->GetCmd(this, 'SGRI');
        cmd->In()->SetI(this->GetGroupIndex());
        ps->PutCmd(cmd);

        return true;
    }
    return false;
}
