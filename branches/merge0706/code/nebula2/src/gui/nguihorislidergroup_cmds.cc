//------------------------------------------------------------------------------
//  nguihorislidergroup_cmds.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gui/nguihorislidergroup.h"

static void n_setlefttext(void* slf, nCmd* cmd);
static void n_setrighttext(void* slf, nCmd* cmd);
static void n_setminvalue(void* slf, nCmd* cmd);
static void n_setmaxvalue(void* slf, nCmd* cmd);
static void n_setvalue(void* slf, nCmd* cmd);
static void n_getvalue(void* slf, nCmd* cmd);
static void n_setleftwidth(void* slf, nCmd* cmd);
static void n_setrightwidth(void* slf, nCmd* cmd);
static void n_setlabelfont(void* slf, nCmd* cmd);
static void n_setknobsize(void* slf, nCmd* cmd);
//-----------------------------------------------------------------------------
/**
    @scriptclass
    nguihorislidergroup

    @cppclass
    nGuiHoriSliderGroup

    @superclass
    nguiformlayout

    @classinfo
    A horizontal slider group consisting of a label, a horizontal slider,
    and a text label which displays the current slider's numerical value
    as a printf-formatted string.
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setlefttext_s",    'SLFT', n_setlefttext);
    cl->AddCmd("v_setrighttext_s",   'SRGT', n_setrighttext);
    cl->AddCmd("v_setminvalue_f",    'SMIN', n_setminvalue);
    cl->AddCmd("v_setmaxvalue_f",    'SMAX', n_setmaxvalue);
    cl->AddCmd("v_setvalue_f",       'SVAL', n_setvalue);
    cl->AddCmd("f_getvalue_v",       'GVAL', n_getvalue);
    cl->AddCmd("v_setleftwidth_f",   'SLWD', n_setleftwidth);
    cl->AddCmd("v_setrightwidth_f",  'SFWD', n_setrightwidth);
    cl->AddCmd("v_setlabelfont_s",   'SFON', n_setlabelfont);
    cl->AddCmd("v_setknobsize_f",    'SKSZ', n_setknobsize);
    cl->EndCmds();
}

//-----------------------------------------------------------------------------
/**
    @cmd
    setlefttext
    @input
    s(a printf formatted string)
    @output
    v
    @info
    Set text to left of slider.
*/
static void
n_setlefttext(void* slf, nCmd* cmd)
{
    nGuiHoriSliderGroup* self = (nGuiHoriSliderGroup*) slf;
    self->SetLeftText(cmd->In()->GetS());
}
//-----------------------------------------------------------------------------
/**
    @cmd
    setrighttext
    @input
    s(a printf formatted string)
    @output
    v
    @info
    Set text to right of slider.
*/
static void
n_setrighttext(void* slf, nCmd* cmd)
{
    nGuiHoriSliderGroup* self = (nGuiHoriSliderGroup*) slf;
    self->SetRightText(cmd->In()->GetS());
}
//-----------------------------------------------------------------------------
/**
    @cmd
    setminvalue
    @input
    f
    @output
    v
    @info
    Sets the slider's minimum value.
*/
static void
n_setminvalue(void* slf, nCmd* cmd)
{
    nGuiHoriSliderGroup* self = (nGuiHoriSliderGroup*) slf;
    self->SetMinValue(cmd->In()->GetF());
}
//-----------------------------------------------------------------------------
/**
    @cmd
    setmaxvalue
    @input
    f
    @output
    v
    @info
    Sets the slider's maximum value.
*/
static void
n_setmaxvalue(void* slf, nCmd* cmd)
{
    nGuiHoriSliderGroup* self = (nGuiHoriSliderGroup*) slf;
    self->SetMaxValue(cmd->In()->GetF());
}
//-----------------------------------------------------------------------------
/**
    @cmd
    setvalue
    @input
    f
    @output
    v
    @info
    Sets the slider's current value.
*/
static void
n_setvalue(void* slf, nCmd* cmd)
{
    nGuiHoriSliderGroup* self = (nGuiHoriSliderGroup*) slf;
    self->SetValue(cmd->In()->GetF());
}
//-----------------------------------------------------------------------------
/**
    @cmd
    getvalue
    @input
    v
    @output
    f
    @info
    Gets the slider's current value.
*/
static void
n_getvalue(void* slf, nCmd* cmd)
{
    nGuiHoriSliderGroup* self = (nGuiHoriSliderGroup*) slf;
    cmd->Out()->SetF(self->GetValue());
}
//-----------------------------------------------------------------------------
/**
    @cmd
    setleftwidth
    @input
    f
    @output
    v
    @info
    Sets relative width of left text label.
*/
static void
n_setleftwidth(void* slf, nCmd* cmd)
{
    nGuiHoriSliderGroup* self = (nGuiHoriSliderGroup*) slf;
    self->SetLeftWidth(cmd->In()->GetF());
}
//-----------------------------------------------------------------------------
/**
    @cmd
    setrightwidth
    @input
    f
    @output
    v
    @info
    Sets relative width of right text label.
*/
static void
n_setrightwidth(void* slf, nCmd* cmd)
{
    nGuiHoriSliderGroup* self = (nGuiHoriSliderGroup*) slf;
    self->SetRightWidth(cmd->In()->GetF());
}
//-----------------------------------------------------------------------------
/**
    @cmd
    setlabelfont
    @input
    f
    @output
    v
    @info
    Sets the font for the left & right labels.
*/
static void
n_setlabelfont(void* slf, nCmd* cmd)
{
    nGuiHoriSliderGroup* self = (nGuiHoriSliderGroup*) slf;
    self->SetLabelFont(cmd->In()->GetS());
}
//-----------------------------------------------------------------------------
/**
    @cmd
    setknobsize
    @input
    f
    @output
    v
    @info
    Sets the width of the thumb/drag button.
*/
static void
n_setknobsize(void* slf, nCmd* cmd)
{
    nGuiHoriSliderGroup* self = (nGuiHoriSliderGroup*) slf;
    self->SetKnobSize(cmd->In()->GetF());
}
