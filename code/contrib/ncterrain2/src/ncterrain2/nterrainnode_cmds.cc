//------------------------------------------------------------------------------
//  nterrainnode_cmds.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "ncterrain2/nterrainnode.h"
#include "kernel/npersistserver.h"

static void n_setchunkfile(void* slf, nCmd* cmd);
static void n_getchunkfile(void* slf, nCmd* cmd);
static void n_settexquadfile(void* slf, nCmd* cmd);
static void n_gettexquadfile(void* slf, nCmd* cmd);
static void n_setmaxpixelerror(void* slf, nCmd* cmd);
static void n_getmaxpixelerror(void* slf, nCmd* cmd);
static void n_setmaxtexelsize(void* slf, nCmd* cmd);
static void n_getmaxtexelsize(void* slf, nCmd* cmd);
static void n_setterrainscale(void* slf, nCmd* cmd);
static void n_getterrainscale(void* slf, nCmd* cmd);
static void n_setterrainorigin(void* slf, nCmd* cmd);
static void n_getterrainorigin(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nterrainnode
    
    @superclass
    nmaterialnode

    @classinfo
    Render a terrain implementing Thatcher Ulrich's ChunkLOD terrain algorithm.
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setchunkfile_s",          'SCHF', n_setchunkfile);
    cl->AddCmd("s_getchunkfile_v",          'GCHF', n_getchunkfile);
    cl->AddCmd("v_settexquadfile_s",        'STQF', n_settexquadfile);
    cl->AddCmd("s_gettexquadfile_v",        'GTQF', n_gettexquadfile);
    cl->AddCmd("v_setmaxpixelerror_f",      'SMPE', n_setmaxpixelerror);
    cl->AddCmd("f_getmaxpixelerror_v",      'GMPE', n_getmaxpixelerror);
    cl->AddCmd("v_setmaxtexelsize_f",       'SMTS', n_setmaxtexelsize);
    cl->AddCmd("f_getmaxtexelsize_v",       'GMTS', n_getmaxtexelsize);
    cl->AddCmd("v_setterrainscale_f",       'STRS', n_setterrainscale);
    cl->AddCmd("f_getteerainscale_v",       'GTRS', n_getterrainscale);
    cl->AddCmd("v_setterrainorigin_fff",    'STRO', n_setterrainorigin);
    cl->AddCmd("fff_getterrainorigin_v",    'GTRO', n_getterrainorigin);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    setchunkfile
    @input
    s(ChunkFileName)
    @output
    v
    @info
    Set name of .chu file.
*/
static void
n_setchunkfile(void* slf, nCmd* cmd)
{
    nTerrainNode* self = (nTerrainNode*) slf;
    self->SetChunkFile(cmd->In()->GetS());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getchunkfile
    @input
    @output
    s(ChunkFileName);
    @info
    Get name of .chu file.
*/
static void
n_getchunkfile(void* slf, nCmd* cmd)
{
    nTerrainNode* self = (nTerrainNode*) slf;
    cmd->Out()->SetS(self->GetChunkFile());
}

//------------------------------------------------------------------------------
/**
    @cmd
    settexquadfile
    @input
    s(TextureQuadFilename)
    @output
    v
    @info
    Set name of a .tqt2 file which delivers the global terrain texture.
*/
static void
n_settexquadfile(void* slf, nCmd* cmd)
{
    nTerrainNode* self = (nTerrainNode*) slf;
    self->SetTexQuadFile(cmd->In()->GetS());
}

//------------------------------------------------------------------------------
/**
    @cmd
    gettexquadfile
    @input
    v
    @output
    s(TextureQuadFilename)
    @info
    Get name of a .tqt2 file which delivers the global terrain texture.
*/
static void
n_gettexquadfile(void* slf, nCmd* cmd)
{
    nTerrainNode* self = (nTerrainNode*) slf;
    cmd->Out()->SetS(self->GetTexQuadFile());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setmaxpixelerror
    @input
    f(MaxPixelError)
    @output
    v
    @info
    Set maximum pixel error, influences number of triangles rendered.
*/
static void
n_setmaxpixelerror(void* slf, nCmd* cmd)
{
    nTerrainNode* self = (nTerrainNode*) slf;
    self->SetMaxPixelError(cmd->In()->GetF());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getmaxpixelerror
    @input
    v
    @output
    f(MaxPixelError)
    @info
    Get maximum pixel error, influences number of triangles rendered.
*/
static void
n_getmaxpixelerror(void* slf, nCmd* cmd)
{
    nTerrainNode* self = (nTerrainNode*) slf;
    cmd->Out()->SetF(self->GetMaxPixelError());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setmaxtexelsize
    @input
    f(MaxTexelSize)
    @output
    v
    @info
    Set maximum texel size, influences displayed texture resolution.
*/
static void
n_setmaxtexelsize(void* slf, nCmd* cmd)
{
    nTerrainNode* self = (nTerrainNode*) slf;
    self->SetMaxTexelSize(cmd->In()->GetF());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getmaxtexelsize
    @input
    v
    @output
    f(MaxTexelSize)
    @info
    Get maximum texel size, influences displayed texture resolution.
*/
static void
n_getmaxtexelsize(void* slf, nCmd* cmd)
{
    nTerrainNode* self = (nTerrainNode*) slf;
    cmd->Out()->SetF(self->GetMaxTexelSize());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setterrainscale
    @input
    f(TerrainScale)
    @output
    v
    @info
    Set terrain scale factor.
*/
static void
n_setterrainscale(void* slf, nCmd* cmd)
{
    nTerrainNode* self = (nTerrainNode*) slf;
    self->SetTerrainScale(cmd->In()->GetF());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getterrainscale
    @input
    f(TerrainScale)
    @output
    v
    @info
    Get the terrain scale factor.
*/
static void
n_getterrainscale(void* slf, nCmd* cmd)
{
    nTerrainNode* self = (nTerrainNode*) slf;
    cmd->Out()->SetF(self->GetTerrainScale());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setterrainorigin
    @input
    f(OrigX), f(OrigY), f(OrigZ)
    @output
    v
    @info
    Set terrain origin (post scale).
*/
static void
n_setterrainorigin(void* slf, nCmd* cmd)
{
    nTerrainNode* self = (nTerrainNode*) slf;
    static vector3 v;
    v.x = cmd->In()->GetF();
    v.y = cmd->In()->GetF();
    v.z = cmd->In()->GetF();
    self->SetTerrainOrigin(v);
}

//------------------------------------------------------------------------------
/**
    @cmd
    getterrainscale
    @input
    @output
    f(OrigX), f(OrigY), f(OrigZ)
    @info
    Get terrain origin (post scale).
*/
static void
n_getterrainorigin(void* slf, nCmd* cmd)
{
    nTerrainNode* self = (nTerrainNode*) slf;
    const vector3& v = self->GetTerrainOrigin();
    cmd->Out()->SetF(v.x);
    cmd->Out()->SetF(v.y);
    cmd->Out()->SetF(v.z);
}

//------------------------------------------------------------------------------
/**
*/
bool
nTerrainNode::SaveCmds(nPersistServer* ps)
{
    if (nMaterialNode::SaveCmds(ps))
    {
        nCmd* cmd;

        //--- setchunkfile ---
        cmd = ps->GetCmd(this, 'SCHF');
        cmd->In()->SetS(this->GetChunkFile());
        ps->PutCmd(cmd);

        //--- settexquadfile ---
        cmd = ps->GetCmd(this, 'STQF');
        cmd->In()->SetS(this->GetTexQuadFile());
        ps->PutCmd(cmd);

        //--- setmaxpixelerror ---
        cmd = ps->GetCmd(this, 'SMPE');
        cmd->In()->SetF(this->GetMaxPixelError());
        ps->PutCmd(cmd);

        //--- setmaxtexelsize ---
        cmd = ps->GetCmd(this, 'SMTS');
        cmd->In()->SetF(this->GetMaxTexelSize());
        ps->PutCmd(cmd);

        //--- setterrainscale ---
        cmd = ps->GetCmd(this, 'STRS');
        cmd->In()->SetF(this->GetTerrainScale());
        ps->PutCmd(cmd);

        //--- setterrainorigin ---
        cmd = ps->GetCmd(this, 'STRO');
        const vector3& v = this->GetTerrainOrigin();
        cmd->In()->SetF(v.x);
        cmd->In()->SetF(v.y);
        cmd->In()->SetF(v.z);
        ps->PutCmd(cmd);

        return true;
    }
    return false;
}
