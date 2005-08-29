//------------------------------------------------------------------------------
//  nmaterialnode_cmds.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "scene/nmaterialnode.h"
#include "kernel/npersistserver.h"

static void n_setshader(void* slf, nCmd* cmd);
static void n_getshader(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nmaterialnode

    @cppclass
    nMaterialNode

    @superclass
    nabstractshadernode

    @classinfo
    A material node defines a shader and associated shader variables. It is
    derived from ntransformnode, so it also has a position in space. Note
    that a nmaterialnode itself cannot render, for this one of its subclasses
    have to be used.
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setshader_s", 'SSHD', n_setshader);
    cl->AddCmd("s_getshader_s", 'GSHD', n_getshader);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    setshader
    @input
    s(ShaderResource)
    @output
    v
    @info
    Set the name of the shader resource.
*/
static void
n_setshader(void* slf, nCmd* cmd)
{
    nMaterialNode* self = (nMaterialNode*) slf;
    self->SetShader(cmd->In()->GetS());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getshader
    @input
    v
    @output
    s(ShaderResource)
    @info
    Get the name of the shader resource.
*/
static void
n_getshader(void* slf, nCmd* cmd)
{
    nMaterialNode* self = (nMaterialNode*) slf;
    cmd->Out()->SetS(self->GetShader());
}

//------------------------------------------------------------------------------
/**
*/
bool
nMaterialNode::SaveCmds(nPersistServer* ps)
{
    if (nAbstractShaderNode::SaveCmds(ps))
    {
        nCmd* cmd;

        //--- setshader ---
        if (!this->shaderName.IsEmpty())
        {
            cmd = ps->GetCmd(this, 'SSHD');
            cmd->In()->SetS(this->shaderName.Get());
            ps->PutCmd(cmd);
        }
        return true;
    }
    return false;
}
