//-------------------------------------------------------------------
//  input/ninput_dispatch.cc
//  This file was machine generated.
//  (C) 2000 A.Weissflog/A.Flemming
//-------------------------------------------------------------------
#include "input/ninputserver.h"

static void n_startlogging(void *, nCmd *);
static void n_stoplogging(void *, nCmd *);
static void n_islogging(void *, nCmd *);
static void n_beginmap(void *, nCmd *);
static void n_map(void *, nCmd *);
static void n_endmap(void *, nCmd *);
static void n_getslider(void *, nCmd *);
static void n_getbutton(void *, nCmd *);
static void n_setlongpressedtime(void *, nCmd *);
static void n_getlongpressedtime(void *, nCmd *);
static void n_setdoubleclicktime(void *, nCmd *);
static void n_getdoubleclicktime(void *, nCmd *);
static void n_setmousefactor(void*, nCmd*);
static void n_getmousefactor(void*, nCmd*);
static void n_setmouseinvert(void*, nCmd*);
static void n_getmouseinvert(void*, nCmd*);
static void n_getmousepos(void*, nCmd*);

//-------------------------------------------------------------------
/**
    @scriptclass
    ninputserver

    @cppclass
    nInputServer

    @superclass
    nroot

    @classinfo
    The inputserver lives under /sys/servers/input and
    provides the global list of input events for you. The
    server does not in any case generate the input events
    by itself. The nGfxServer for example feeds the inputserver
    with key and mouse events. The inputsever has to be created
    after the gfxserver, because the gfxserver provides a windows
    handle which is needed by the inputserver(at least under win32
    for direct input). In /sys/share/input you can lookup which
    input devices the server has recognized and which channels a
    device provides.
*/
void n_initcmds(nClass *cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_startlogging_v",          'STLG', n_startlogging);
    cl->AddCmd("v_stoplogging_v",           'SPLG', n_stoplogging);
    cl->AddCmd("b_islogging_v",             'ISLG', n_islogging);
    cl->AddCmd("v_beginmap_v",              'BGMP', n_beginmap);
    cl->AddCmd("b_map_ss",                  'MAP_', n_map);
    cl->AddCmd("v_endmap_v",                'EDMP', n_endmap);
    cl->AddCmd("f_getslider_s",             'GSLD', n_getslider);
    cl->AddCmd("b_getbutton_s",             'GBTN', n_getbutton);
    cl->AddCmd("v_setlongpressedtime_f",    'SLPT', n_setlongpressedtime);
    cl->AddCmd("f_getlongpressedtime_v",    'GLPT', n_getlongpressedtime);
    cl->AddCmd("v_setdoubleclicktime_f",    'SDCT', n_setdoubleclicktime);
    cl->AddCmd("f_getdoubleclicktime_v",    'GDCT', n_getdoubleclicktime);
    cl->AddCmd("v_setmousefactor_f",        'SMSF', n_setmousefactor);
    cl->AddCmd("f_getmousefactor_v",        'GMSF', n_getmousefactor);
    cl->AddCmd("v_setmouseinvert_b",        'SMIV', n_setmouseinvert);
    cl->AddCmd("b_getmouseinvert_v",        'GMIV', n_getmouseinvert);
    cl->AddCmd("ff_getmousepos_v",          'GMXY', n_getmousepos);
    cl->EndCmds();
}

//-------------------------------------------------------------------
/**
    @cmd
    startlogging
    @input
    v
    @output
    v
    @info
    Starts input logging to stdout.
*/
static void n_startlogging(void *o, nCmd *)
{
    nInputServer *self = (nInputServer *) o;
    self->StartLogging();
}

//-------------------------------------------------------------------
/**
    @cmd
    stoplogging
    @input
    v
    @output
    v
    @info
    Stops input logging.
*/
static void n_stoplogging(void *o, nCmd *)
{
    nInputServer *self = (nInputServer *) o;
    self->StopLogging();
}

//-------------------------------------------------------------------
/**
    @cmd
    islogging
    @input
    v
    @output
    b (Logging)
    @info
    Returns the logging status.
*/
static void n_islogging(void *o, nCmd *cmd)
{
    nInputServer *self = (nInputServer *) o;
    cmd->Out()->SetB(self->IsLogging());
}

//-------------------------------------------------------------------
/**
    @cmd
    beginmap
    @input
    v
    @output
    v
    @info
    Starts a block of mapping definitions. All earlier defined
    mappings are lost!
*/
static void n_beginmap(void *o, nCmd *)
{
    nInputServer *self = (nInputServer *) o;
    self->BeginMap();
}

//-------------------------------------------------------------------
/**
    @cmd
    map
    @input
    s (EventName), s (StateName)
    @output
    b (Success)
    @info
    Maps a input event to a input state. The 'EventName'
    defines an input event which controls the via 'StateName'
    given input state. The definition of a input event consists
    of a device ID, channel ID and channel modifier (not on axes):
    dev:channel[.up|down|pressed|long|double]

    For example:
     - keyb0:b.pressed - Key 'B' on Keyboard 0 pressed
     - keyb0:a.down - Key 'A' on Keyboard 0 pressed down
     - joy0:b0.up - Button 0 on Joystick 0 released
     - joy1:b1.double - Button 1 on Joystick 1 doubble clicked
     - joy0:b2.long - Button 2 on Joystick 2 pressed very long
     - mouse0:-x - Mouse 0 moved to the left

    A list of the accepted devices can be found under
    '/sys/share/input/devs', the list of the channels provided
    by the device can be found in the 'channels' subdirectory
    of each device.

    The list of the actual states is under '/sys/share/input/states'.
    Some examples for a complete mapping:
     - .map joy0:-x move_left
     - .map joy0:+x move_right
     - .map joy0:-y move_down
     - .map joy0:+y move_up
     - .map keyb0:f1.down "script:incr x"
     - .map keyb0:f2.down "script:newv sammler"
*/
static void n_map(void *o, nCmd *cmd)
{
    nInputServer *self = (nInputServer *) o;
    const char *s0 = cmd->In()->GetS();
    const char *s1 = cmd->In()->GetS();
    cmd->Out()->SetB(self->Map(s0,s1));
}

//-------------------------------------------------------------------
/**
    @cmd
    endmap
    @input
    v
    @output
    v
    @info
    Closes a block of mapping definitions.
*/
static void n_endmap(void *o, nCmd *)
{
    nInputServer *self = (nInputServer *) o;
    self->EndMap();
}

//-------------------------------------------------------------------
/**
    @cmd
    getslider
    @input
    s (InputState)
    @output
    f (Value)
    @info
    Returns the state of the input state
    as an analog slider value.
*/
static void n_getslider(void *o, nCmd *cmd)
{
    nInputServer *self = (nInputServer *) o;
    cmd->Out()->SetF(self->GetSlider(cmd->In()->GetS()));
}

//-------------------------------------------------------------------
/**
    @cmd
    getbutton
    @input
    s (InputState)
    @output
    b (ButtonPressed)
    @info
    Returns the state of the input state as a button
    Depending on the input mapping the routine returns
    true if:
    - the button is pressed(.pressed)
    - the button is down(.down)
    - the button is released(.up)
    - the button is pressed for a longer time(.long)
    - the button is double clicked(.double)
*/
static void n_getbutton(void *o, nCmd *cmd)
{
    nInputServer *self = (nInputServer *) o;
    cmd->Out()->SetB(self->GetButton(cmd->In()->GetS()));
}

//-------------------------------------------------------------------
/**
    @cmd
    setlongpressedtime
    @input
    f (LongPressedTime)
    @output
    v
    @info
    Sets the time it takes to trigger a long pressed event
    of an input mapping.
*/
static void n_setlongpressedtime(void *o, nCmd *cmd)
{
    nInputServer *self = (nInputServer *) o;
    self->SetLongPressedTime(cmd->In()->GetF());
}

//-------------------------------------------------------------------
/**
    @cmd
    getlongpressedtime
    @input
    v
    @output
    f (LongPressedTime)
    @info
    Returns the via 'setlongpressedtime' set value.
*/
static void n_getlongpressedtime(void *o, nCmd *cmd)
{
    nInputServer *self = (nInputServer *) o;
    cmd->Out()->SetF(self->GetLongPressedTime());
}

//-------------------------------------------------------------------
/**
    @cmd
    setdoubleclicktime
    @input
    f (DoubleClickTime)
    @output
    v
    @info
    Sets the time interval for a double click event.
*/
static void n_setdoubleclicktime(void *o, nCmd *cmd)
{
    nInputServer *self = (nInputServer *) o;
    self->SetDoubleClickTime(cmd->In()->GetF());
}

//-------------------------------------------------------------------
/**
    @cmd
    getdoubleclicktime
    @input
    v
    @output
    f (DoubleClickTime)
    @info
    Returns the time interval of the double click event,
    set via 'setdoubleclicktime'.
*/
static void n_getdoubleclicktime(void *o, nCmd *cmd)
{
    nInputServer *self = (nInputServer *) o;
    cmd->Out()->SetF(self->GetDoubleClickTime());
}

//-------------------------------------------------------------------
/**
    @cmd
    setmousefactor
    @input
    f (MouseFactor)
    @output
    v
    @info
    Set the mouse input factor.
*/
static void n_setmousefactor(void* slf, nCmd* cmd)
{
    nInputServer* self = (nInputServer*) slf;
    self->SetMouseFactor(cmd->In()->GetF());
}

//-------------------------------------------------------------------
/**
    @cmd
    getmousefactor
    @input
    v
    @output
    f (MouseFactor)
    @info
    Get the mouse input factor.
*/
static void n_getmousefactor(void* slf, nCmd* cmd)
{
    nInputServer* self = (nInputServer*) slf;
    cmd->Out()->SetF(self->GetMouseFactor());
}

//-------------------------------------------------------------------
/**
    @cmd
    setmouseinvert
    @input
    b (MouseInvert)
    @output
    v
    @info
    Set the mouse invert flag.
*/
static void n_setmouseinvert(void* slf, nCmd* cmd)
{
    nInputServer* self = (nInputServer*) slf;
    self->SetMouseInvert(cmd->In()->GetB());
}

//-------------------------------------------------------------------
/**
    @cmd
    getmouseinvert
    @input
    v
    @output
    b (MouseInvert)
    @info
    Get the mouse invert flag.
*/
static void n_getmouseinvert(void* slf, nCmd* cmd)
{
    nInputServer* self = (nInputServer*) slf;
    cmd->Out()->SetB(self->GetMouseInvert());
}

//-------------------------------------------------------------------
/**
    @cmd
    getmousepos
    @input
    v
    @output
    ff (x,y coords)
    @info
    Get the current mouse position.
    (0,0) is the upper left hand corner,
    (1,1) is the lower right.
*/
static void n_getmousepos(void* slf, nCmd* cmd)
{
    nInputServer* self = (nInputServer*) slf;
    const vector2& pos = self->GetMousePos();
    cmd->Out()->SetF(pos.x);
    cmd->Out()->SetF(pos.y);
}
//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
