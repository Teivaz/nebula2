#include "kernel/npersistserver.h"
#include "BombsquadBruce/bbengine.h"

static void n_stopengine(void* slf, nCmd* cmd);
static void n_setgame(void* slf, nCmd* cmd);
static void n_loadobject(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    bbengine
    
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
    clazz->AddCmd("v_stopengine_v", 'STOP', n_stopengine);
    clazz->AddCmd("v_setgame_s",    'SETG', n_setgame);
    clazz->AddCmd("v_loadobject_s", 'LOAD', n_loadobject);
    clazz->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    stopengine

    @input
    v

    @output
    v

    @info
    Stops engine, exits
*/
static
void
n_stopengine(void* slf, nCmd* cmd)
{
    BBEngine* self = (BBEngine*)slf;
    self->StopEngine();
}
//------------------------------------------------------------------------------
/**
    @cmd
    setgame

    @input
    s (game path)

    @output
    v

    @info
    Switches the active game, deactivating the old game (if any) and 
    initializing the new one.
*/
static
void
n_setgame(void* slf, nCmd* cmd)
{
    BBEngine* self = (BBEngine*)slf;
    self->SetGame(cmd->In()->GetS());
}
//------------------------------------------------------------------------------
/**
    @cmd
    loadobject

    @input
    s (path to .n2 file)

    @output
    v

    @info
    Loads a persistent object from an .n2 file.
    The path should have the form:
    [assignDir:][path]filename.n2
    [path] = subdir/[path]
*/
static
void
n_loadobject(void* slf, nCmd* cmd)
{
    BBEngine* self = (BBEngine*)slf;
    self->LoadObject(cmd->In()->GetS());
}
