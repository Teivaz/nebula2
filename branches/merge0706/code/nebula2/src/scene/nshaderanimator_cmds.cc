//------------------------------------------------------------------------------
//  nshaderanimator_cmds.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "scene/nshaderanimator.h"
#include "kernel/npersistserver.h"

static void n_setparamname(void* slf, nCmd* cmd);
static void n_getparamname(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nshaderanimator
    @cppclass
    nShaderAnimator
    @superclass
    nanimator
    @classinfo
    Base class for shader parameter animators.
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setparamname_s", 'SPNM', n_setparamname);
    cl->AddCmd("s_getparamname_v", 'GPNM', n_getparamname);
    cl->AddCmd("v_setvectorname_s", 'SVCN', n_setparamname);    // not a bug!
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    setparamname
    @input
    s(ParameterName)
    @output
    v
    @info
    Set name of shader parameter to animate in target object.
*/
static void
n_setparamname(void* slf, nCmd* cmd)
{
    nShaderAnimator* self = (nShaderAnimator*) slf;
    self->SetParamName(cmd->In()->GetS());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getparamname
    @input
    v
    @output
    s(ParameterName)
    @info
    Get name of shader parameter to animate in target object.
*/
static void
n_getparamname(void* slf, nCmd* cmd)
{
    nShaderAnimator* self = (nShaderAnimator*) slf;
    cmd->Out()->SetS(self->GetParamName());
}

//------------------------------------------------------------------------------
/**
*/
bool
nShaderAnimator::SaveCmds(nPersistServer* ps)
{
    if (nAnimator::SaveCmds(ps))
    {
        nCmd* cmd;

        //--- setparamname ---
        if (this->GetParamName())
        {
            cmd = ps->GetCmd(this, 'SPNM');
            cmd->In()->SetS(this->GetParamName());
            ps->PutCmd(cmd);
        }
        return true;
    }
    return false;
}



