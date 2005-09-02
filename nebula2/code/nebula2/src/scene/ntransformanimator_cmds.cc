//------------------------------------------------------------------------------
//  ntransformanimator_cmds.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "scene/ntransformanimator.h"
#include "kernel/npersistserver.h"

static void n_addposkey(void* slf, nCmd* cmd);
static void n_addeulerkey(void* slf, nCmd* cmd);
static void n_addscalekey(void* slf, nCmd* cmd);
static void n_addquatkey(void* slf, nCmd* cmd);
static void n_getnumposkeys(void* slf, nCmd* cmd);
static void n_getnumeulerkeys(void* slf, nCmd* cmd);
static void n_getnumscalekeys(void* slf, nCmd* cmd);
static void n_getnumquatkeys(void* slf, nCmd* cmd);
static void n_getposkeyat(void* slf, nCmd* cmd);
static void n_geteulerkeyat(void* slf, nCmd* cmd);
static void n_getscalekeyat(void* slf, nCmd* cmd);
static void n_getquatkeyat(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    ntransformanimator

    @cppclass
    nTransformAnimator

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
    cl->AddCmd("v_addquatkey_fffff",   'ADQK', n_addquatkey);
    cl->AddCmd("i_getnumposkeys_v",    'GNPK', n_getnumposkeys);
    cl->AddCmd("i_getnumeulerkeys_v",  'GNEK', n_getnumeulerkeys);
    cl->AddCmd("i_getnumscalekeys_v",  'GNSK', n_getnumscalekeys);
    cl->AddCmd("i_getnumquatkeys_v",   'GNQK', n_getnumquatkeys);
    cl->AddCmd("ffff_getposkeyat_i",   'GPKA', n_getposkeyat);
    cl->AddCmd("ffff_geteulerkeyat_i", 'GEKA', n_geteulerkeyat);
    cl->AddCmd("ffff_getscalekeyat_i", 'GSKA', n_getscalekeyat);
    cl->AddCmd("ffff_getquatkeyat_i",  'GQKA', n_getquatkeyat);
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
    addquatkey
    @input
    f(Time), f(QuatX), f(QuatY), f(QuatZ), f(QuatW)
    @output
    v
    @info
    Add a quaternion key to the rotation key array.
    Warning! DON'T mix quaternions and eulers!
*/
static void
n_addquatkey(void* slf, nCmd* cmd)
{
    nTransformAnimator* self = (nTransformAnimator*) slf;
    quaternion q0;
    float f0 = cmd->In()->GetF();
    q0.x = cmd->In()->GetF();
    q0.y = cmd->In()->GetF();
    q0.z = cmd->In()->GetF();
    q0.w = cmd->In()->GetF();
    self->AddQuatKey(f0, q0);
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
    getnumquatkeys
    @input
    v
    @output
    i(NumQuatKeys)
    @info
    Return number of quaternion keys.
*/
static void
n_getnumquatkeys(void* slf, nCmd* cmd)
{
    nTransformAnimator* self = (nTransformAnimator*) slf;
    cmd->Out()->SetI(self->GetNumQuatKeys());
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
    @cmd
    getquatkeyat
    @input
    i(Index)
    @output
    f(Time), f(QuatX), f(QuatY), f(QuatZ), f(QuatW)
    @info
    Get quaternion key attributes at given index.
*/
static void
n_getquatkeyat(void* slf, nCmd* cmd)
{
    nTransformAnimator* self = (nTransformAnimator*) slf;
    float f0;
    quaternion q0;
    self->GetQuatKeyAt(cmd->In()->GetI(), f0, q0);
    cmd->Out()->SetF(f0);
    cmd->Out()->SetF(q0.x);
    cmd->Out()->SetF(q0.y);
    cmd->Out()->SetF(q0.z);
    cmd->Out()->SetF(q0.w);
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
        numKeys = this->GetNumEulerKeys();
        if (numKeys > 0)
        {
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
        }
        else
        {
            //--- addquatkey ---
            numKeys = this->GetNumQuatKeys();
            for (curKey = 0; curKey < numKeys; curKey++)
            {
                float time;
                quaternion val;
                cmd = ps->GetCmd(this, 'ADQK');
                this->GetQuatKeyAt(curKey, time, val);
                cmd->In()->SetF(time);
                cmd->In()->SetF(val.x);
                cmd->In()->SetF(val.y);
                cmd->In()->SetF(val.z);
                cmd->In()->SetF(val.w);
                ps->PutCmd(cmd);
            }
        }


        //--- addscalekey ---
        numKeys = this->GetNumScaleKeys();
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
