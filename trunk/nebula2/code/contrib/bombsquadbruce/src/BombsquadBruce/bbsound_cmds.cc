//------------------------------------------------------------------------------
//  bbsound_cmds.cc
//  (C) 2004 Rafael Van Daele-Hunt
//------------------------------------------------------------------------------
#include "BombsquadBruce/bbsound.h"

static void n_init(void* slf, nCmd* cmd);
static void n_setposition(void* slf, nCmd* cmd);
static void n_setvolume(void* slf, nCmd* cmd);
static void n_play(void* slf, nCmd* cmd);
static void n_stop(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    naudioserver3
    
    @cppclass
    nAudioServer3
    
    @superclass
    nroot

    @classinfo
    Server object of the Nebula2 audio subsystem (in its 3rd incarnation).
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_init_sbb",         'INIT', n_init);
    cl->AddCmd("v_setposition_fff",  'SETP', n_setposition);
    cl->AddCmd("v_setvolume_f",      'SETV', n_setvolume);
    cl->AddCmd("v_play_v",           'PLAY', n_play);
    cl->AddCmd("v_stop_v",           'STOP', n_stop);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    init
    @input
    s (name of the sound file), b (loop), b (ambient)
    @output
    v
    @info
    Initialize the sound from a .wav or .ogg.
    loop: true->loop, false->one time.
    ambient: true->sound is omnipresent, false->sound has a position
*/
static void
n_init(void* slf, nCmd* cmd)
{
    BBSound* self = (BBSound*) slf;
    const char* name = cmd->In()->GetS();
    bool loop = cmd->In()->GetB();
    self->Init(name, loop, cmd->In()->GetB());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setposition
    @input
    fff (position in worldspace)
    @output
    v
    @info
    Sets the sound's apparent location.
*/
static void
n_setposition(void* slf, nCmd* cmd)
{
    BBSound* self = (BBSound*) slf;
    vector3 pos;
    pos.x = cmd->In()->GetF();
    pos.y = cmd->In()->GetF();
    pos.z = cmd->In()->GetF();
    self->SetPosition(pos);
}

//------------------------------------------------------------------------------
/**
    @cmd
    setvolume
    @input
    f (new volume)
    @output
    v
    @info
    Sets the sound's volume.
*/
static void
n_setvolume(void* slf, nCmd* cmd)
{
    BBSound* self = (BBSound*) slf;
    self->SetVolume(cmd->In()->GetF());
}

//------------------------------------------------------------------------------
/**
    @cmd
    play
    @input
    v
    @output
    v
    @info
    Plays the sound.
*/
static void
n_play(void* slf, nCmd* cmd)
{
    BBSound* self = (BBSound*) slf;
    self->Play();
}


//------------------------------------------------------------------------------
/**
    @cmd
    stop
    @input
    v
    @output
    v
    @info
    Stops playing the sound.
*/
static void
n_stop(void* slf, nCmd* cmd)
{
    BBSound* self = (BBSound*) slf;
    self->Stop();
}



