//------------------------------------------------------------------------------
//  nspotlightnode_cmds.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "scene/nspotlightnode.h"
#include "kernel/npersistserver.h"

static void n_setperspective(void* slf, nCmd* cmd);
static void n_setorthogonal(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nspotlightnode
    
    @superclass
    nlightnode
    
    @classinfo
    Implements a spot light node (i.e. to implement spot light sources).
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setperspective_ffff", 'SPSP', n_setperspective);
    cl->AddCmd("v_setorthogonal_ffff",  'SORL', n_setorthogonal);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    setperspective
    
    @input
    f(AngleOfView), f(AspectRatio), f(NearPlane), f(farPlane)
    
    @output
    v
    
    @info
    Define the perspective view volume. The settings are similar to camera settings.
*/
static void
n_setperspective(void* slf, nCmd* cmd)
{
    nSpotLightNode* self = (nSpotLightNode*) slf;
    float aov       = cmd->In()->GetF();
    float aspect    = cmd->In()->GetF();
    float nearPlane = cmd->In()->GetF();
    float farPlane  = cmd->In()->GetF();
    self->SetPerspective(aov, aspect, nearPlane, farPlane);
}

//------------------------------------------------------------------------------
/**
    @cmd
    setorthogonal
    
    @input
    f(Width), f(Height), f(NearPlane), f(FarPlane)
    
    @output
    v
    
    @info
    Define the orthogonal view volume. The settings are similar to camera settings.
*/
static void
n_setorthogonal(void* slf, nCmd* cmd)
{
    nSpotLightNode* self = (nSpotLightNode*) slf;
    float w = cmd->In()->GetF();
    float h = cmd->In()->GetF();
    float n = cmd->In()->GetF();
    float f = cmd->In()->GetF();
    self->SetOrthogonal(w, h, n, f);
}

//------------------------------------------------------------------------------
/**
*/
bool
nSpotLightNode::SaveCmds(nPersistServer* ps)
{
    if (nAbstractShaderNode::SaveCmds(ps))
    {
        nCmd* cmd;

        if (this->camera.GetType() == nCamera2::Perspective)
        {
            //--- setperspective ---
            cmd = ps->GetCmd(this, 'SPSP');
            cmd->In()->SetF(this->camera.GetAngleOfView());
            cmd->In()->SetF(this->camera.GetAspectRatio());
            cmd->In()->SetF(this->camera.GetNearPlane());
            cmd->In()->SetF(this->camera.GetFarPlane());
            ps->PutCmd(cmd);
        }
        else
        {
            //--- setorthogonal ---
            cmd = ps->GetCmd(this, 'SORL');
            cmd->In()->SetF(this->camera.GetWidth());
            cmd->In()->SetF(this->camera.GetHeight());
            cmd->In()->SetF(this->camera.GetNearPlane());
            cmd->In()->SetF(this->camera.GetFarPlane());
            ps->PutCmd(cmd);
        }
        return true;
    }
    return false;
}

