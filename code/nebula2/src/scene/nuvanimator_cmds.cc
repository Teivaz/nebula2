//------------------------------------------------------------------------------
//  nuvanimator_cmds.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "scene/nuvanimator.h"
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
    nuvanimator

    @cppclass
    nUvAnimator

    @superclass
    nanimator

    @classinfo
    Animate UV coordinates of a shape node.
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_addposkey_ifff",     'ADPK', n_addposkey);
    cl->AddCmd("v_addeulerkey_ifff",   'ADEK', n_addeulerkey);
    cl->AddCmd("v_addscalekey_ifff",   'ADSK', n_addscalekey);
    cl->AddCmd("i_getnumposkeys_i",    'GNPK', n_getnumposkeys);
    cl->AddCmd("i_getnumeulerkeys_i",  'GNEK', n_getnumeulerkeys);
    cl->AddCmd("i_getnumscalekeys_i",  'GNSK', n_getnumscalekeys);
    cl->AddCmd("fff_getposkeyat_ii",   'GPKA', n_getposkeyat);
    cl->AddCmd("fff_geteulerkeyat_ii", 'GEKA', n_geteulerkeyat);
    cl->AddCmd("fff_getscalekeyat_ii", 'GSKA', n_getscalekeyat);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    addposkey
    @input
    i(TextureLayer), f(Time), f(PosU), f(PosV)
    @output
    v
    @info
    Add a position key to the position key array.
*/
static void
n_addposkey(void* slf, nCmd* cmd)
{
    nUvAnimator* self = (nUvAnimator*) slf;
    vector2 v0;
    int i0 = cmd->In()->GetI();
    float f0 = cmd->In()->GetF();
    v0.x = cmd->In()->GetF();
    v0.y = cmd->In()->GetF();
    self->AddPosKey(i0, f0, v0);
}

//------------------------------------------------------------------------------
/**
    @cmd
    addeulerkey
    @input
    i(TextureLayer), f(Time), f(EulerU), f(EulerV)
    @output
    v
    @info
    Add a euler angle key to the position key array.
*/
static void
n_addeulerkey(void* slf, nCmd* cmd)
{
    nUvAnimator* self = (nUvAnimator*) slf;
    vector2 v0;
    int i0 = cmd->In()->GetI();
    float f0 = cmd->In()->GetF();
    v0.x = n_deg2rad(cmd->In()->GetF());
    v0.y = n_deg2rad(cmd->In()->GetF());
    self->AddEulerKey(i0, f0, v0);
}

//------------------------------------------------------------------------------
/**
    @cmd
    addscalekey
    @input
    i(TextureLayer), f(Time), f(ScaleU), f(ScaleV)
    @output
    v
    @info
    Add a scale key to the position key array.
*/
static void
n_addscalekey(void* slf, nCmd* cmd)
{
    nUvAnimator* self = (nUvAnimator*) slf;
    vector2 v0;
    int i0 = cmd->In()->GetI();
    float f0 = cmd->In()->GetF();
    v0.x = cmd->In()->GetF();
    v0.y = cmd->In()->GetF();
    self->AddScaleKey(i0, f0, v0);
}

//------------------------------------------------------------------------------
/**
    @cmd
    getnumposkeys
    @input
    i(TextureLayer)
    @output
    i(NumPosKeys)
    @info
    Return number of position keys.
*/
static void
n_getnumposkeys(void* slf, nCmd* cmd)
{
    nUvAnimator* self = (nUvAnimator*) slf;
    cmd->Out()->SetI(self->GetNumPosKeys(cmd->In()->GetI()));
}

//------------------------------------------------------------------------------
/**
    @cmd
    getnumeulerkeys
    @input
    i(TextureLayer)
    @output
    i(NumEulerKeys)
    @info
    Return number of euler keys.
*/
static void
n_getnumeulerkeys(void* slf, nCmd* cmd)
{
    nUvAnimator* self = (nUvAnimator*) slf;
    cmd->Out()->SetI(self->GetNumEulerKeys(cmd->In()->GetI()));
}

//------------------------------------------------------------------------------
/**
    @cmd
    getnumscalekeys
    @input
    i(TextureLayer)
    @output
    i(NumScaleKeys)
    @info
    Return number of scale keys.
*/
static void
n_getnumscalekeys(void* slf, nCmd* cmd)
{
    nUvAnimator* self = (nUvAnimator*) slf;
    cmd->Out()->SetI(self->GetNumScaleKeys(cmd->In()->GetI()));
}

//------------------------------------------------------------------------------
/**
    @cmd
    getposkeyat
    @input
    i(TextureLayer), i(KeyIndex)
    @output
    f(Time), f(PosU), f(PosU)
    @info
    Get position key attributes at given index.
*/
static void
n_getposkeyat(void* slf, nCmd* cmd)
{
    nUvAnimator* self = (nUvAnimator*) slf;
    float f0;
    vector2 v0;
    int i0 = cmd->In()->GetI();
    int i1 = cmd->In()->GetI();
    self->GetPosKeyAt(i0, i1, f0, v0);
    cmd->Out()->SetF(f0);
    cmd->Out()->SetF(v0.x);
    cmd->Out()->SetF(v0.y);
}

//------------------------------------------------------------------------------
/**
    @cmd
    geteulerkeyat
    @input
    i(TextureLayer), i(KeyIndex)
    @output
    f(Time), f(EulerU), f(EulerV)
    @info
    Get euler key attributes at given index.
*/
static void
n_geteulerkeyat(void* slf, nCmd* cmd)
{
    nUvAnimator* self = (nUvAnimator*) slf;
    float f0;
    vector2 v0;
    int i0 = cmd->In()->GetI();
    int i1 = cmd->In()->GetI();
    self->GetEulerKeyAt(i0, i1, f0, v0);
    cmd->Out()->SetF(f0);
    cmd->Out()->SetF(n_rad2deg(v0.x));
    cmd->Out()->SetF(n_rad2deg(v0.y));
}

//------------------------------------------------------------------------------
/**
    @cmd
    getscalekeyat
    @input
    i(TextureLayer), i(KeyIndex)
    @output
    f(Time), f(ScaleU), f(ScaleV)
    @info
    Get scale key attributes at given index.
*/
static void
n_getscalekeyat(void* slf, nCmd* cmd)
{
    nUvAnimator* self = (nUvAnimator*) slf;
    float f0;
    vector2 v0;
    int i0 = cmd->In()->GetI();
    int i1 = cmd->In()->GetI();
    self->GetScaleKeyAt(i0, i1, f0, v0);
    cmd->Out()->SetF(f0);
    cmd->Out()->SetF(v0.x);
    cmd->Out()->SetF(v0.y);
}

//------------------------------------------------------------------------------
/**
*/
bool
nUvAnimator::SaveCmds(nPersistServer* ps)
{
    if (nAnimator::SaveCmds(ps))
    {
        int texLayer;
        vector2 val;
        for (texLayer = 0; texLayer < nGfxServer2::MaxTextureStages; texLayer++)
        {
            nCmd* cmd;
            int curKey;
            int numKeys;

            //--- addposkey ---
            numKeys = this->GetNumPosKeys(texLayer);
            for (curKey = 0; curKey < numKeys; curKey++)
            {
                float time;
                cmd = ps->GetCmd(this, 'ADPK');
                this->GetPosKeyAt(texLayer, curKey, time, val);
                cmd->In()->SetI(texLayer);
                cmd->In()->SetF(time);
                cmd->In()->SetF(val.x);
                cmd->In()->SetF(val.y);
                ps->PutCmd(cmd);
            }

            //--- addeulerkey ---
            numKeys = this->GetNumEulerKeys(texLayer);
            for (curKey = 0; curKey < numKeys; curKey++)
            {
                float time;
                cmd = ps->GetCmd(this, 'ADEK');
                this->GetEulerKeyAt(texLayer, curKey, time, val);
                cmd->In()->SetI(texLayer);
                cmd->In()->SetF(time);
                cmd->In()->SetF(n_rad2deg(val.x));
                cmd->In()->SetF(n_rad2deg(val.y));
                ps->PutCmd(cmd);
            }


            //--- addscalekey ---
            numKeys = this->GetNumScaleKeys(texLayer);
            for (curKey = 0; curKey < numKeys; curKey++)
            {
                float time;
                cmd = ps->GetCmd(this, 'ADSK');
                this->GetScaleKeyAt(texLayer, curKey, time, val);
                cmd->In()->SetI(texLayer);
                cmd->In()->SetF(time);
                cmd->In()->SetF(val.x);
                cmd->In()->SetF(val.y);
                ps->PutCmd(cmd);
            }
        }
        return true;
    }
    return false;
}


