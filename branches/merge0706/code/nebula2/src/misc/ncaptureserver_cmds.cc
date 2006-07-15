//------------------------------------------------------------------------------
//  ncaptureserver_cmds.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "misc/ncaptureserver.h"

static void n_setbasedirectory(void* slf, nCmd* cmd);
static void n_getbasedirectory(void* slf, nCmd* cmd);
static void n_setframetime(void* slf, nCmd* cmd);
static void n_getframetime(void* slf, nCmd* cmd);
static void n_start(void* slf, nCmd* cmd);
static void n_stop(void* slf, nCmd* cmd);
static void n_toggle(void* slf, nCmd* cmd);
static void n_iscapturing(void* slf, nCmd* cmd);

//-------------------------------------------------------------------
/**
    @scriptclass
    ncaptureserver

    @cppclass
    nCaptureServer

    @superclass
    nroot

    @classinfo
    Provide frame capture capabilities to Nebula2 applications.
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setbasedirectory_s", 'SBSD', n_setbasedirectory);
    cl->AddCmd("s_getbasedirectory_v", 'GBSD', n_getbasedirectory);
    cl->AddCmd("v_setframetime_f",     'SFRT', n_setframetime);
    cl->AddCmd("f_getframetime_v",     'GFRT', n_getframetime);
    cl->AddCmd("v_start_v",            'STRT', n_start);
    cl->AddCmd("v_stop_v",             'STOP', n_stop);
    cl->AddCmd("v_toggle_v",           'TOGL', n_toggle);
    cl->AddCmd("b_iscapturing_v",      'ISCP', n_iscapturing);
    cl->EndCmds();
}

//-------------------------------------------------------------------
/**
    @cmd
    setbasedirectory
    @input
    s(BaseDirectory)
    @output
    v
    @info
    Set the base directory for capturing.
*/
static void
n_setbasedirectory(void* slf, nCmd* cmd)
{
    nCaptureServer* self = (nCaptureServer*) slf;
    self->SetBaseDirectory(cmd->In()->GetS());
}

//-------------------------------------------------------------------
/**
    @cmd
    getbasedirectory
    @input
    v
    @output
    s(BaseDirectory)
    @info
    Get the base directory for capturing.
*/
static void
n_getbasedirectory(void* slf, nCmd* cmd)
{
    nCaptureServer* self = (nCaptureServer*) slf;
    cmd->Out()->SetS(self->GetBaseDirectory().Get());
}

//-------------------------------------------------------------------
/**
    @cmd
    setframetime
    @input
    f(FrameTime)
    @output
    v
    @info
    Set the locked frame time for capturing. Default is 1/25.
*/
static void
n_setframetime(void* slf, nCmd* cmd)
{
    nCaptureServer* self = (nCaptureServer*) slf;
    self->SetFrameTime(cmd->In()->GetF());
}

//-------------------------------------------------------------------
/**
    @cmd
    getframetime
    @input
    v
    @output
    f(FrameTime)
    @info
    Get the locked frame time for capturing.
*/
static void
n_getframetime(void* slf, nCmd* cmd)
{
    nCaptureServer* self = (nCaptureServer*) slf;
    cmd->Out()->SetF(float(self->GetFrameTime()));
}

//-------------------------------------------------------------------
/**
    @cmd
    start
    @input
    v
    @output
    v
    @info
    Start a new capture session.
*/
static void
n_start(void* slf, nCmd* /*cmd*/)
{
    nCaptureServer* self = (nCaptureServer*) slf;
    self->Start();
}

//-------------------------------------------------------------------
/**
    @cmd
    stop
    @input
    v
    @output
    v
    @info
    Stop the current capture session.
*/
static void
n_stop(void* slf, nCmd* /*cmd*/)
{
    nCaptureServer* self = (nCaptureServer*) slf;
    self->Stop();
}

//-------------------------------------------------------------------
/**
    @cmd
    toggle
    @input
    v
    @output
    v
    @info
    Toggle the current capture session.
*/
static void
n_toggle(void* slf, nCmd* /*cmd*/)
{
    nCaptureServer* self = (nCaptureServer*) slf;
    self->Toggle();
}

//-------------------------------------------------------------------
/**
    @cmd
    iscapturing
    @input
    v
    @output
    b(Capturing)
    @info
    Return true if currently capturing.
*/
static void
n_iscapturing(void* slf, nCmd* cmd)
{
    nCaptureServer* self = (nCaptureServer*) slf;
    cmd->Out()->SetB(self->IsCapturing());
}





