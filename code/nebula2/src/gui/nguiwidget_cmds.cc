//-----------------------------------------------------------------------------
//  nguiwidget_cmds.cc
//  (C) 2003 RadonLabs GmbH
//-----------------------------------------------------------------------------
#include "gui/nguiwidget.h"
#include "gui/nguiserver.h"
#include "kernel/npersistserver.h"

static void n_show(void* slf, nCmd* cmd);
static void n_hide(void* slf, nCmd* cmd);
static void n_isshown(void* slf, nCmd* cmd);
static void n_enable(void* slf, nCmd* cmd);
static void n_disable(void* slf, nCmd* cmd);
static void n_isenabled(void* slf, nCmd* cmd);
static void n_setcommand(void* slf, nCmd* cmd);
static void n_getcommand(void* slf, nCmd* cmd);
static void n_setshowcommand(void* slf, nCmd* cmd);
static void n_getshowcommand(void* slf, nCmd* cmd);
static void n_sethidecommand(void* slf, nCmd* cmd);
static void n_gethidecommand(void* slf, nCmd* cmd);
static void n_setframecommand(void* slf, nCmd* cmd);
static void n_getframecommand(void* slf, nCmd* cmd);
static void n_setbuttondowncommand(void* slf, nCmd* cmd);
static void n_getbuttondowncommand(void* slf, nCmd* cmd);
static void n_setrect(void* slf, nCmd* cmd);
static void n_getrect(void* slf, nCmd* cmd);
static void n_settooltip(void* slf, nCmd* cmd);
static void n_gettooltip(void* slf, nCmd* cmd);
static void n_setblinking(void* slf, nCmd* cmd);
static void n_setblinkrate(void* slf, nCmd* cmd);
static void n_getblinking(void* slf, nCmd* cmd);
static void n_setdefaultbrush(void* slf, nCmd* cmd);
static void n_getdefaultbrush(void* slf, nCmd* cmd);
static void n_setpressedbrush(void* slf, nCmd* cmd);
static void n_getpressedbrush(void* slf, nCmd* cmd);
static void n_sethighlightbrush(void* slf, nCmd* cmd);
static void n_gethighlightbrush(void* slf, nCmd* cmd);
static void n_setdisabledbrush(void* slf, nCmd* cmd);
static void n_getdisabledbrush(void* slf, nCmd* cmd);
static void n_hasfocus(void* slf, nCmd* cmd);

//-----------------------------------------------------------------------------
/**
    @scriptclass
    nguiwidget

    @cppclass nGuiWidget

    @superclass
    nroot

    @classinfo
    Ancestor of all gui widgets.
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_show_v",                      'SHOW', n_show);
    cl->AddCmd("v_hide_v",                      'HIDE', n_hide);
    cl->AddCmd("b_isshown_v",                   'ISSH', n_isshown);
    cl->AddCmd("v_enable_v",                    'ENAB', n_enable);
    cl->AddCmd("v_disable_v",                   'DISB', n_disable);
    cl->AddCmd("b_isenabled_v",                 'ISEB', n_isenabled);
    cl->AddCmd("v_setcommand_s",                'STCM', n_setcommand);
    cl->AddCmd("s_getcommand_v",                'GTCM', n_getcommand);
    cl->AddCmd("v_setshowcommand_s",            'SSHC', n_setshowcommand);
    cl->AddCmd("s_getshowcommand_v",            'GSHC', n_getshowcommand);
    cl->AddCmd("v_sethidecommand_s",            'SHDC', n_sethidecommand);
    cl->AddCmd("s_gethidecommand_v",            'GHDC', n_gethidecommand);
    cl->AddCmd("v_setframecommand_s",           'SFRC', n_setframecommand);
    cl->AddCmd("s_getframecommand_v",           'GFRC', n_getframecommand);
    cl->AddCmd("v_setbuttondowncommand_s",      'SBDC', n_setbuttondowncommand);
    cl->AddCmd("s_getbuttondowncommand_v",      'GBDC', n_getbuttondowncommand);
    cl->AddCmd("v_setrect_ffff",                'STPS', n_setrect);
    cl->AddCmd("ffff_getrect_v",                'GTPS', n_getrect);
    cl->AddCmd("v_settooltip_s",                'STTP', n_settooltip);
    cl->AddCmd("s_gettooltip_v",                'GTTP', n_gettooltip);
    cl->AddCmd("v_setblinking_bf",              'SBLK', n_setblinking);
    cl->AddCmd("v_setblinkrate_f",              'SBLR', n_setblinkrate);
    cl->AddCmd("b_getblinking_v",               'GBLK', n_getblinking);
    cl->AddCmd("v_setdefaultbrush_s",           'SDFB', n_setdefaultbrush);
    cl->AddCmd("s_getdefaultbrush_v",           'GDFB', n_getdefaultbrush);
    cl->AddCmd("v_setpressedbrush_s",           'SPRB', n_setpressedbrush);
    cl->AddCmd("s_getpressedbrush_v",           'GPRB', n_getpressedbrush);
    cl->AddCmd("v_sethighlightbrush_s",         'SHLB', n_sethighlightbrush);
    cl->AddCmd("s_gethighlightbrush_v",         'GHLB', n_gethighlightbrush);
    cl->AddCmd("v_setdisabledbrush_s",          'SDSB', n_setdisabledbrush);
    cl->AddCmd("s_getdisabledbrush_v",          'GDSB', n_getdisabledbrush);
    cl->AddCmd("b_hasfocus_v",                  'HASF', n_hasfocus);
    cl->EndCmds();
}

//-----------------------------------------------------------------------------
/**
    @cmd
    show
    @input
    v
    @output
    v
    @info
    Make the widget visible.
*/
static void
n_show(void* slf, nCmd* /*cmd*/)
{
    nGuiWidget* self = (nGuiWidget*) slf;
    self->Show();
}

//-----------------------------------------------------------------------------
/**
    @cmd
    hide
    @input
    v
    @output
    v
    @info
    Make the widget invisible.
*/
static void
n_hide(void* slf, nCmd* /*cmd*/)
{
    nGuiWidget* self = (nGuiWidget*) slf;
    self->Hide();
}

//-----------------------------------------------------------------------------
/**
    @cmd
    isshown
    @input
    v
    @output
    b(ShownFlag)
    @info
    Return true if widget is currently shown.
*/
static void
n_isshown(void* slf, nCmd* cmd)
{
    nGuiWidget* self = (nGuiWidget*) slf;
    cmd->Out()->SetB(self->IsShown());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    enable
    @input
    v
    @output
    v
    @info
    Enable the widget for user interaction.
*/
static void
n_enable(void* slf, nCmd* /*cmd*/)
{
    nGuiWidget* self = (nGuiWidget*) slf;
    self->Enable();
}

//-----------------------------------------------------------------------------
/**
    @cmd
    disable
    @input
    v
    @output
    v
    @info
    Disable the widget.
*/
static void
n_disable(void* slf, nCmd* /*cmd*/)
{
    nGuiWidget* self = (nGuiWidget*) slf;
    self->Disable();
}

//-----------------------------------------------------------------------------
/**
    @cmd
    isenabled
    @input
    v
    @output
    b(EnabledFlag)
    @info
    Return true if widget is currently enabled.
*/
static void
n_isenabled(void* slf, nCmd* cmd)
{
    nGuiWidget* self = (nGuiWidget*) slf;
    cmd->Out()->SetB(self->IsEnabled());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    setcommand
    @input
    s(Command)
    @output
    v
    @info
    Set command to be executed as the widget's action.
*/
static void 
n_setcommand(void* slf, nCmd* cmd)
{
    nGuiWidget* self = (nGuiWidget*) slf;
    self->SetCommand(cmd->In()->GetS());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    getcommand
    @input
    v
    @output
    s(Command)
    @info
    Get command to be executed as the widget's action.
*/
static void 
n_getcommand(void* slf, nCmd* cmd)
{
    nGuiWidget* self = (nGuiWidget*) slf;
    cmd->Out()->SetS(self->GetCommand());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    setshowcommand
    @input
    s(ShowCommand)
    @output
    v
    @info
    Set command to be executed when widget is shown.
*/
static void 
n_setshowcommand(void* slf, nCmd* cmd)
{
    nGuiWidget* self = (nGuiWidget*) slf;
    self->SetShowCommand(cmd->In()->GetS());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    getshowcommand
    @input
    v
    @output
    s(ShowCommand)
    @info
    Get command to be executed when widget is shown.
*/
static void 
n_getshowcommand(void* slf, nCmd* cmd)
{
    nGuiWidget* self = (nGuiWidget*) slf;
    cmd->Out()->SetS(self->GetShowCommand());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    sethidecommand
    @input
    s(HideCommand)
    @output
    v
    @info
    Set command to be executed when widget is hidden.
*/
static void 
n_sethidecommand(void* slf, nCmd* cmd)
{
    nGuiWidget* self = (nGuiWidget*) slf;
    self->SetHideCommand(cmd->In()->GetS());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    gethidecommand
    @input
    v
    @output
    s(HideCommand)
    @info
    Get command to be executed when widget is hidden.
*/
static void 
n_gethidecommand(void* slf, nCmd* cmd)
{
    nGuiWidget* self = (nGuiWidget*) slf;
    cmd->Out()->SetS(self->GetHideCommand());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    setframecommand
    @input
    s(FrameCommand)
    @output
    v
    @info
    Set command to be executed per frame while widget is visible.
*/
static void 
n_setframecommand(void* slf, nCmd* cmd)
{
    nGuiWidget* self = (nGuiWidget*) slf;
    self->SetFrameCommand(cmd->In()->GetS());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    getframecommand
    @input
    v
    @output
    s(FrameCommand)
    @info
    Get command to be executed per frame while widget is visible.
*/
static void 
n_getframecommand(void* slf, nCmd* cmd)
{
    nGuiWidget* self = (nGuiWidget*) slf;
    cmd->Out()->SetS(self->GetFrameCommand());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    setbuttondowncommand
    @input
    s
    @output
    v
    @info
    Set optional command to be executed on button down.
*/
static void 
n_setbuttondowncommand(void* slf, nCmd* cmd)
{
    nGuiWidget* self = (nGuiWidget*) slf;
    self->SetButtonDownCommand(cmd->In()->GetS());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    getbuttondowncommand
    @input
    v
    @output
    s
    @info
    Get optional command to be executed on button down.
*/
static void 
n_getbuttondowncommand(void* slf, nCmd* cmd)
{
    nGuiWidget* self = (nGuiWidget*) slf;
    cmd->Out()->SetS(self->GetButtonDownCommand());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    setrect
    @input
    f(x0), f(x1), f(y0), f(y1)
    @output
    v
    @info
    Set screen space rectangle.
*/
static void
n_setrect(void* slf, nCmd* cmd)
{
    nGuiWidget* self = (nGuiWidget*) slf;
    rectangle r;
    r.v0.x = cmd->In()->GetF();
    r.v1.x = cmd->In()->GetF();
    r.v0.y = cmd->In()->GetF();
    r.v1.y = cmd->In()->GetF();
    self->SetRect(r);
}

//-----------------------------------------------------------------------------
/**
    @cmd
    getrect
    @input
    v
    @output
    f(x0), f(x1), f(y0), f(y1)
    @info
    Get screen space rectangle.
*/
static void
n_getrect(void* slf, nCmd* cmd)
{
    nGuiWidget* self = (nGuiWidget*) slf;
    const rectangle& r = self->GetRect();
    cmd->Out()->SetF(r.v0.x);
    cmd->Out()->SetF(r.v1.x);
    cmd->Out()->SetF(r.v0.y);
    cmd->Out()->SetF(r.v1.y);
}

//-----------------------------------------------------------------------------
/**
    @cmd
    settooltip
    @input
    s(TooltipString)
    @output
    v
    @info
    Set an optional tooltip string.
*/
static void
n_settooltip(void* slf, nCmd* cmd)
{
    nGuiWidget* self = (nGuiWidget*) slf;
    self->SetTooltip(cmd->In()->GetS());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    gettooltip
    @input
    v
    @output
    s(TooltipString)
    @info
    Get tooltip string.
*/
static void
n_gettooltip(void* slf, nCmd* cmd)
{
    nGuiWidget* self = (nGuiWidget*) slf;
    cmd->Out()->SetS(self->GetTooltip());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    setblinking
    @input
    b(BlinkingState), f(TimeOut)
    @output
    v
    @info
    Set blinking state of widget (only implemented by some widget classes).
    If blinking is turned on, it will turn off automatically after TimeOut
    time has elapsed. Set TimeOut to 0 for infinite blinking.
*/
static void
n_setblinking(void* slf, nCmd* cmd)
{
    nGuiWidget* self = (nGuiWidget*) slf;
    bool b = cmd->In()->GetB();
    float t = cmd->In()->GetF();
    self->SetBlinking(b, t);
}

//-----------------------------------------------------------------------------
/**
    @cmd
    setblinkrate
    @input
    f(BlinkRate)
    @output
    v
    @info
    Sets the number of seconds for a complete blink cycle (on & off once).
    Only implemented by some widget classes.
*/
static void
n_setblinkrate(void* slf, nCmd* cmd)
{
    nGuiWidget* self = (nGuiWidget*) slf;
    self->SetBlinkRate(cmd->In()->GetF());
}
//-----------------------------------------------------------------------------
/**
    @cmd
    getblinking
    @input
    v
    @output
    b(BlinkingState)
    @info
    Get blinking state of widget (only implemented by some widget classes).
*/
static void
n_getblinking(void* slf, nCmd* cmd)
{
    nGuiWidget* self = (nGuiWidget*) slf;
    cmd->Out()->SetB(self->GetBlinking());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    setdefaultbrush
    @input
    s(DefaultBrushName)
    @output
    v
    @info
    Set default brush name.
*/
static void
n_setdefaultbrush(void* slf, nCmd* cmd)
{
    nGuiWidget* self = (nGuiWidget*) slf;
    self->SetDefaultBrush(cmd->In()->GetS());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    getdefaultbrush
    @input
    v
    @output
    s(DefaultBrushName)
    @info
    Get default brush name.
*/
static void
n_getdefaultbrush(void* slf, nCmd* cmd)
{
    nGuiWidget* self = (nGuiWidget*) slf;
    cmd->Out()->SetS(self->GetDefaultBrush());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    setpressedbrush
    @input
    s(PressedBrushName)
    @output
    v
    @info
    Set pressed brush name.
*/
static void
n_setpressedbrush(void* slf, nCmd* cmd)
{
    nGuiWidget* self = (nGuiWidget*) slf;
    self->SetPressedBrush(cmd->In()->GetS());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    getpressedbrush
    @input
    v
    @output
    s(PressedBrushName)
    @info
    Get pressed brush name.
*/
static void
n_getpressedbrush(void* slf, nCmd* cmd)
{
    nGuiWidget* self = (nGuiWidget*) slf;
    cmd->Out()->SetS(self->GetPressedBrush());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    sethighlightbrush
    @input
    s(HighlightBrushName)
    @output
    v
    @info
    Set pressed brush name.
*/
static void
n_sethighlightbrush(void* slf, nCmd* cmd)
{
    nGuiWidget* self = (nGuiWidget*) slf;
    self->SetHighlightBrush(cmd->In()->GetS());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    gethighlightbrush
    @input
    v
    @output
    s(HighlightBrushName)
    @info
    Get highlight brush name.
*/
static void
n_gethighlightbrush(void* slf, nCmd* cmd)
{
    nGuiWidget* self = (nGuiWidget*) slf;
    cmd->Out()->SetS(self->GetHighlightBrush());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    setdisabledbrush
    @input
    s(DisabledBrushName)
    @output
    v
    @info
    Set brush for rendering in disabled state.
*/
static void
n_setdisabledbrush(void* slf, nCmd* cmd)
{
    nGuiWidget* self = (nGuiWidget*) slf;
    self->SetDisabledBrush(cmd->In()->GetS());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    getdisabledbrush
    @input
    v
    @output
    s(DisabledBrushName)
    @info
    Get disabled brush name.
*/
static void
n_getdisabledbrush(void* slf, nCmd* cmd)
{
    nGuiWidget* self = (nGuiWidget*) slf;
    cmd->Out()->SetS(self->GetDisabledBrush());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    hasfocus
    @input
    v
    @output
    b(Focus)
    @info
    Return true if widget has focus.
*/
static void
n_hasfocus(void* slf, nCmd* cmd)
{
    nGuiWidget* self = (nGuiWidget*) slf;
    cmd->Out()->SetB(self->HasFocus());
}

//-----------------------------------------------------------------------------
/**
    05-Oct-04    kims    created
*/
bool
nGuiWidget::SaveCmds(nPersistServer* ps)
{
    if (nRoot::SaveCmds(ps))
    {
        nCmd* cmd;
        nString str;

        //--- hide ---
        if (!this->IsShown())
        {
            cmd = ps->GetCmd(this, 'HIDE');
            ps->PutCmd(cmd);
        }

        //--- disable ---
        if (!this->IsEnabled())
        {
            cmd = ps->GetCmd(this, 'DISB');
            ps->PutCmd(cmd);
        }

        //--- setcommand ---
        str = this->GetCommand();
        if (!str.IsEmpty())
        {
            cmd = ps->GetCmd(this, 'STCM');
            cmd->In()->SetS(this->GetCommand());
            ps->PutCmd(cmd);
        }

        //--- setshowcommand ---
        str = this->GetShowCommand();
        if (!str.IsEmpty())
        {
            cmd = ps->GetCmd(this, 'SSHC');
            cmd->In()->SetS(this->GetShowCommand());
            ps->PutCmd(cmd);
        }

        //--- sethidecommand ---
        str = this->GetHideCommand();
        if (!str.IsEmpty())
        {
            cmd = ps->GetCmd(this, 'SHDC');
            cmd->In()->SetS(this->GetHideCommand());
            ps->PutCmd(cmd);
        }

        //--- setframecommand ---
        str = this->GetFrameCommand();
        if (!str.IsEmpty())
        {
            cmd = ps->GetCmd(this, 'SFRC');
            cmd->In()->SetS(this->GetFrameCommand());
            ps->PutCmd(cmd);
        }

        //--- setbuttondowdncommand ---
        str = this->GetButtonDownCommand();
        if (!str.IsEmpty())
        {
            cmd = ps->GetCmd(this, 'SBDC');
            cmd->In()->SetS(this->GetButtonDownCommand());
            ps->PutCmd(cmd);
        }

        //--- settooltip ---
        if (this->GetTooltip())
        {
            cmd = ps->GetCmd(this, 'STTP');
            cmd->In()->SetS(this->GetTooltip());
            ps->PutCmd(cmd);
        }

        //--- setblinking ---
        if (this->GetBlinking())
        {
            cmd = ps->GetCmd(this, 'SBLK');
            cmd->In()->SetB(this->GetBlinking());
            cmd->In()->SetF((float)this->blinkTimeOut);
            ps->PutCmd(cmd);

            cmd = ps->GetCmd(this, 'SBLR');
            cmd->In()->SetF((float)this->blinkRate);
            ps->PutCmd(cmd);
        }

        //--- setdefaultbrush ---
        if (this->GetDefaultBrush())
        {
            cmd = ps->GetCmd(this, 'SDFB');
            cmd->In()->SetS(this->GetDefaultBrush());
            ps->PutCmd(cmd);
        }

        //--- setpressedbrush ---
        if (this->GetPressedBrush())
        {
            cmd = ps->GetCmd(this, 'SPRB');
            cmd->In()->SetS(this->GetPressedBrush());
            ps->PutCmd(cmd);
        }

        //--- sethighlightbrush ---
        if (this->GetHighlightBrush())
        {
            cmd = ps->GetCmd(this, 'SHLB');
            cmd->In()->SetS(this->GetHighlightBrush());
            ps->PutCmd(cmd);
        }

        //--- setdisabledbrush ---
        if (this->GetDisabledBrush())
        {
            cmd = ps->GetCmd(this, 'SDSB');
            cmd->In()->SetS(this->GetDisabledBrush());
            ps->PutCmd(cmd);
        }

        //--- setrect ---
        rectangle rc = this->GetRect();
        cmd = ps->GetCmd(this, 'STPS');
        cmd->In()->SetF(rc.v0.x);
        cmd->In()->SetF(rc.v1.x);
        cmd->In()->SetF(rc.v0.y);
        cmd->In()->SetF(rc.v1.y);
        ps->PutCmd(cmd);
 
        return true;
    }

    return false;
}

