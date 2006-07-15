//-------------------------------------------------------------------
// nCollChunkerNode commands
//-------------------------------------------------------------------
#include "clodterrain/nclodchunkernode.h"
#include "kernel/npersistserver.h"

static void n_setchunkfilename(void *, nCmd *);
static void n_setspacings(void *, nCmd *);
static void n_setchunkparameters(void *, nCmd *);
static void n_compilechunksfromfile(void *, nCmd *);
static void n_setvalidateflag(void *, nCmd *);
static void n_settileindexfilename(void *, nCmd *);
static void n_settilespersplat(void *, nCmd *);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nCLODChunkerNode

    @superclass
    nRoot

    @classinfo
    Support class for nCLODvertextree and nCLODRenderNode.  This node will
    take a heightfield as input and generate the Chunked LOD vertex data.
*/
void n_initcmds(nClass *cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setchunkfilename_s",          'SCOS', n_setchunkfilename);
    cl->AddCmd("v_settileindexfilename_s",          'STIF', n_settileindexfilename);
    cl->AddCmd("v_setspacings_fff",             'SSPA', n_setspacings);
    cl->AddCmd("v_settilespersplat_i",              'STPS', n_settilespersplat);
    cl->AddCmd("v_setchunkparameters_if",       'SCHP', n_setchunkparameters);
    cl->AddCmd("v_compilechunksfromfile_s",         'CTRE', n_compilechunksfromfile);
    cl->AddCmd("v_setvalidateflag_b",           'SVFL', n_setvalidateflag);
    cl->EndCmds();
}

/**
    @cmd
    setchunkfilename

    @input
    s

    @output
    v

    @info
    Specifies the name of the output file which will hold the chunked data.
*/
static void n_setchunkfilename(void *o, nCmd *cmd)
{
    nCLODChunkerNode *self = (nCLODChunkerNode *) o;
    self->setChunkFilename(cmd->In()->GetS());
}

/**
    @cmd
    settileindexfilename

    @input
    s

    @output
    v

    @info
    Specifies the name of the input file which holds tile index information.
*/
static void n_settileindexfilename(void *o, nCmd *cmd)
{
    nCLODChunkerNode *self = (nCLODChunkerNode *) o;
    self->setTileIndexFilename(cmd->In()->GetS());
}

/**
    @cmd
    settilespersplat

    @input
    i

    @output
    v

    @info
    Specifies the number of tiles to put into each splat.
*/
static void n_settilespersplat(void *o, nCmd *cmd)
{
    nCLODChunkerNode *self = (nCLODChunkerNode *) o;
    self->setTilesPerSplat(cmd->In()->GetI());
}



/**
    @cmd
    setspacings

    @input
    fff

    @output
    v

    @info
    Specify the x,y, and z scale factors.  X and Y are the horizontal scales and Z is
    the vertical scale.
*/
static void n_setspacings(void *o, nCmd *cmd)
{
    nCLODChunkerNode *self = (nCLODChunkerNode *) o;
    float arg1 = cmd->In()->GetF();
    float arg2 = cmd->In()->GetF();
    float arg3 = cmd->In()->GetF();
    self->setSpacings(arg1,arg2,arg3);
}
/**
    @cmd
    setchunkparameters

    @input
    if

    @output
    v

    @info
    Specify the tree depth and maximum error
*/
static void n_setchunkparameters(void *o, nCmd *cmd)
{
    nCLODChunkerNode *self = (nCLODChunkerNode *) o;
    int depth = cmd->In()->GetI();
    float maxerror = cmd->In()->GetF();
    self->setChunkParameters(depth,maxerror);
}

/**
    @cmd
    compilechunksfromfile

    @input
    s

    @output
    v

    @info
    Compiles the chunked data and writes to the previously specified filename.
*/
static void n_compilechunksfromfile(void *o, nCmd *cmd)
{
    nCLODChunkerNode *self = (nCLODChunkerNode *) o;
    const char * arg1 = cmd->In()->GetS();
    self->compileChunksFromFile(arg1);
}



bool nCLODChunkerNode::SaveCmds(nPersistServer *ps)
{
    return false;
}

/**
    @cmd
    setvalidateflag

    @input
    b

    @output
    v

    @info
    If \b true tells the compiler node to attempt loading in the compiled collision chunks to
    verify that the original chunks and those read from the file match.  Useful for checking new
    types of collision trees, or especially hairy heightfields with unusual amounts of
    vertices or faces.

    The default is for this flag to be \b true.
*/
static void n_setvalidateflag(void *o, nCmd *cmd)
{
    nCLODChunkerNode *self = (nCLODChunkerNode *) o;
    bool arg1 = cmd->In()->GetB();
    self->setValidateFlag(arg1);
}


//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
