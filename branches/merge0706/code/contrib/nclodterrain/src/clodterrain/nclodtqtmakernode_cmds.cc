#define N_IMPLEMENTS nCLODTQTMakerNode
//-------------------------------------------------------------------
// nCLODTQTMAkerNode commands
//-------------------------------------------------------------------
#include "clodterrain/nclodtqtmakernode.h"

static void n_settqtfilename(void *, nCmd *);
static void n_settqtparameters(void *, nCmd *);
static void n_compiletqtfromfile(void *, nCmd *);
static void n_setvalidateflag(void *, nCmd *);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nCLODTQTMakerNode

    @superclass
    nRoot

    @classinfo
    Support class for nCLODvertextree and nCLODRenderNode.  This node will
    take a heightfield as input and generate the Chunked LOD vertex data.
*/
void n_initcmds(nClass *cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_settqtfilename_s",            'STFN', n_settqtfilename);
    cl->AddCmd("v_settqtparameters_ii",         'STQP', n_settqtparameters);
    cl->AddCmd("v_compiletqtfromfile_s",            'CTFF', n_compiletqtfromfile);
    cl->AddCmd("v_setvalidateflag_b",           'SVFL', n_setvalidateflag);
    cl->EndCmds();
}

/**
    @cmd
    settqtfilename

    @input
    s

    @output
    v

    @info
    Specifies the name of the output file which will hold the tqt data.
*/
static void n_settqtfilename(void *o, nCmd *cmd)
{
    nCLODTQTMakerNode *self = (nCLODTQTMakerNode *) o;
    self->setTQTFilename(cmd->In()->GetS());
}

/**
    @cmd
    settqtparameters

    @input
    ii

    @output
    v

    @info
    Specify the tree depth and tile size
*/
static void n_settqtparameters(void *o, nCmd *cmd)
{
    nCLODTQTMakerNode *self = (nCLODTQTMakerNode *) o;
    int arg1 = cmd->In()->GetI();
    int arg2 = cmd->In()->GetI();
    self->setTQTParameters(arg1,arg2);
}
/**
    @cmd
    settqtparameters

    @input
    ii

    @output
    v

    @info
    Specify the tree depth and tile size
*/
static void n_setchunkparameters(void *o, nCmd *cmd)
{
    nCLODTQTMakerNode *self = (nCLODTQTMakerNode *) o;
    int depth = cmd->In()->GetI();
    int maxerror = cmd->In()->GetI();
    self->setTQTParameters(depth,maxerror);
}

/**
    @cmd
    compiletqtfromfile

    @input
    s

    @output
    v

    @info
    Compiles the tqt data and writes to the previously specified filename.
*/
static void n_compiletqtfromfile(void *o, nCmd *cmd)
{
    nCLODTQTMakerNode *self = (nCLODTQTMakerNode *) o;
    const char * arg1 = cmd->In()->GetS();
    self->compileTQTFromFile(arg1);
}



bool nCLODTQTMakerNode::SaveCmds(nPersistServer *ps)
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
    nCLODTQTMakerNode *self = (nCLODTQTMakerNode *) o;
    bool arg1 = cmd->In()->GetB();
    self->setValidateFlag(arg1);
}


//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
