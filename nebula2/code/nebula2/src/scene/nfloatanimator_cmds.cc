//------------------------------------------------------------------------------
//  nfloatanimator_cmds.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "scene/nfloatanimator.h"
#include "kernel/npersistserver.h"

static void n_setvectorname(void* slf, nCmd* cmd);
static void n_getvectorname(void* slf, nCmd* cmd);
static void n_addkey(void* slf, nCmd* cmd);
static void n_getnumkeys(void* slf, nCmd* cmd);
static void n_getkeyat(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nfloatanimator    

    @cppclass
    nFloatAnimator

    @superclass
    nanimator

    @classinfo
    Animate a float vector attribute of a nabstractshadernode.
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setvectorname_s", 'SVCN', n_setvectorname);
    cl->AddCmd("s_getvectorname_v", 'GVCN', n_getvectorname);
    cl->AddCmd("v_addkey_ff",    'ADDK', n_addkey);
    cl->AddCmd("i_getnumkeys_v",    'GNKS', n_getnumkeys);
    cl->AddCmd("ff_getkeyat_i",  'GKAT', n_getkeyat);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    setvectorname
    @input
    s(VectorName)
    @output
    v
    @info
    Set name of float vector variable to animate in target object.
*/
void
n_setvectorname(void* slf, nCmd* cmd)
{
    nFloatAnimator* self = (nFloatAnimator*) slf;
    self->SetVectorName(cmd->In()->GetS());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getvectorname
    @input
    v
    @output
    s(VectorName)
    @info
    Get name of float vector variable to animate in target object.
*/
void
n_getvectorname(void* slf, nCmd* cmd)
{
    nFloatAnimator* self = (nFloatAnimator*) slf;
    cmd->Out()->SetS(self->GetVectorName());
}

//------------------------------------------------------------------------------
/**
    @cmd
    addkey
    @input
    f(Time), f(Value)
    @output
    v
    @info
    Add a float vector key to the animation key array.
*/
void
n_addkey(void* slf, nCmd* cmd)
{
    nFloatAnimator* self = (nFloatAnimator*) slf;

    float time = cmd->In()->GetF();
    float value = cmd->In()->GetF();

    self->AddKey(time, value);
}

//------------------------------------------------------------------------------
/**
    @cmd
    getnumkeys
    @input
    v
    @output
    i(NumKeys)
    @info
    Returns number of key in animation array.
*/
void
n_getnumkeys(void* slf, nCmd* cmd)
{
    nFloatAnimator* self = (nFloatAnimator*) slf;
    cmd->Out()->SetI(self->GetNumKeys());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getkeyat
    @input
    i(KeyIndex)
    @output
    f(Time), f(Value)
    @info
    Returns key at given index.
*/
void
n_getkeyat(void* slf, nCmd* cmd)
{
    nFloatAnimator* self = (nFloatAnimator*) slf;
    float time;
    float value;
    self->GetKeyAt(cmd->In()->GetI(), time, value);
    cmd->Out()->SetF(time);
    cmd->Out()->SetF(value);
}

//------------------------------------------------------------------------------
/**
*/
bool
nFloatAnimator::SaveCmds(nPersistServer* ps)
{
    if (nAnimator::SaveCmds(ps))
    {
        nCmd* cmd;

        //--- setvectorname ---
        if (this->GetVectorName())
        {
            cmd = ps->GetCmd(this, 'SVCN');
            cmd->In()->SetS(this->GetVectorName());
            ps->PutCmd(cmd);
        }

        //--- addkey ---
        int i;
        int numKeys = this->GetNumKeys();
        for (i = 0; i < numKeys; i++)
        {
            float time;
            float val;
            cmd = ps->GetCmd(this, 'ADDK');
            this->GetKeyAt(i, time, val);
            cmd->In()->SetF(time);
            cmd->In()->SetF(val);
            ps->PutCmd(cmd);
        }

        return true;
    }
    return false;
}
