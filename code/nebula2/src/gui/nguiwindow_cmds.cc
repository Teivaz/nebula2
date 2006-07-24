//------------------------------------------------------------------------------
//  nguiwindow_cmds.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gui/nguiwindow.h"

static void n_setescapecommand(void* slf, nCmd* cmd);
static void n_getescapecommand(void* slf, nCmd* cmd);
static void n_setmodal(void* slf, nCmd* cmd);
static void n_ismodal(void* slf, nCmd* cmd);
static void n_setdismissed(void* slf, nCmd* cmd);
static void n_isdismissed(void* slf, nCmd* cmd);
static void n_setfadeintime(void* slf, nCmd* cmd);
static void n_setfadeouttime(void* slf, nCmd* cmd);
static void n_close(void* slf, nCmd* cmd);
static void n_setclosecommand(void* slf, nCmd* cmd);
static void n_getclosecommand(void* slf, nCmd* cmd);

//-----------------------------------------------------------------------------
/**
    @scriptclass
    nguiwindow

    @cppclass
    nGuiWindow

    @superclass
    nguiwidget

    @classinfo
    A button widget.

    - 07-Jun-05    kims    Added 'close', 'setclosecommand' and 'getclosecommand'
                           to provide fading and prevent crashes when a custom
                           nguibutton tries to close parent window.
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setescapecommand_s",  'SESH', n_setescapecommand);
    cl->AddCmd("s_getescapecommand_v",  'GESH', n_getescapecommand);
    cl->AddCmd("v_setmodal_b",          'SMOD', n_setmodal);
    cl->AddCmd("b_ismodal_v",           'ISMD', n_ismodal);
    cl->AddCmd("v_setdismissed_b",      'SDIS', n_setdismissed);
    cl->AddCmd("b_isdismissed_v",       'ISDS', n_isdismissed);
    cl->AddCmd("v_setfadeintime_f",     'SFIT', n_setfadeintime);
    cl->AddCmd("v_setfadeouttime_f",    'SFOT', n_setfadeouttime);
    cl->AddCmd("v_close_v",             'CLOS', n_close);
    cl->AddCmd("v_setclosecommand_s",   'STCC', n_setclosecommand);
    cl->AddCmd("s_getclosecommand_v",   'GTCC', n_getclosecommand);
    cl->EndCmds();
}

//-----------------------------------------------------------------------------
/**
    @cmd
    setescapecommand
    @input
    s(EscapeCommand)
    @output
    v
    @info
    Set script command to be executed when escape key is pressed.
*/
static void
n_setescapecommand(void* slf, nCmd* cmd)
{
    nGuiWindow* self = (nGuiWindow*) slf;
    self->SetEscapeCommand(cmd->In()->GetS());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    getescapecommand
    @input
    v
    @output
    s(EscapeCommand)
    @info
    Get script command to be executed when escape key is pressed.
*/
static void
n_getescapecommand(void* slf, nCmd* cmd)
{
    nGuiWindow* self = (nGuiWindow*) slf;
    cmd->Out()->SetS(self->GetEscapeCommand());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    setmodal
    @input
    b(ModalFlag)
    @output
    v
    @info
    Set the modal flag.
*/
static void
n_setmodal(void* slf, nCmd* cmd)
{
    nGuiWindow* self = (nGuiWindow*) slf;
    self->SetModal(cmd->In()->GetB());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    ismodal
    @input
    v
    @output
    b(ModalFlag)
    @info
    Get the modal flag of the window.
*/
static void
n_ismodal(void* slf, nCmd* cmd)
{
    nGuiWindow* self = (nGuiWindow*) slf;
    cmd->Out()->SetB(self->IsModal());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    setdismissed
    @input
    b
    @output
    v
    @info
    Set the dismissed flag. Dismissed windows will be removed by the
    gui server after windows have been triggered.

    FIXME: For now, this works only for toplevel windows.
*/
static void
n_setdismissed(void* slf, nCmd* cmd)
{
    nGuiWindow* self = (nGuiWindow*) slf;
    self->SetDismissed(cmd->In()->GetB());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    isdismissed
    @input
    v
    @output
    v
    @info
    Get the widget's dismissed flag.
*/
static void
n_isdismissed(void* slf, nCmd* cmd)
{
    nGuiWindow* self = (nGuiWindow*) slf;
    cmd->Out()->SetB(self->IsDismissed());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    setfadeintime
    @input
    f
    @output
    v
    @info
    Set the number of seconds the menu takes to fade in from transparency.
*/
static void
n_setfadeintime(void* slf, nCmd* cmd)
{
    nGuiWindow* self = (nGuiWindow*) slf;
    self->SetFadeInTime(cmd->In()->GetF());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    setfadeouttime
    @input
    f
    @output
    v
    @info
    Set the number of seconds the menu takes to fade out from transparency.
*/
static void
n_setfadeouttime(void* slf, nCmd* cmd)
{
    nGuiWindow* self = (nGuiWindow*) slf;
    self->SetFadeOutTime(cmd->In()->GetF());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    close
    @input
    v
    @output
    v
    @info
    Send close request to the window
+*/
static void
n_close(void* slf, nCmd* /*cmd*/)
{
    nGuiWindow* self = (nGuiWindow*) slf;
    self->SetCloseRequested(true);
}

//-----------------------------------------------------------------------------
/**
    @cmd
    setclosecommand
    @input
    s(CloseCommand)
    @output
    v
    @info
    Set script command to be executed when window is closed.
+*/
static void
n_setclosecommand(void* slf, nCmd* cmd)
{
    nGuiWindow* self = (nGuiWindow*) slf;
    self->SetCloseCommand(cmd->In()->GetS());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    getclosecommand
    @input
    v
    @output
    s(CloseCommand)
    @info
    Get script command to be executed when window is closed.
+*/
static void
n_getclosecommand(void* slf, nCmd* cmd)
{
    nGuiWindow* self = (nGuiWindow*) slf;
    cmd->Out()->SetS(self->GetCloseCommand());
}
