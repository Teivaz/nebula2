//------------------------------------------------------------------------------
//  (C) 2004 Rafael Van Daele-Hunt
//------------------------------------------------------------------------------
#include "BombsquadBruce/bbmobileactor.h"
#include "kernel/npersistserver.h"

// set character attributes
static void n_setmaxspd(void* slf, nCmd* cmd);
static void n_setturnspd(void* slf, nCmd* cmd);
static void n_setapparentturnspd(void* slf, nCmd* cmd);
static void n_settiltspd(void* slf, nCmd* cmd);
static void n_setcontrol(void* slf, nCmd* cmd);
// get current state
static void n_getheadingvector(void* slf, nCmd* cmd);
static void n_getcurturn(void* slf, nCmd* cmd);
static void n_getcurtilt(void* slf, nCmd* cmd);
static void n_getcurstrafe(void* slf, nCmd* cmd);
static void n_getcurfwd(void* slf, nCmd* cmd);
static void n_move(void* slf, nCmd* cmd);
// animation
static void n_setanimstate(void* slf, nCmd* cmd);
static void n_setanimspeed(void* slf, nCmd* cmd);
//------------------------------------------------------------------------------
/**
    @scriptclass
    bbmobileactor
    
    @superclass
    bbactor

    @classinfo
    This is the base class for everything that moves and is visible, so
    it contains commands to set variables that control how whatever it
    is can move.
*/
void
n_initcmds(nClass* clazz)
{
    clazz->BeginCmds();
    clazz->AddCmd("v_setmaxspd_f",          'SFWD', n_setmaxspd );
    clazz->AddCmd("v_setturnspd_f",         'STRN', n_setturnspd );
    clazz->AddCmd("v_setapparentturnspd_f", 'SATR', n_setapparentturnspd );
    clazz->AddCmd("v_settiltspd_f",         'STLT', n_settiltspd );
    clazz->AddCmd("v_setcontrol_ss",        'CTRA', n_setcontrol );
    clazz->AddCmd("fff_getheadingvector_v", 'GHVC', n_getheadingvector );
    clazz->AddCmd("f_getcurturn_v",         'GTRN', n_getcurturn );
    clazz->AddCmd("f_getcurtilt_v",         'GTLT', n_getcurtilt );
    clazz->AddCmd("f_getcurstrafe_v",       'GSTR', n_getcurstrafe );
    clazz->AddCmd("f_getcurfwd_v",          'GFWD', n_getcurfwd );
    clazz->AddCmd("v_move_fffff",           'MOVE', n_move );
    clazz->AddCmd("v_setanimstate_if",      'ANST', n_setanimstate );
    clazz->AddCmd("v_setanimspeed_f",       'ANSP', n_setanimspeed );
    clazz->EndCmds();
}

//------------------------------------------------------------------------------
/**
@cmd
setmaxspd
@input
f

@output
v

@info
The rate at which the Actor moves forward
*/
static void n_setmaxspd(void* slf, nCmd* cmd)
{
    BBMobileActor* self = (BBMobileActor*)slf;
    self->SetFwdSpd(cmd->In()->GetF());
}
//------------------------------------------------------------------------------
/**
@cmd
setturnspd
@input
f

@output
v

@info
The rate at which the Actor rotates
*/
static void n_setturnspd(void* slf, nCmd* cmd)
{
    BBMobileActor* self = (BBMobileActor*)slf;
    self->SetTurnSpd(cmd->In()->GetF());
}
//------------------------------------------------------------------------------
/**
@cmd
setapparentturnspd
@input
f

@output
v

@info
The rate at which the Actor appears to rotate when
you move in a direction other than the current 
apparent direction.
*/
static void n_setapparentturnspd(void* slf, nCmd* cmd)
{
    BBMobileActor* self = (BBMobileActor*)slf;
    self->SetApparentTurnSpd(cmd->In()->GetF());
}

//------------------------------------------------------------------------------
/**
@cmd
settiltspd
@input
f

@output
v

@info
The rate at which the actor rotates or moves vertically
*/
static void n_settiltspd(void* slf, nCmd* cmd)
{
    BBMobileActor* self = (BBMobileActor*)slf;
    self->SetTiltSpd(cmd->In()->GetF());
}

//------------------------------------------------------------------------------
/**
@cmd
setcontrol

@input
s (name of the control), s (channel to which this control should listen)

@output
v

@info
Tells a player control (e.g. SpinLeft) to listen to a certain channel (e.g. mouseleft)
*/

static
void
n_setcontrol(void* slf, nCmd* cmd)
{
    BBMobileActor* self = (BBMobileActor*)slf;
    std::string control( cmd->In()->GetS() );
    std::string channel( cmd->In()->GetS() );
    self->SetControl(control, channel);
}
//------------------------------------------------------------------------------
/**
@cmd
getheadingvector

@input
v

@output
fff

@info
Returns the player's heading
*/

static
void
n_getheadingvector(void* slf, nCmd* cmd)
{
    BBMobileActor* self = (BBMobileActor*)slf;
    const vector3& heading = self->GetCurHeadingVector();
    cmd->Out()->SetF( heading.x );
    cmd->Out()->SetF( heading.y );
    cmd->Out()->SetF( heading.z );
}
//------------------------------------------------------------------------------
/**
@cmd
getcurturn

@input
v

@output
f

@info
Returns the player's current rotation input 
(modified by the rate given by setturnspd)
*/

static
void
n_getcurturn(void* slf, nCmd* cmd)
{
    BBMobileActor* self = (BBMobileActor*)slf;
    cmd->Out()->SetF( self->GetCurTurnSpd() );
}
//------------------------------------------------------------------------------
/**
@cmd
getcurtilt

@input
v

@output
f

@info
Returns the player's current tilt (up/down) input 
(modified by the rate given by settiltspd)
*/

static
void
n_getcurtilt(void* slf, nCmd* cmd)
{
    BBMobileActor* self = (BBMobileActor*)slf;
    cmd->Out()->SetF( self->GetCurTiltSpd() );
}
//------------------------------------------------------------------------------
/**
@cmd
getcurturn

@input
v

@output
f

@info
Returns the player's current left/right motion 
(modified by the rate given by setmaxspd)
*/

static
void
n_getcurstrafe(void* slf, nCmd* cmd)
{
    BBMobileActor* self = (BBMobileActor*)slf;
    cmd->Out()->SetF( self->GetCurStrafeSpd() );
}
//------------------------------------------------------------------------------
/**
@cmd
getcurfwd

@input
v

@output
f

@info
Returns the player's current fwd/back motion 
(modified by the rate given by setmaxspd)
*/

static
void
n_getcurfwd(void* slf, nCmd* cmd)
{
    BBMobileActor* self = (BBMobileActor*)slf;
    cmd->Out()->SetF( self->GetCurSpd() );
}
//------------------------------------------------------------------------------
/**
@cmd
move

@input
v

@output
fff (velocity in cm/s) f (rotational velocity in rad/s) f (elapsed time in s)

@info
Move the avatar by the given velocity, multiplied by delta time
*/

static
void
n_move(void* slf, nCmd* cmd)
{
    BBMobileActor* self = (BBMobileActor*)slf;
    vector3 v;
    v.x = cmd->In()->GetF();
    v.y = cmd->In()->GetF();
    v.z = cmd->In()->GetF();
    const float rotation = cmd->In()->GetF();
    float dt = cmd->In()->GetF();
    self->Move( v, rotation, dt );
}
//------------------------------------------------------------------------------
/**
@cmd
setanimstate

@input
v

@output
i (state number), f (speed factor)

@info
Set the animation state of a skinned avatar.
The speed factor will be multiplied by the base animation
speed specified in the animation file.
*/

static
void
n_setanimstate(void* slf, nCmd* cmd)
{
    BBMobileActor* self = (BBMobileActor*)slf;
    int state = cmd->In()->GetI();
    self->SetAnimState( state, cmd->In()->GetF() );
}
//------------------------------------------------------------------------------
/**
@cmd
setanimspeed

@input
v

@output
f (speed factor)

@info
The speed factor will be multiplied by the base animation
speed specified in the animation file.
*/

static
void
n_setanimspeed(void* slf, nCmd* cmd)
{
    BBMobileActor* self = (BBMobileActor*)slf;
    self->SetAnimSpeed( cmd->In()->GetF() );
}
//------------------------------------------------------------------------------
/**
@param  ps          writes the nCmd object contents out to a file.
@return             success or failure
*/
bool
BBMobileActor::SaveCmds(nPersistServer* ps)
{
    if (BBActor::SaveCmds(ps))
    {
        nCmd* cmd = ps->GetCmd(this, 'SFWD');
        ps->PutCmd(cmd);
        ps->PutCmd(ps->GetCmd(this, 'SBAK'));
        ps->PutCmd(ps->GetCmd(this, 'SSTR'));
        ps->PutCmd(ps->GetCmd(this, 'STRN'));
        ps->PutCmd(ps->GetCmd(this, 'STLT'));
        ps->PutCmd(ps->GetCmd(this, 'CTRA'));
        ps->PutCmd(ps->GetCmd(this, 'ANST'));
        return true;
    }
    return false;
}

