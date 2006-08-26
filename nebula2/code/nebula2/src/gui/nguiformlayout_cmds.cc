//------------------------------------------------------------------------------
//  nguiformlayout_cmds.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gui/nguiformlayout.h"

static void n_attachform(void* slf, nCmd* cmd);
static void n_attachwidget(void* slf, nCmd* cmd);
static void n_attachpos(void* slf, nCmd* cmd);

//-----------------------------------------------------------------------------
/**
    @scriptclass
    nguiformlayout

    @cppclass
    nGuiFormLayout

    @superclass
    nguiwidget

    @classinfo
    A nguiformlayout arranges the position of size of child widgets
    according to the layout rules "Attach To Form", "Attach To Widget" or
    "Attach To Pos".
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("i_attachform_osf",      'AFRM', n_attachform);
    cl->AddCmd("i_attachwidget_osof",   'AWGT', n_attachwidget);
    cl->AddCmd("i_attachpos_osf",       'APOS', n_attachpos);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    attachform
    @input
    o(Widget), s(Edge=top|bottom|left|right)
    @output
    v
    @info
    Attaches the given widget's edge to one of the form layout's edges.
*/
static void
n_attachform(void* slf, nCmd* cmd)
{
    nGuiFormLayout* self = (nGuiFormLayout*) slf;
    nGuiWidget* widget = (nGuiWidget*) cmd->In()->GetO();
    nGuiFormLayout::Edge edge = nGuiFormLayout::StringToEdge(cmd->In()->GetS());
    float offset = cmd->In()->GetF();
    n_assert(widget->IsA(nKernelServer::Instance()->FindClass("nguiwidget")));
    cmd->Out()->SetI(self->AttachForm(widget, edge, offset));
}

//------------------------------------------------------------------------------
/**
    @cmd
    attachwidget
    @input
    o(Widget), s(Edge=top|bottom|left|right), o(OtherWidget)
    @output
    v
    @info
    Attaches the given widget's edge to one of the other child widgets close
    edge.
*/
static void
n_attachwidget(void* slf, nCmd* cmd)
{
    nGuiFormLayout* self = (nGuiFormLayout*) slf;
    nGuiWidget* widget = (nGuiWidget*) cmd->In()->GetO();
    nGuiFormLayout::Edge edge = nGuiFormLayout::StringToEdge(cmd->In()->GetS());
    nGuiWidget* other = (nGuiWidget*) cmd->In()->GetO();
    float offset = cmd->In()->GetF();
    n_assert(widget->IsA(nKernelServer::Instance()->FindClass("nguiwidget")));
    n_assert(other->IsA(nKernelServer::Instance()->FindClass("nguiwidget")));
    cmd->Out()->SetI(self->AttachWidget(widget, edge, other, offset));
}

//------------------------------------------------------------------------------
/**
    @cmd
    attachpos
    @input
    o(Widget), s(Edge=top|bottom|left|right), f(RelativePos)
    @output
    v
    @info
    Attaches the given widget's edge to a relative position (0.0 .. 1.0)
    in the form.
*/
static void
n_attachpos(void* slf, nCmd* cmd)
{
    nGuiFormLayout* self = (nGuiFormLayout*) slf;
    nGuiWidget* widget = (nGuiWidget*) cmd->In()->GetO();
    nGuiFormLayout::Edge edge = nGuiFormLayout::StringToEdge(cmd->In()->GetS());
    float pos = cmd->In()->GetF();
    n_assert(widget->IsA(nKernelServer::Instance()->FindClass("nguiwidget")));
    cmd->Out()->SetI(self->AttachPos(widget, edge, pos));
}
