//------------------------------------------------------------------------------
//  (C) 2004 Rafael Van Daele-Hunt
//------------------------------------------------------------------------------
#include "BombsquadBruce/bbofmworld.h"
#include "kernel/npersistserver.h"
#include "BombsquadBruce/general.h"

static void n_setmincollisionheight(void* slf, nCmd* cmd);
static void n_setplayer(void* slf, nCmd* cmd);
static void n_setwater(void* slf, nCmd* cmd);
static void n_setoncollisionscript(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
@scriptclass
cofmworld

@superclass
bbworld

@classinfo
The Bombsquad Bruce version of BBWorld (q.v.)
*/
void
n_initcmds(nClass* clazz)
{
    clazz->BeginCmds();
    clazz->AddCmd("v_setplayer_s",             'STPL', n_setplayer);
    clazz->AddCmd("v_setmincollisionheight_f", 'SMCH', n_setmincollisionheight);
    clazz->AddCmd("v_setwater_s",              'STWR', n_setwater);
    clazz->AddCmd("v_setoncollisionscript_s",  'ONCL', n_setoncollisionscript );
    clazz->EndCmds();
}

//------------------------------------------------------------------------------
/**
@cmd
setmincollisionheight

@input
f

@output
v

@info
Objects shorter than this won't collide with the player
*/
static void n_setmincollisionheight(void* slf, nCmd* cmd)
{
    BBOFMWorld* self = (BBOFMWorld*)slf;
    self->SetMinCollisionHeight(cmd->In()->GetF());
}

//------------------------------------------------------------------------------
/**
@cmd
setplayer

@input
s

@output
v

@info
Sets the world's reference to the player's avatar
*/
static void n_setplayer(void* slf, nCmd* cmd)
{
    BBOFMWorld* self = (BBOFMWorld*)slf;
    self->SetPlayer(cmd->In()->GetS());
}
//------------------------------------------------------------------------------
/**
@cmd
setwater

@input
s (path to BBWater)

@output
v

@info
Sets the world's reference to the global water
*/
static void n_setwater(void* slf, nCmd* cmd)
{
    BBOFMWorld* self = (BBOFMWorld*)slf;
    self->SetWater(cmd->In()->GetS());
}

//------------------------------------------------------------------------------
/**
@cmd
setoncollisionscript

@input
s

@output
v

@info
Sets the script function that will be called if
the player enters collision with an object
(probably by falling on it from above)
*/
static void n_setoncollisionscript(void* slf, nCmd* cmd)
{
    BBOFMWorld* self = (BBOFMWorld*)slf;
    self->SetOnCollisionScript(cmd->In()->GetS());
}
//------------------------------------------------------------------------------
/**
@param  ps          writes the nCmd object contents out to a file.
@return             success or failure
*/
bool
BBOFMWorld::SaveCmds(nPersistServer* ps)
{
    if (nRoot::SaveCmds(ps))
    {
        BBUtil::SaveStrCmd doCmd( ps, this );
        doCmd( m_rWater.getname(), 'SWTR' );
        return true;
    }
    return false;
}
