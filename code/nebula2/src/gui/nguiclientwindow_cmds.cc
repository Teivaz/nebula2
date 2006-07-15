//------------------------------------------------------------------------------
//  nguiclientwindow_cmds.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gui/nguiclientwindow.h"

static void n_setmovable(void* slf, nCmd* cmd);
static void n_ismovable(void* slf, nCmd* cmd);
static void n_setresizable(void* slf, nCmd* cmd);
static void n_isresizable(void* slf, nCmd* cmd);
static void n_setclosebutton(void* slf, nCmd* cmd);
static void n_hasclosebutton(void* slf, nCmd* cmd);
static void n_settitle(void* slf, nCmd* cmd);
static void n_gettitle(void* slf, nCmd* cmd);

//-----------------------------------------------------------------------------
/**
    @scriptclass
    nguiclientwindow

    @cppclass
    nGuiClientWindow

    @superclass
    nguiwindow

    @classinfo
    A window with optional close button, title bar, resize button and
    client area.
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setmovable_b",       'SMOV', n_setmovable);
    cl->AddCmd("b_ismovable_v",        'ISMV', n_ismovable);
    cl->AddCmd("v_setresizable_b",     'SRES', n_setresizable);
    cl->AddCmd("b_isresizable_v",      'ISRS', n_isresizable);
    cl->AddCmd("v_setclosebutton_b",    'SCLB', n_setclosebutton);
    cl->AddCmd("b_hasclosebutton_v",    'GCLB', n_hasclosebutton);
    cl->AddCmd("v_settitle_s",          'STIT', n_settitle);
    cl->AddCmd("s_gettitle_v",          'GTIT', n_gettitle);
    cl->EndCmds();
}

//-----------------------------------------------------------------------------
/**
    @cmd
    setmovable
    @input
    b(Movable)
    @output
    v
    @info
    Set window's movable flag.
*/
static void
n_setmovable(void* slf, nCmd* cmd)
{
    nGuiClientWindow* self = (nGuiClientWindow*) slf;
    self->SetMovable(cmd->In()->GetB());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    ismovable
    @input
    v
    @output
    b(Movable)
    @info
    Get window's movable flag.
*/
static void
n_ismovable(void* slf, nCmd* cmd)
{
    nGuiClientWindow* self = (nGuiClientWindow*) slf;
    cmd->Out()->SetB(self->IsMovable());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    setresizable
    @input
    b(Resizable)
    @output
    v
    @info
    Set window's resizable flag.
*/
static void
n_setresizable(void* slf, nCmd* cmd)
{
    nGuiClientWindow* self = (nGuiClientWindow*) slf;
    self->SetResizable(cmd->In()->GetB());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    isresizable
    @input
    v
    @output
    b(Resizable)
    @info
    Get window's resizable flag.
*/
static void
n_isresizable(void* slf, nCmd* cmd)
{
    nGuiClientWindow* self = (nGuiClientWindow*) slf;
    cmd->Out()->SetB(self->IsResizable());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    setclosebutton
    @input
    b(HasCloseButton)
    @output
    v
    @info
    Set whether the window has a close button or not.
*/
static void
n_setclosebutton(void* slf, nCmd* cmd)
{
    nGuiClientWindow* self = (nGuiClientWindow*) slf;
    self->SetCloseButton(cmd->In()->GetB());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    hasclosebutton
    @input
    v
    @output
    b(HasCloseButton)
    @info
    Return true if window has a close button.
*/
static void
n_hasclosebutton(void* slf, nCmd* cmd)
{
    nGuiClientWindow* self = (nGuiClientWindow*) slf;
    cmd->Out()->SetB(self->HasCloseButton());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    settitle
    @input
    s(Title)
    @output
    v
    @info
    Set the window title.
*/
static void
n_settitle(void* slf, nCmd* cmd)
{
    nGuiClientWindow* self = (nGuiClientWindow*) slf;
    self->SetTitle(cmd->In()->GetS());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    gettitle
    @input
    v
    @output
    s(Title)
    @info
    Get the window title.
*/
static void
n_gettitle(void* slf, nCmd* cmd)
{
    nGuiClientWindow* self = (nGuiClientWindow*) slf;
    cmd->Out()->SetS(self->GetTitle());
}

