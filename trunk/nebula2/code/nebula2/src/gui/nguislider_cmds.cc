//------------------------------------------------------------------------------
//  nguislider_cmds.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gui/nguislider.h"

static void n_setrange(void* slf, nCmd* cmd);
static void n_getrange(void* slf, nCmd* cmd);
static void n_setstepsize(void* slf, nCmd* cmd);
static void n_getstepsize(void* slf, nCmd* cmd);
static void n_sethorizontal(void* slf, nCmd* cmd);
static void n_gethorizontal(void* slf, nCmd* cmd);
static void n_setbackgroundbrush(void* slf, nCmd* cmd);
static void n_getbackgroundbrush(void* slf, nCmd* cmd);

//-----------------------------------------------------------------------------
/**
    @scriptclass
    nguislider
    @superclass
    nguiwidget
    @classinfo
    A horizontal or vertical slider widget.
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setrange_ff",             'SRNG', n_setrange);
    cl->AddCmd("ff_getrange_v",             'GRNG', n_getrange);
    cl->AddCmd("v_setstepsize_f",           'SSTP', n_setstepsize);
    cl->AddCmd("f_getstepsize_v",           'GSTP', n_getstepsize);
    cl->AddCmd("v_sethorizontal_b",         'SHRZ', n_sethorizontal);
    cl->AddCmd("b_gethorizontal_v",         'GHRZ', n_gethorizontal);
    cl->AddCmd("v_setbackgroundbrush_s",    'SBGB', n_setbackgroundbrush);
    cl->AddCmd("s_getbackgroundbrush_v",    'GBGB', n_getbackgroundbrush);
    cl->EndCmds();
}

//-----------------------------------------------------------------------------
/**
    @cmd
    setrange
    @input
    f(MinVal), f(MaxVal)
    @output
    v
    @info
    Set the slider range (between 0.0 and 1.0).
*/
static void
n_setrange(void* slf, nCmd* cmd)
{
    nGuiSlider* self = (nGuiSlider*) slf;
    float f0 = cmd->In()->GetF();
    float f1 = cmd->In()->GetF();
    self->SetRange(f0, f1);
}

//-----------------------------------------------------------------------------
/**
    @cmd
    getrange
    @input
    v
    @output
    f(MinVal), f(MaxVal)
    @info
    Get the slider range (between 0.0 and 1.0).
*/
static void
n_getrange(void* slf, nCmd* cmd)
{
    nGuiSlider* self = (nGuiSlider*) slf;
    float f0, f1;
    self->GetRange(f0, f1);
    cmd->Out()->SetF(f0);
    cmd->Out()->SetF(f1);
}

//-----------------------------------------------------------------------------
/**
    @cmd
    setstepsize
    @input
    f(StepSize)
    @output
    v
    @info
    Set the digital step size.
*/
static void
n_setstepsize(void* slf, nCmd* cmd)
{
    nGuiSlider* self = (nGuiSlider*) slf;
    self->SetStepSize(cmd->In()->GetF());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    getstepsize
    @input
    v
    @output
    f(StepSize)
    @info
    Get the digital step size.
*/
static void
n_getstepsize(void* slf, nCmd* cmd)
{
    nGuiSlider* self = (nGuiSlider*) slf;
    cmd->Out()->SetF(self->GetStepSize());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    sethorizontal
    @input
    b(HorizontalFlag)
    @output
    v
    @info
    Set to true if this is a horizontal slider.
*/
static void
n_sethorizontal(void* slf, nCmd* cmd)
{
    nGuiSlider* self = (nGuiSlider*) slf;
    self->SetHorizontal(cmd->In()->GetB());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    gethorizontal
    @input
    v
    @output
    b(HorizontalFlag)
    @info
    Get horizontal flag.
*/
static void
n_gethorizontal(void* slf, nCmd* cmd)
{
    nGuiSlider* self = (nGuiSlider*) slf;
    cmd->Out()->SetB(self->GetHorizontal());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    setbackgroundbrush
    @input
    s(BrushName)
    @output
    v
    @info
    Set the brush to be used for rendering the slider background.
*/
static void
n_setbackgroundbrush(void* slf, nCmd* cmd)
{
    nGuiSlider* self = (nGuiSlider*) slf;
    self->SetBackgroundBrush(cmd->In()->GetS());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    getbackgroundbrush
    @input
    v
    @output
    s(BrushName)
    @info
    Get the brush to be used for rendering the slider background.
*/
static void
n_getbackgroundbrush(void* slf, nCmd* cmd)
{
    nGuiSlider* self = (nGuiSlider*) slf;
    cmd->Out()->SetS(self->GetBackgroundBrush());
}
