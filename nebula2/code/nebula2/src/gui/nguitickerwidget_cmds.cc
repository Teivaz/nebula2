//------------------------------------------------------------------------------
//  nguitickerwidget_cmds.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gui/nguitickerwidget.h"

static void n_setscrollspeed(void* slf, nCmd* cmd);
static void n_getscrollspeed(void* slf, nCmd* cmd);
static void n_restart(void* slf, nCmd* cmd);

//-----------------------------------------------------------------------------
/**
    @scriptclass
    nguitickerwidget
    @superclass
    nguitextlabel
    @classinfo
    A text lable that moves from left to right.
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setscrollspeed_f",   'SSCS', n_setscrollspeed);
    cl->AddCmd("f_getscrollspeed_v",   'GSCS', n_getscrollspeed);
    cl->AddCmd("v_restart_v",          'REST', n_restart);
    cl->EndCmds();
}

//-----------------------------------------------------------------------------
/**
    @cmd
    setscrollspeed
    @input
    f(seconds)
    @output
    v
    @info
    Set how many seconds a char needs to move over the full screen
*/
static void
n_setscrollspeed(void* slf, nCmd* cmd)
{
    nGuiTickerWidget* self = (nGuiTickerWidget*) slf;
    self->SetScrollSpeed(cmd->In()->GetF());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    getscrollspeed
    @input
    v
    @output
    f(seconds)
    @info
    get how many seconds a char needs to move over the full screen
*/
static void
n_getscrollspeed(void* slf, nCmd* cmd)
{
    nGuiTickerWidget* self = (nGuiTickerWidget*) slf;
    cmd->Out()->SetF(self->GetScrollSpeed());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    restart
    @input
    v
    @output
    v
    @info
    restart the scrolling of the widget
*/
static void
n_restart(void* slf, nCmd* /*cmd*/)
{
    nGuiTickerWidget* self = (nGuiTickerWidget*) slf;
    self->Restart();
}
