//------------------------------------------------------------------------------
//  (C) 2004 Rafael Van Daele-Hunt
//------------------------------------------------------------------------------
#include "BombsquadBruce/ccwater.h"
#include "kernel/npersistserver.h"

static void n_setbasewaterlevel(void* slf, nCmd* cmd);
static void n_setwaveproperties(void* slf, nCmd* cmd);
static void n_setlayernode(void* slf, nCmd* cmd);
static void n_addlayer(void* slf, nCmd* cmd);
static void n_getlevel(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    ccwater
    
    @superclass
    ccroot

    @classinfo
    The water for Bombsquad Bruce.
*/
void
n_initcmds(nClass* clazz)
{
    clazz->BeginCmds();
    clazz->AddCmd("v_setbasewaterlevel_f", 'SWLV', n_setbasewaterlevel);
    clazz->AddCmd("v_setwaveproperties_ff", 'SWVP', n_setwaveproperties);
    clazz->AddCmd("v_setlayernode_s", 'SLYN', n_setlayernode);
    clazz->AddCmd("v_addlayer_f", 'ADDL', n_addlayer);
    clazz->AddCmd("f_getlevel_v", 'GETL', n_getlevel);
    clazz->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    setbasewaterlevel 

    @input
    f

    @output
    v

    @info
    Sets the global water level zero point
*/
static
void
n_setbasewaterlevel(void* slf, nCmd* cmd)
{
    CCWater* self = (CCWater*) slf;
    self->SetBaseWaterLevel(cmd->In()->GetF());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setwaveproperties

    @input
    f (height) f (frequency)

    @output
    v

    @info
    Water level will vary by +- height, at a speed determined by frequency
*/
static
void
n_setwaveproperties(void* slf, nCmd* cmd)
{
    CCWater* self = (CCWater*) slf;
    const float height = cmd->In()->GetF();
    const float frequency = cmd->In()->GetF();
    self->SetWaveProperties(height, frequency);
}

//------------------------------------------------------------------------------
/**
    @cmd
    setlayernode

    @input
    s (path to nSceneNode)

    @output
    v

    @info
    Sets the nSceneNode used by the underwater depth layers 
*/
static
void
n_setlayernode(void* slf, nCmd* cmd)
{
    CCWater* self = (CCWater*) slf;
    self->SetLayerNode(cmd->In()->GetS());
}

//------------------------------------------------------------------------------
/**
    @cmd
    addlayer

    @input
    f (depth)

    @output
    v

    @info
    Adds an extra plane underneath the surface of the water (at the given depth) 
    to add visual interest and cause opacity to increase based on water depth
*/
static
void
n_addlayer(void* slf, nCmd* cmd)
{
    CCWater* self = (CCWater*) slf;
    const float depth = cmd->In()->GetF();
    self->AddLayer(depth );
}
//------------------------------------------------------------------------------
/**
    @cmd
    getlevel

    @input
    v

    @output
    f (current water level)

    @info
*/
static
void
n_getlevel(void* slf, nCmd* cmd)
{
    CCWater* self = (CCWater*) slf;
    cmd->Out()->SetF(self->GetWaterLevel());
}
//------------------------------------------------------------------------------
/**
    @param  ps          writes the nCmd object contents out to a file.
    @return             success or failure
*/
bool
CCWater::SaveCmds(nPersistServer* ps)
{
    if (CCRoot::SaveCmds(ps))
    {
        nCmd* cmd = ps->GetCmd(this, 'SWVP');
        cmd->In()->SetF(m_WaveHeight);
        cmd->In()->SetF(m_WaveFrequency);
        ps->PutCmd(cmd);
        //ps->GetCmd(this, 'SWVP');
        //cmd->In()->SetF(m_WaveHeight);
        //cmd->In()->SetF(m_WaveFrequency);
        //ps->PutCmd(cmd);
        //ps->GetCmd(this, 'SWVP');
        //cmd->In()->SetF(m_WaveHeight);
        //cmd->In()->SetF(m_WaveFrequency);
        //ps->PutCmd(cmd);
        return true;
    }
    return false;
}

