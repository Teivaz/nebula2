//-------------------------------------------------------------------
//  (c)2004 Rafael Van Daele-Hunt
//-------------------------------------------------------------------
#include "octree/noctbbox.h"

static void n_setbbox(void *, nCmd *);

//------------------------------------------------------------------------------
/**
    @scriptclass
    noctbbox

    @superclass
    noctree

    @classinfo
    The nOctBBox class is used in conjunction with nOctree
    to cull all objects outside the given box.
*/
void n_initcmds(nClass *cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setbbox_ffffff",'SSDN',n_setbbox);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    setbbox

    @input
    fff (centre), fff (extents)

    @output
    v

    @info
    Defines the clipping box.
*/
static void n_setbbox(void *o, nCmd *cmd)
{
    nOctBBox *self = (nOctBBox *) o;
    float xPos = cmd->In()->GetF();
    float yPos = cmd->In()->GetF();
    float zPos = cmd->In()->GetF();
    float xRad = cmd->In()->GetF();
    float yRad = cmd->In()->GetF();
    float zRad = cmd->In()->GetF();
    self->SetBBox( bbox3( vector3( xPos, yPos, zPos ), vector3( xRad, yRad, zRad ) ) );
}


//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
