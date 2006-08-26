//------------------------------------------------------------------------------
//  nlightnode_cmds.cc
//  (C) 2004 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "scene/nlightnode.h"
#include "kernel/npersistserver.h"

static void n_settype(void* slf, nCmd* cmd);
static void n_gettype(void* slf, nCmd* cmd);
static void n_setcastshadows(void* slf, nCmd* cmd);
static void n_getcastshadows(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nlightnode

    @cppclass
    nLightNode

    @superclass
    ntransformnode

    @classinfo
    Scene node which provides lighting information.
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_settype_s",           'STYP', n_settype);
    cl->AddCmd("s_gettype_v",           'GTYP', n_gettype);
    cl->AddCmd("v_setcastshadows_b",           'STCS', n_setcastshadows);
    cl->AddCmd("b_getcastshadows_v",           'GTCS', n_getcastshadows);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    settype
    @input
    s(Type="Point","Directional","Spot")
    @output
    v
    @info
    Set the light's type.
*/
static void
n_settype(void* slf, nCmd* cmd)
{
    nLightNode* self = (nLightNode*) slf;
    self->SetType(nLight::StringToType(cmd->In()->GetS()));
}

//------------------------------------------------------------------------------
/**
    @cmd
    gettype
    @input
    v
    @output
    s(Type="Point","Directional","Spot")
    @info
    Get the light's type.
*/
static void
n_gettype(void* slf, nCmd* cmd)
{
    nLightNode* self = (nLightNode*) slf;
    cmd->Out()->SetS(nLight::TypeToString(self->GetType()));
}

//------------------------------------------------------------------------------
/**
    @cmd
    setcastshadows
    @input
    b
    @output
    v
    @info
    Set the light to cast shadows or not.
*/
static void
n_setcastshadows(void* slf, nCmd* cmd)
{
    nLightNode* self = (nLightNode*) slf;
    self->SetCastShadows(cmd->In()->GetB());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getcastshadows
    @input
    v
    @output
    b
    @info
    Check if light cast shadows.
*/
static void
n_getcastshadows(void* slf, nCmd* cmd)
{
    nLightNode* self = (nLightNode*) slf;
    cmd->Out()->SetB(self->GetCastShadows());
}

//------------------------------------------------------------------------------
/**
*/
bool
nLightNode::SaveCmds(nPersistServer* ps)
{
    if (nAbstractShaderNode::SaveCmds(ps))
    {
        nCmd* cmd;
        vector4 c;

        //--- settype ---
        cmd = ps->GetCmd(this, 'STYP');
        cmd->In()->SetS(nLight::TypeToString(this->GetType()));
        ps->PutCmd(cmd);

        //--- setcastshadows ---
        cmd = ps->GetCmd(this, 'STCS');
        cmd->In()->SetB(this->GetCastShadows());
        ps->PutCmd(cmd);

        return true;
    }
    return false;
}
