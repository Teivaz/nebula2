//------------------------------------------------------------------------------
//  ntransformanimator_cmds.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "scene/ntransformanimator.h"
#include "kernel/npersistserver.h"

static void n_addposkey(void* slf, nCmd* cmd);
static void n_addeulerkey(void* slf, nCmd* cmd);
static void n_addscalekey(void* slf, nCmd* cmd);
static void n_getnumposkeys(void* slf, nCmd* cmd);
static void n_getnumeulerkeys(void* slf, nCmd* cmd);
static void n_getnumscalekeys(void* slf, nCmd* cmd);
static void n_getposkeyat(void* slf, nCmd* cmd);
static void n_geteulerkeyat(void* slf, nCmd* cmd);
static void n_getscalekeyat(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    ntransformanimator
    @superclass
    nanimator
    @classinfo
    Animate attributes of a transform node.
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_addposkey_ffff",     'ADPK', n_addposkey);
    cl->AddCmd("v_addeulerkey_ffff",   'ADEK', n_addeulerkey);
    cl->AddCmd("v_addscalekey_ffff",   'ADSK', n_addscalekey);
    cl->AddCmd("i_getnumposkeys_v",    'GNPK', n_getnumposkeys);
    cl->AddCmd("i_getnumeulerkeys_v",  'GNEK', n_getnumeulerkeys);
    cl->AddCmd("i_getnumscalekeys_v",  'GNSK', n_getnumscalekeys);
    cl->AddCmd("ffff_getposkeyat_i",   'GPKA', n_getposkeyat);
    cl->AddCmd("ffff_geteulerkeyat_i", 'GEKA', n_geteulerkeyat);
    cl->AddCmd("ffff_getscalekeyat_i", 'GSKA', n_getscalekeyat);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    addposkey
    @input
    f(Time), f(PosX), f(PosY), f(PosZ)
    @output
    v
    @info
    Add a position key to the position key array.
*/
static void
n_addposkey(void* slf, nCmd* cmd)
{
    nTransformAnimator* self = (nTransformAnimator*) slf;
    vector3 v0;
    float f0 = cmd->In()->GetF();
    v0.x = cmd->In()->GetF();
    v0.y = cmd->In()->GetF();
    v0.z = cmd->In()->GetF();
    self->AddPosKey(f0, v0);
}

//------------------------------------------------------------------------------
/**
    @cmd
    addeulerkey
    @input
    f(Time), f(EulerX), f(EulerY), f(EulerZ)
    @output
    v
    @info
    Add a euler angle key to the position key array.
*/
static void
n_addeulerkey(void* slf, nCmd* cmd)
{
    nTransformAnimator* self = (nTransformAnimator*) slf;
    vector3 v0;
    float f0 = cmd->In()->GetF();
    v0.x = n_deg2rad(cmd->In()->GetF());
    v0.y = n_deg2rad(cmd->In()->GetF());
    v0.z = n_deg2rad(cmd->In()->GetF());
    self->AddEulerKey(f0, v0);
}

//------------------------------------------------------------------------------
/**
    @cmd
    addscalekey
    @input
    f(Time), f(ScaleX), f(ScaleY), f(ScaleZ)
    @output
    v
    @info
    Add a scale key to the position key array.
*/
static void
n_addscalekey(void* slf, nCmd* cmd)
{
    nTransformAnimator* self = (nTransformAnimator*) slf;
    vector3 v0;
    float f0 = cmd->In()->GetF();
    v0.x = cmd->In()->GetF();
    v0.y = cmd->In()->GetF();
    v0.z = cmd->In()->GetF();
    self->AddScaleKey(f0, v0);
}

//------------------------------------------------------------------------------
/**
    @cmd
    getnumposkeys
    @input
    v
    @output
    i(NumPosKeys)
    @info
    Return number of position keys.
*/
static void
n_getnumposkeys(void* slf, nCmd* cmd)
{
    nTransformAnimator* self = (nTransformAnimator*) slf;
    cmd->Out()->SetI(self->GetNumPosKeys());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getnumeulerkeys
    @input
    v
    @output
    i(NumEulerKeys)
    @info
    Return number of euler keys.
*/
static void
n_getnumeulerkeys(void* slf, nCmd* cmd)
{
    nTransformAnimator* self = (nTransformAnimator*) slf;
    cmd->Out()->SetI(self->GetNumEulerKeys());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getnumscalekeys
    @input
    v
    @output
    i(NumScaleKeys)
    @info
    Return number of scale keys.
*/
static void
n_getnumscalekeys(void* slf, nCmd* cmd)
{
    nTransformAnimator* self = (nTransformAnimator*) slf;
    cmd->Out()->SetI(self->GetNumScaleKeys());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getposkeyat
    @input
    i(Index)
    @output
    f(Time), f(PosX), f(PosY), f(PosZ)
    @info
    Get position key attributes at given index.
*/
static void
n_getposkeyat(void* slf, nCmd* cmd)
{
    nTransformAnimator* self = (nTransformAnimator*) slf;
    float f0;
    vector3 v0;
    self->GetPosKeyAt(cmd->In()->GetI(), f0, v0);
    cmd->Out()->SetF(f0);
    cmd->Out()->SetF(v0.x);
    cmd->Out()->SetF(v0.y);
    cmd->Out()->SetF(v0.z);
}

//------------------------------------------------------------------------------
/**
    @cmd
    geteulerkeyat
    @input
    i(Index)
    @output
    f(Time), f(EulerX), f(EulerY), f(EulerZ)
    @info
    Get euler key attributes at given index.
*/
static void
n_geteulerkeyat(void* slf, nCmd* cmd)
{
    nTransformAnimator* self = (nTransformAnimator*) slf;
    float f0;
    vector3 v0;
    self->GetEulerKeyAt(cmd->In()->GetI(), f0, v0);
    cmd->Out()->SetF(f0);
    cmd->Out()->SetF(n_rad2deg(v0.x));
    cmd->Out()->SetF(n_rad2deg(v0.y));
    cmd->Out()->SetF(n_rad2deg(v0.z));
}

//------------------------------------------------------------------------------
/**
    @cmd
    getscalekeyat
    @input
    i(Index)
    @output
    f(Time), f(ScaleX), f(ScaleY), f(ScaleZ)
    @info
    Get scale key attributes at given index.
*/
static void
n_getscalekeyat(void* slf, nCmd* cmd)
{
    nTransformAnimator* self = (nTransformAnimator*) slf;
    float f0;
    vector3 v0;
    self->GetScaleKeyAt(cmd->In()->GetI(), f0, v0);
    cmd->Out()->SetF(f0);
    cmd->Out()->SetF(v0.x);
    cmd->Out()->SetF(v0.y);
    cmd->Out()->SetF(v0.z);
}

//------------------------------------------------------------------------------
/**
*/
bool
nTransformAnimator::SaveCmds(nPersistServer* ps)
{
    if (nAnimator::SaveCmds(ps))
    {
        nCmd* cmd;
        int curKey;
        int numKeys;

        //--- addposkey ---
        numKeys = this->GetNumPosKeys();
        for (curKey = 0; curKey < numKeys; curKey++)
        {
            float time;
            vector3 val;
            cmd = ps->GetCmd(this, 'ADPK');
            this->GetPosKeyAt(curKey, time, val);
            cmd->In()->SetF(time);
            cmd->In()->SetF(val.x);
            cmd->In()->SetF(val.y);
            cmd->In()->SetF(val.z);
            ps->PutCmd(cmd);
        }

        //--- addeulerkey ---
        numKeys = this->GetNumPosKeys();
        for (curKey = 0; curKey < numKeys; curKey++)
        {
            float time;
            vector3 val;
            cmd = ps->GetCmd(this, 'ADEK');
            this->GetEulerKeyAt(curKey, time, val);
            cmd->In()->SetF(time);
            cmd->In()->SetF(n_rad2deg(val.x));
            cmd->In()->SetF(n_rad2deg(val.y));
            cmd->In()->SetF(n_rad2deg(val.z));
            ps->PutCmd(cmd);
        }

        //--- addscalekey ---
        numKeys = this->GetNumPosKeys();
        for (curKey = 0; curKey < numKeys; curKey++)
        {
            float time;
            vector3 val;
            cmd = ps->GetCmd(this, 'ADSK');
            this->GetScaleKeyAt(curKey, time, val);
            cmd->In()->SetF(time);
            cmd->In()->SetF(val.x);
            cmd->In()->SetF(val.y);
            cmd->In()->SetF(val.z);
            ps->PutCmd(cmd);
        }

        return true;
    }
    return false;
}
