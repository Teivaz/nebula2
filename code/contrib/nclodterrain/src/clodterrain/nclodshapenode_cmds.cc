//------------------------------------------------------------------------------
//  nCLODShapeNode_cmds.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "clodterrain/nclodshapenode.h"
#include "kernel/npersistserver.h"

static void n_setterrainname(void* slf, nCmd* cmd);
static void n_getterrainname(void* slf, nCmd* cmd);
static void n_settqtname(void* slf, nCmd* cmd);
static void n_gettqtname(void* slf, nCmd* cmd);
static void n_setterrainresourceloader(void* slf, nCmd* cmd);
static void n_getterrainresourceloader(void* slf, nCmd* cmd);
static void n_setscreenspaceerror(void* self, nCmd *cmd);
static void n_getscreenspaceerror(void* self, nCmd *cmd);
static void n_setcollisionspace(void *slf, nCmd *cmd);
static void n_getcollisionspace(void *slf, nCmd *cmd);
static void n_begindetailtextures(void *slf, nCmd *cmd);
static void n_setdetailtexture(void *slf, nCmd *cmd);
static void n_enddetailtextures(void *slf, nCmd *cmd);
static void n_setdetailscale(void *slf, nCmd *cmd);
static void n_getdetailscale(void *slf, nCmd *cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nCLODShapeNode

    @cppclass
    nShapeNode
    
    @superclass
    nmaterialnode

    @classinfo
    This one can render a chunked lod terrain.
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setterrainname_s",            'STRN', n_setterrainname);
    cl->AddCmd("s_getterrainname_v",            'GTRN', n_getterrainname);
    cl->AddCmd("v_settqtname_s",                'STQN', n_settqtname);
    cl->AddCmd("s_gettqtname_v",                'GTQN', n_gettqtname);
    cl->AddCmd("v_setterrainresourceloader_s",  'SMRL', n_setterrainresourceloader);
    cl->AddCmd("s_getterrainresourceloader_v",  'GMRL', n_getterrainresourceloader);
    cl->AddCmd("v_setscreenspaceerror_f",       'SSSE', n_setscreenspaceerror);
    cl->AddCmd("f_getscreenspaceerror_v",       'GSSE', n_getscreenspaceerror);
    cl->AddCmd("v_setcollisionspace_s",         'SCSP', n_setcollisionspace);
    cl->AddCmd("s_getcollisionspace_v",         'GCSP', n_getcollisionspace);
    cl->AddCmd("v_begindetailtextures_i",       'BDTX', n_begindetailtextures);
    cl->AddCmd("v_setdetailtexture_is",         'SDTX', n_setdetailtexture);
    cl->AddCmd("v_enddetailtextures_v",         'EDTX', n_enddetailtextures);
    cl->AddCmd("v_setdetailscale_f",            'SDSC', n_setdetailscale);
    cl->AddCmd("f_getdetailscale_v",            'GDSC', n_getdetailscale);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    setterrainname
    @input
    s(MeshResource)
    @output
    v
    @info
    Set the name of the terrain resource.
*/
static void
n_setterrainname(void* slf, nCmd* cmd)
{
    nCLODShapeNode* self = (nCLODShapeNode*) slf;
    self->SetTerrainName(cmd->In()->GetS());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getterrainname
    @input
    v
    @output
    s(MeshResource)
    @info
    Get the name of the terrain resource.
*/
static void
n_getterrainname(void* slf, nCmd* cmd)
{
    nCLODShapeNode* self = (nCLODShapeNode*) slf;
    cmd->Out()->SetS(self->GetTerrainName());
}


//------------------------------------------------------------------------------
/**
    @cmd
    settqtname
    @input
    s(MeshResource)
    @output
    v
    @info
    Set the name of the terrain texture quadtree.
*/
static void
n_settqtname(void* slf, nCmd* cmd)
{
    nCLODShapeNode* self = (nCLODShapeNode*) slf;
    self->SetTqtName(cmd->In()->GetS());
}

//------------------------------------------------------------------------------
/**
    @cmd
    gettqtname
    @input
    v
    @output
    s(MeshResource)
    @info
    Get the name of the terrain texture quadtree.
*/
static void
n_gettqtname(void* slf, nCmd* cmd)
{
    nCLODShapeNode* self = (nCLODShapeNode*) slf;
    cmd->Out()->SetS(self->GetTqtName());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setterrainresourceloader
    @input
    o(ResourceLoader)
    @output
    v
    @info
    Set the NOH path for the mesh resource loader.
*/
static void
n_setterrainresourceloader(void* slf, nCmd* cmd)
{
    nCLODShapeNode* self = (nCLODShapeNode*) slf;
    self->SetTerrainResourceLoader(cmd->In()->GetS());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getterrainresourceloader
    @input
    v
    @output
    o(ResourceLoader)
    @info
    Get the NOH path for the mesh resource loader.
*/
static void
n_getterrainresourceloader(void* slf, nCmd* cmd)
{
    nCLODShapeNode* self = (nCLODShapeNode*) slf;
    cmd->Out()->SetS(self->GetTerrainResourceLoader());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setscreenspaceerror
    @input
    f (error)
    @output
    v
    @info
    Set the allowed screen space error, in pixels
*/
static void
n_setscreenspaceerror(void* slf, nCmd* cmd)
{
    nCLODShapeNode* self = (nCLODShapeNode*) slf;
    self->SetScreenSpaceError(cmd->In()->GetF());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getscreenspaceerror
    @input
    v
    @output
    f (error value)
    @info
    Get the current value of the screen space error in pixels
*/
static void
n_getscreenspaceerror(void* slf, nCmd* cmd)
{
    nCLODShapeNode* self = (nCLODShapeNode*) slf;
    cmd->Out()->SetF((float)self->GetScreenSpaceError());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setcollisionspace
    @input
    s (path to collision space)
    @output
    v
    @info
    Set the collision space to use
*/
static void
n_setcollisionspace(void* slf, nCmd* cmd)
{
    nCLODShapeNode* self = (nCLODShapeNode*) slf;
    self->SetCollisionSpace(cmd->In()->GetS());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getscreenspaceerror
    @input
    v
    @output
    s (current collision space)
    @info
    Get the current value of the screen space error in pixels
*/
static void
n_getcollisionspace(void* slf, nCmd* cmd)
{
    nCLODShapeNode* self = (nCLODShapeNode*) slf;
    cmd->Out()->SetS(self->GetCollisionSpace());
}

//------------------------------------------------------------------------------
/**
    @cmd
    begindetailtextures
    @input
    i (number of detail textures)

    @output
    v
    @info
    Setup the process to specify detail textures used for splatting.

*/
static void
n_begindetailtextures(void* slf, nCmd* cmd)
{
    nCLODShapeNode* self = (nCLODShapeNode*) slf;
    self->BeginDetailTextures(cmd->In()->GetI());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setdetailtexture
    @input
    i (index of this detail texture)
    s (path to texture resource)

    @output
    v
    @info
    Specify the resource to use for a specific detail texture.
    You need to specify EVERY detail texture, even if you aren't using it.
*/
static void
n_setdetailtexture(void* slf, nCmd* cmd)
{
    nCLODShapeNode* self = (nCLODShapeNode*) slf;
    unsigned int detailtexindex = cmd->In()->GetI();
    const char *detailtexname = cmd->In()->GetS();
    self->SetDetailTexture(detailtexindex, detailtexname);
}

//------------------------------------------------------------------------------
/**
    @cmd
    enddetailtextures
    @input
    v

    @output
    v
    @info
    Indicate that you are done specifying the detail textures.  Also checks that
    every detail texture index has a valid texture.
*/
static void
n_enddetailtextures(void* slf, nCmd* cmd)
{
    nCLODShapeNode* self = (nCLODShapeNode*) slf;
    self->EndDetailTextures();
}


//------------------------------------------------------------------------------
/**
    @cmd
    setdetailscale
    @input
    f (scale)
    @output
    v
    @info
    Set the scaling of detail textures; texture (u,v) is equal to vertex (x,y) * scale
*/
static void
n_setdetailscale(void* slf, nCmd* cmd)
{
    nCLODShapeNode* self = (nCLODShapeNode*) slf;
    self->SetDetailScale(cmd->In()->GetF());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getdetailscale
    @input
    v
    @output
    f (scale)
    @info
    Get the scaling of detail textures; texture (u,v) is equal to vertex (x,y) * scale
*/
static void
n_getdetailscale(void* slf, nCmd* cmd)
{
    nCLODShapeNode* self = (nCLODShapeNode*) slf;
    cmd->Out()->SetF((float)self->GetDetailScale());
}

//------------------------------------------------------------------------------
/**
    - 16-Mar-05 kims Fixed to save all commands. Thanks to Ville Ruusutie for the patch.
*/
bool
nCLODShapeNode::SaveCmds(nPersistServer* ps)
{
    if (nMaterialNode::SaveCmds(ps))
    {
        nCmd* cmd;

        //--- setmesh ---
        cmd = ps->GetCmd(this, 'SMSN');
        cmd->In()->SetS(this->GetTerrainName());
        ps->PutCmd(cmd);

        //--- settqt ---
        cmd = ps->GetCmd(this, 'STQN');
        cmd->In()->SetS(this->GetTqtName());
        ps->PutCmd(cmd);

        //--- setscreenspaceerror ---
        cmd = ps->GetCmd(this, 'SSSE');
        cmd->In()->SetF((float)this->GetScreenSpaceError());
        ps->PutCmd(cmd);

        //--- setcollisionspace ---
        const char* csName = this->GetCollisionSpace();
        // only when we have a collision space
        if (csName)
        {
            cmd = ps->GetCmd(this, 'SCSP');
            cmd->In()->SetS(csName);
            ps->PutCmd(cmd);
        }

        //--- setmeshresourceloader ---
        const char* rlName = this->GetTerrainResourceLoader();
        // only when we have a resource loader
        if (rlName)
        {
            cmd = ps->GetCmd(this, 'SMRL');
            cmd->In()->SetS(rlName);
            ps->PutCmd(cmd);
        }

        //--- begindetailtextures ---
        cmd = ps->GetCmd(this, 'BDTX');
        cmd->In()->SetI(this->detailTextures.Size());
        ps->PutCmd(cmd);

        //--- setdetailtexture ---
        for (int i=0; i<this->detailTextures.Size(); i++)
        {
            cmd = ps->GetCmd(this, 'SDTX');
            cmd->In()->SetI(i);
            cmd->In()->SetS(this->detailTextures.At(i)->GetFilename().Get());
            ps->PutCmd(cmd);
        }

        //--- enddetailtextures ---
        cmd = ps->GetCmd(this, 'EDTX');
        ps->PutCmd(cmd);

        //--- setdetailscale ---
        cmd = ps->GetCmd(this, 'SDSC');
        cmd->In()->SetF((float)this->GetDetailScale());
        ps->PutCmd(cmd);

        return true;
    }
    return false;
}

