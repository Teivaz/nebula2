//------------------------------------------------------------------------------
//  (C) 2004 Rafael Van Daele-Hunt
//------------------------------------------------------------------------------
#include "BombsquadBruce/ccgame.h"
#include "kernel/npersistserver.h"

static void n_setworld(void* slf, nCmd* cmd);
static void n_setrenderer(void* slf, nCmd* cmd);
static void n_setgametick(void* slf, nCmd* cmd);
static void n_setcamera(void* slf, nCmd* cmd);
static void n_setdebugcamera(void* slf, nCmd* cmd);
static void n_start(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    ccgame
    
    @superclass
    nroot

    @classinfo
    Overarching game object which is instantiated and takes care of running
    the entire game.
*/
void
n_initcmds(nClass* clazz)
{
    clazz->BeginCmds();
    clazz->AddCmd("v_setworld_s",               'SWLD', n_setworld);
    clazz->AddCmd("v_setrenderer_s",            'SREN', n_setrenderer);
    clazz->AddCmd("v_setgametick_f",            'SGTK', n_setgametick);
    clazz->AddCmd("v_setcamera_s",              'SCAM', n_setcamera);
    clazz->AddCmd("v_setdebugcamera_s",              'SDCM', n_setdebugcamera);
    clazz->AddCmd("v_start_v",                  'GOGO', n_start);
    clazz->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    setworld

    @input
    s

    @output
    v

    @info
    Sets the path to the world where the entities live.
*/
static
void
n_setworld(void* slf, nCmd* cmd)
{
    CCGame* self = (CCGame*)slf;
    self->SetWorld(cmd->In()->GetS());
}
//------------------------------------------------------------------------------
/**
    @cmd
    setrenderer

    @input
    s

    @output
    v

    @info
    Sets the path to the renderer.
*/
static
void
n_setrenderer(void* slf, nCmd* cmd)
{
    CCGame* self = (CCGame*)slf;
    self->SetRenderer(cmd->In()->GetS());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setgametick

    @input
    f (ticks per second)

    @output
    v

    @info
    Sets the number of game state updates per second
*/
static
void
n_setgametick(void* slf, nCmd* cmd)
{
    CCGame* self = (CCGame*)slf;
    self->SetGameTick(cmd->In()->GetF());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setcamera

    @input
    s (NOH path of a CCCamera)

    @output
    v

    @info
    Sets the active camera to attach.
*/
static void n_setcamera(void* slf, nCmd* cmd)
{
    CCGame* self = (CCGame*)slf;
    self->SetCamera(cmd->In()->GetS());
}
//------------------------------------------------------------------------------
/**
    @cmd
    setdebugcamera

    @input
    s (NOH path of a CCCamera)

    @output
    v

    @info
    Sets a debug visualization camera (shows the culling
    of whatever camera was last set with setcamera).
*/
static void n_setdebugcamera(void* slf, nCmd* cmd)
{
    CCGame* self = (CCGame*)slf;
    self->SetDebugCamera(cmd->In()->GetS());
}
//------------------------------------------------------------------------------
/**
    @cmd
    start

    @input
    s

    @output
    v

    @info
    Starts the game.
*/
static void n_start(void* slf, nCmd* cmd)
{
    CCGame* self = (CCGame*)slf;
    self->Start();
}
//------------------------------------------------------------------------------
/**
    @param  fileServer  writes the nCmd object contents out to a file.
    @return             success or failure
*/
bool
CCGame::SaveCmds(nPersistServer* ps)
{
    if (nRoot::SaveCmds(ps))
    {
            nCmd* cmd = ps->GetCmd(this, 'REGS');
      ps->PutCmd(cmd);

        return true;
    }
    return false;
}

