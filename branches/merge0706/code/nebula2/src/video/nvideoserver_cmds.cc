//------------------------------------------------------------------------------
//  nvideoserver_cmds.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "video/nvideoserver.h"

static void n_open(void* slf, nCmd* cmd);
static void n_close(void* slf, nCmd* cmd);
static void n_isopen(void* slf, nCmd* cmd);
static void n_playfile(void* slf, nCmd* cmd);
static void n_stop(void* slf, nCmd* cmd);
static void n_isplaying(void* slf, nCmd* cmd);
static void n_setenablescaling(void* slf, nCmd* cmd);
static void n_getenablescaling(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nvideoserver

    @cppclass
    nVideoServer

    @superclass
    nroot

    @classinfo
    An abstract video playback server object.
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("b_open_v",              'OPEN', n_open);
    cl->AddCmd("v_close_v",             'CLOS', n_close);
    cl->AddCmd("b_isopen_v",            'ISOP', n_isopen);
    cl->AddCmd("b_playfile_s",          'PLFL', n_playfile);
    cl->AddCmd("v_stop_v",              'STOP', n_stop);
    cl->AddCmd("b_isplaying_v",         'ISPL', n_isplaying);
    cl->AddCmd("v_setenablescaling_b",  'SESC', n_setenablescaling);
    cl->AddCmd("b_getenablescaling_v",  'GESC', n_getenablescaling);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    open
    @input
    v
    @output
    v
    @info
    Open the video server.
*/
static void
n_open(void* slf, nCmd* /*cmd*/)
{
    nVideoServer* self = (nVideoServer*) slf;
    self->Open();
}

//------------------------------------------------------------------------------
/**
    @cmd
    close
    @input
    v
    @output
    v
    @info
    Close the video server.
*/
static void
n_close(void* slf, nCmd* /*cmd*/)
{
    nVideoServer* self = (nVideoServer*) slf;
    self->Close();
}

//------------------------------------------------------------------------------
/**
    @cmd
    isopen
    @input
    v
    @output
    b(IsOpen)
    @info
    Return true if video server is open.
*/
static void
n_isopen(void* slf, nCmd* cmd)
{
    nVideoServer* self = (nVideoServer*) slf;
    cmd->Out()->SetB(self->IsOpen());
}

//------------------------------------------------------------------------------
/**
    @cmd
    playfile
    @input
    s(Filename)
    @output
    b(Success)
    @info
    Start playback of a video file.
*/
static void
n_playfile(void* slf, nCmd* cmd)
{
    nVideoServer* self = (nVideoServer*) slf;
    cmd->Out()->SetB(self->PlayFile(cmd->In()->GetS()));
}

//------------------------------------------------------------------------------
/**
    @cmd
    stopfile
    @input
    v
    @output
    v
    @info
    Stop currently playing video.
*/
static void
n_stop(void* slf, nCmd* /*cmd*/)
{
    nVideoServer* self = (nVideoServer*) slf;
    self->Stop();
}

//------------------------------------------------------------------------------
/**
    @cmd
    isplaying
    @input
    v
    @output
    b
    @info
    Return true if currently playing.
*/
static void
n_isplaying(void* slf, nCmd* cmd)
{
    nVideoServer* self = (nVideoServer*) slf;
    cmd->Out()->SetB(self->IsPlaying());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setenablescaling
    @input
    b(Scaling)
    @output
    v
    @info
    Enable/disable scaling to screen size.
*/
static void
n_setenablescaling(void* slf, nCmd* cmd)
{
    nVideoServer* self = (nVideoServer*) slf;
    self->SetEnableScaling(cmd->In()->GetB());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getenablescaling
    @input
    v
    @output
    b(Scaling)
    @info
    Get the scale-to-screensize flag.
*/
static void
n_getenablescaling(void* slf, nCmd* cmd)
{
    nVideoServer* self = (nVideoServer*) slf;
    cmd->Out()->SetB(self->GetEnableScaling());
}


