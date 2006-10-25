//------------------------------------------------------------------------------
//  ntextureanimator_cmds.cc
//  (C) 2004 Rafael Van Daele-Hunt
//------------------------------------------------------------------------------
#include "scene/ntextureanimator.h"
#include "kernel/npersistserver.h"
#include "gfx2/ntexture2.h"

static void n_addtexture(void* slf, nCmd* cmd);
static void n_setshaderparam(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    ntextureanimator

    @cppclass
    nTextureAnimator

    @superclass
    nanimator

    @classinfo
    Switch between different textures based on an nRenderContext variable.
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_addtexture_s",     'ATEX', n_addtexture);
    cl->AddCmd("v_setshaderparam_s", 'SHAD', n_setshaderparam);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    addtexture
    @input
    s(Texture path)
    @output
    v
    @info
    Add a texture to the array (with index number equal to the number of textures added thus far).
*/
static void
n_addtexture(void* slf, nCmd* cmd)
{
    nTextureAnimator* self = (nTextureAnimator*) slf;
    self->AddTexture(cmd->In()->GetS());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setshaderparam
    @input
    s(Texture path)
    @output
    v
    @info
    Sets the shader state parameter that will be passed to nAbstractShaderNode::SetTexture.
*/
static void
n_setshaderparam(void* slf, nCmd* cmd)
{
    nTextureAnimator* self = (nTextureAnimator*) slf;
    self->SetShaderParam(cmd->In()->GetS());
}

//------------------------------------------------------------------------------
/**
*/
bool
nTextureAnimator::SaveCmds(nPersistServer* ps)
{
    if (nAnimator::SaveCmds(ps))
    {
        nCmd* cmd;

        //--- addtexture ---
        const int numKeys = this->GetNumTextures();
        for (int curKey = 0; curKey < numKeys; ++curKey)
        {
            cmd = ps->GetCmd(this, 'ATEX');
            cmd->In()->SetS(this->GetTextureAt(curKey)->GetName());
            ps->PutCmd(cmd);
        }
        //--- setshaderparam ---
        cmd = ps->GetCmd(this, 'SHAD');
        cmd->In()->SetS(this->GetShaderParam());
        ps->PutCmd(cmd);

        return true;
    }
    return false;
}
