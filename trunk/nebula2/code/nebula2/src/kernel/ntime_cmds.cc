//-------------------------------------------------------------------
//  kernel/ntime_dispatch.cc
//  This file was machine generated.
//  (C) 2000 A.Weissflog/A.Flemming
//-------------------------------------------------------------------
#include "kernel/ntimeserver.h"
#include "kernel/nkernelserver.h"
#include "kernel/ncmdproto.h"

static void n_resettime(void *, nCmd *);
static void n_stoptime(void *, nCmd *);
static void n_starttime(void *, nCmd *);
static void n_gettime(void *, nCmd *);
static void n_settime(void *, nCmd *);
static void n_lockdeltat(void *, nCmd *);
static void n_waitdeltat(void *, nCmd *);
static void n_getlockdeltat(void *, nCmd *);
static void n_getwaitdeltat(void *, nCmd *);
static void n_enableframetime(void *, nCmd *);
static void n_disableframetime(void *, nCmd *);
static void n_getframetime(void *, nCmd *);

//-------------------------------------------------------------------
/**
    @scriptclass
    ntimeserver

    @cppclass
    nTimeServer

    @superclass
    nroot

    @classinfo
    The ntimeserver object lives under the name /sys/servers/time
    and provides a central time source for Nebula. 
*/
void n_initcmds(nClass *cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_resettime_v",         'REST', n_resettime);
    cl->AddCmd("v_stoptime_v",          'STOP', n_stoptime);
    cl->AddCmd("v_starttime_v",         'STRT', n_starttime);
    cl->AddCmd("f_gettime_v",           'GETT', n_gettime);
    cl->AddCmd("v_settime_f",           'SETT', n_settime);
    cl->AddCmd("v_lockdeltat_f",        'LDLT', n_lockdeltat);
    cl->AddCmd("v_waitdeltat_f",        'WTDT', n_waitdeltat);
    cl->AddCmd("f_getlockdeltat_v",     'GLDT', n_getlockdeltat);
    cl->AddCmd("f_getwaitdeltat_v",     'GWDT', n_getwaitdeltat);
    cl->AddCmd("v_enableframetime_v",   'ENFT', n_enableframetime);
    cl->AddCmd("v_disableframetime_v",  'DSFT', n_disableframetime);
    cl->AddCmd("f_getframetime_v",      'GTFT', n_getframetime);
    cl->EndCmds();
}

//-------------------------------------------------------------------
/**
    @cmd
    resettime

    @input
    v

    @output
    v

    @info
    Reset global time to 0.0.
*/
static void n_resettime(void *o, nCmd *)
{
    nTimeServer *self = (nTimeServer *) o;
    self->ResetTime();
}

//-------------------------------------------------------------------
/**
    @cmd
    stoptime

    @input
    v

    @output
    v

    @info
    Freeze global time.
*/
static void n_stoptime(void *o, nCmd *)
{
    nTimeServer *self = (nTimeServer *) o;
    self->StopTime();
}

//-------------------------------------------------------------------
/**
    @cmd
    starttime

    @input
    v

    @output
    v

    @info
    Unfreeze global time.
*/
static void n_starttime(void *o, nCmd *)
{
    nTimeServer *self = (nTimeServer *) o;
    self->StartTime();
}

//-------------------------------------------------------------------
/**
    @cmd
    gettime

    @input
    v

    @output
    f (Time)

    @info
    Return current global time in seconds since the time server
    was created.
*/
static void n_gettime(void *o, nCmd *cmd)
{
    nTimeServer *self = (nTimeServer *) o;
    {
        float t = (float) self->GetTime();
        cmd->Out()->SetF(t);
    }
}

//-------------------------------------------------------------------
/**
    @cmd
    settime

    @input
    f (Time)

    @output
    v

    @info
    Set global time in seconds.
*/
static void n_settime(void *o, nCmd *cmd)
{
    nTimeServer *self = (nTimeServer *) o;
    self->SetTime((double)cmd->In()->GetF());
}

//-------------------------------------------------------------------
/**
    @cmd
    lockdeltat

    @input
    f (DeltaT)

    @output
    v

    @info
    If DeltaT is greater then 0.0, the frame time will be advanced
    in steps of size DeltaT. A value of 0.0 switches back to the
    proper timing behaviour. Useful to lock virtual frame rate to
    a fixed value.
*/
static void n_lockdeltat(void *o, nCmd *cmd)
{
    nTimeServer *self = (nTimeServer *) o;
    self->LockDeltaT(cmd->In()->GetF());
}

//-------------------------------------------------------------------
/**
    @cmd
    waitdeltat

    @input
    f (DeltaT)

    @output
    v

    @info
    If DeltaT is greater then 0.0, then the time server will
    actually sleep this amount of time in each frame. This is
    useful for simulating a slow machine.
*/
static void n_waitdeltat(void *o, nCmd *cmd)
{
    nTimeServer *self = (nTimeServer *) o;
    self->WaitDeltaT(cmd->In()->GetF());
}

//-------------------------------------------------------------------
/**
    @cmd
    getlockdeltat

    @input
    v

    @output
    f (DeltaT)

    @info
    Return the value set by 'lockdeltat'.
*/
static void n_getlockdeltat(void *o, nCmd *cmd)
{
    nTimeServer *self = (nTimeServer *) o;
    cmd->Out()->SetF((float)self->GetLockDeltaT());
}

//-------------------------------------------------------------------
/**
    @cmd
    getwaitdeltat

    @input
    v

    @output
    f (DeltaT)

    @info
    Return the value set by 'waitdeltat'.
*/
static void n_getwaitdeltat(void *o, nCmd *cmd)
{
    nTimeServer *self = (nTimeServer *) o;
    cmd->Out()->SetF((float)self->GetWaitDeltaT());
}

//-------------------------------------------------------------------
/**
    @cmd
    enableframetime

    @input
    v

    @output
    v

    @info
    Enable the 'frame time' mode of the 'getframetime'
    command. In this mode 'getframetime' will return exactly
    the same value each time it is called within a frame.
    Otherwise 'getframetime' returns the same value 'gettime'
    would.
*/
static void n_enableframetime(void *o, nCmd *)
{
    nTimeServer *self = (nTimeServer *) o;
    self->EnableFrameTime();
}

//-------------------------------------------------------------------
/**
    @cmd
    disableframetime

    @input
    v

    @output
    v

    @info
    Turn off frame time mode of 'getframetime'. Further
    calls to 'getframetime' will return exactly the same value
    as 'gettime'.
*/
static void n_disableframetime(void *o, nCmd *)
{
    nTimeServer *self = (nTimeServer *) o;
    self->DisableFrameTime();
}

//-------------------------------------------------------------------
/**
    @cmd
    getframetime

    @input
    v

    @output
    v

    @info
    Get the current frame time stamp. This value differs from the
    value returned by 'gettime' because 'gettime' will return
    a different value each time it is called, while 'getframetime'
    returns the same timestamp within one frame.
*/
static void n_getframetime(void *o, nCmd *cmd)
{
    nTimeServer *self = (nTimeServer *) o;
    cmd->Out()->SetF((float)self->GetFrameTime());
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
