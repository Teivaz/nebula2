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
static void n_setmeshresourceloader(void* slf, nCmd* cmd);
static void n_getmeshresourceloader(void* slf, nCmd* cmd);
static void n_setmeshusage(void* slf, nCmd* cmd);
static void n_getmeshusage(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nshapenode

    @cppclass
    nShapeNode
    
    @superclass
    nmaterialnode

    @classinfo
    This one can render a simple static mesh.
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setmesh_s",               'SMSH', n_setmesh);
    cl->AddCmd("s_getmesh_v",               'GMSH', n_getmesh);
    cl->AddCmd("v_setgroupindex_i",         'SGRI', n_setgroupindex);
    cl->AddCmd("i_getgroupindex_v",         'GGRI', n_getgroupindex);
    cl->AddCmd("v_setmeshresourceloader_s", 'SMRL', n_setmeshresourceloader);
    cl->AddCmd("s_getmeshresourceloader_v", 'GMRL', n_getmeshresourceloader);
    cl->AddCmd("v_setmeshusage_s",          'SMSU', n_setmeshusage);
    cl->AddCmd("s_getmeshusage_v",          'GMSU', n_getmeshusage);
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
    setmeshresourceloader
    @input
    o(ResourceLoader)
    @output
    v
    @info
    Set the NOH path for the mesh resource loader.
*/
static void
n_setmeshresourceloader(void* slf, nCmd* cmd)
{
    nShapeNode* self = (nShapeNode*) slf;
    self->SetMeshResourceLoader(cmd->In()->GetS());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getmeshresourceloader
    @input
    v
    @output
    o(ResourceLoader)
    @info
    Get the NOH path for the mesh resource loader.
*/
static void
n_getmeshresourceloader(void* slf, nCmd* cmd)
{
    nShapeNode* self = (nShapeNode*) slf;
    cmd->Out()->SetS(self->GetMeshResourceLoader());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setmeshusage
    @input
    s(usage flag string)
    @output
    v
    @info
    Set the usage flags for the mesh.
*/
static void
n_setmeshusage(void* slf, nCmd* cmd)
{
    nShapeNode* self = (nShapeNode*) slf;
    const int flags = nMesh2::ConvertUsageStringToFlags( cmd->In()->GetS() );
    self->SetMeshUsage(flags);
}


//------------------------------------------------------------------------------
/**
    @cmd
    getmeshusage
    @input
    v
    @output
    s(usage flag string)
    @info
    Gets the usage flags for the mesh.
*/
static void
n_getmeshusage(void* slf, nCmd* cmd)
{
    nShapeNode* self = (nShapeNode*) slf;
    const char* flagString = nMesh2::ConvertUsageFlagsToString( self->GetMeshUsage() ).Get();
    cmd->Out()->SetS(flagString);
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

        //--- setmeshresourceloader ---
        const char* rlName = this->GetMeshResourceLoader();
        // only when we have a resource loader
        if (rlName)
        {
            cmd = ps->GetCmd(this, 'SMRL');
            cmd->In()->SetS(rlName);
            ps->PutCmd(cmd);
        }

        //--- setmeshusage ---
        cmd = ps->GetCmd(this, 'SMSU');
        cmd->In()->SetI(this->GetMeshUsage());
        ps->PutCmd(cmd);

        return true;
    }
    return false;
}

