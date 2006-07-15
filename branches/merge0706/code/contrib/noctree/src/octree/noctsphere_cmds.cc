//-------------------------------------------------------------------
//  (c)2004 Rafael Van Daele-Hunt
//-------------------------------------------------------------------
#include "octree/noctsphere.h"

static void n_setsphere(void *, nCmd *);

//------------------------------------------------------------------------------
/**
    @scriptclass
    noctsphere

    @superclass
    noctree

    @classinfo
    The nOctSphere class is used in conjunction with nOctree
    to cull all objects outside the given sphere.
*/
void n_initcmds(nClass *cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setsphere_ffff",'SSDN',n_setsphere);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    setsphere

    @input
    fff (x,y,z position in worldspace), f (radius)

    @output
    v

    @info
    Defines the clipping sphere.
*/
static void n_setsphere(void *o, nCmd *cmd)
{
    nOctSphere *self = (nOctSphere *) o;
    float xPos = cmd->In()->GetF();
    float yPos = cmd->In()->GetF();
    float zPos = cmd->In()->GetF();
    float r = cmd->In()->GetF();
    self->SetSphere( sphere( xPos, yPos, zPos, r ) );
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
