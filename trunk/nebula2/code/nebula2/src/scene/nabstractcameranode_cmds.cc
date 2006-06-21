//------------------------------------------------------------------------------
//  ncameranode_cmds.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "scene/nabstractcameranode.h"
#include "renderpath/nrprendertarget.h"
#include "kernel/npersistserver.h"
#include "gfx2/ngfxserver2.h"

static void n_setrenderpathsection(void* slf, nCmd* cmd);
static void n_getrenderpathsection(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    ncameranode
    
    @superclass
    ntransformnode

    @classinfo
    Renders a camera view into a render target.
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setrenderpathsection_s", 'SRPS', n_setrenderpathsection);
    cl->AddCmd("s_getrenderpathsection_v", 'GRPS', n_getrenderpathsection);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    setrenderpathsection
    @input
    s(RenderPathSection)
    @output
    v
    @info
    Set the render path section which this camera should use for rendering.
*/
static void
n_setrenderpathsection(void* slf, nCmd* cmd)
{
    nAbstractCameraNode* self = (nAbstractCameraNode*) slf;
    self->SetRenderPathSection(cmd->In()->GetS());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getrenderpathsection
    @input
    v
    @output
    s(RenderPathSection)
    @info
    Get the render path section.
*/
static void
n_getrenderpathsection(void* slf, nCmd* cmd)
{
    nAbstractCameraNode* self = (nAbstractCameraNode*) slf;
    cmd->Out()->SetS(self->GetRenderPathSection().Get());
}

//------------------------------------------------------------------------------
/**
*/
bool 
nAbstractCameraNode::SaveCmds(nPersistServer* ps)
{
    if (nTransformNode::SaveCmds(ps))
    {
        nCmd* cmd;

        //--- setrenderpathsection ---
        cmd = ps->GetCmd(this, 'SRPS');
        cmd->In()->SetS(this->GetRenderPathSection().Get());
        ps->PutCmd(cmd);

        return true;
    }
    return false;
}
