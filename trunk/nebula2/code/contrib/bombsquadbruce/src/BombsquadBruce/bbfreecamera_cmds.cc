//------------------------------------------------------------------------------
//  (C) 2003 Rafael Van Daele-Hunt
//------------------------------------------------------------------------------
#include "BombsquadBruce/bbfreecamera.h"
#include "kernel/npersistserver.h"

static void n_setposition(void* slf, nCmd* cmd);
static void n_setorientation(void* slf, nCmd* cmd);
static void n_setspinmode(void* slf, nCmd* cmd);
static void n_setmovespeed(void* slf, nCmd* cmd);
static void n_setrotatespeed(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    bbfreecamera
    
    @superclass
    bbcamera

    @classinfo
    A BBFreeCamera keeps track of its position and orientation and 
    changes these in response to user input.
*/
void
n_initcmds(nClass* clazz)
{
    clazz->BeginCmds();
    clazz->AddCmd("v_setposition_fff", 'SPOS', n_setposition);
    clazz->AddCmd("v_setorientation_ffff", 'SORI', n_setorientation);
    clazz->AddCmd("v_setspinmode_s", 'SSPM', n_setspinmode );
    clazz->AddCmd("v_setmovespeed_f", 'SMOV', n_setmovespeed );
    clazz->AddCmd("v_setrotatespeed_f", 'SROT', n_setrotatespeed );
    clazz->EndCmds();
}


//------------------------------------------------------------------------------
/**
    @cmd
    setposition

    @input
    f, f, f

    @output
    v

    @info
    Sets camera position in worldspace.
*/
static
void
n_setposition(void* slf, nCmd* cmd)
{
    BBFreeCamera* self = (BBFreeCamera*)slf;
    float x = cmd->In()->GetF();
    float y = cmd->In()->GetF();
    float z = cmd->In()->GetF();
    self->SetPosition(x, y, z);
}
//------------------------------------------------------------------------------
/**
    @cmd
    setorientation
    @input
    f(X), f(Y), f(Z), f (w)
    @output
    v
    @info
    Set orientation of the camera (with a nonnormalized quaternion)

*/
static void
n_setorientation(void* slf, nCmd* cmd)
{
    BBFreeCamera* self = (BBFreeCamera*) slf;
    const float x = cmd->In()->GetF();
    const float y = cmd->In()->GetF();
    const float z = cmd->In()->GetF();
    const float w = cmd->In()->GetF();
    quaternion quat( x, y, z, w );
    quat.normalize();
    self->SetOrientation( quat );
}
//------------------------------------------------------------------------------
/**
@cmd
setspinmode

@input
s (the name of the mode)

@output
v

@info
Determines how the camera reacts to SPIN channels (usually mouse movement).
*/

static
void
n_setspinmode(void* slf, nCmd* cmd)
{
    BBFreeCamera* self = (BBFreeCamera*)slf;
    nString modeStr( cmd->In()->GetS() );
	if(modeStr=="NONE")
	{
        self->SetSpinMode( BBFreeCamera::NONE );
	}
    else if(modeStr=="DRAG")
	{
		self->SetSpinMode( BBFreeCamera::DRAG );
	}
	else if(modeStr=="ROTATE")
	{
		self->SetSpinMode( BBFreeCamera::ROTATE );
	}
    else 
        n_printf("Unknown free camera mode '%s': valid is 'NONE|DRAG|ROTATE'.\n",modeStr.Get());
}
//------------------------------------------------------------------------------
/**
@cmd
setmovespeed

@input
f

@output
v

@info
Sets the rate at which the camera position changes in response to MOVE_ channels.
*/

static
void
n_setmovespeed(void* slf, nCmd* cmd)
{
	BBFreeCamera* self = (BBFreeCamera*)slf;
	self->SetMoveSpeed(cmd->In()->GetF());
}
//------------------------------------------------------------------------------
/**
@cmd
setrotatespeed

@input
f

@output
v

@info
Sets the rate at which the camera rotates in response to SPIN_ channels 
(for a given mouse sensitivity)
*/

static
void
n_setrotatespeed(void* slf, nCmd* cmd)
{
	BBFreeCamera* self = (BBFreeCamera*)slf;
	self->SetRotateSpeed(cmd->In()->GetF());
}
//------------------------------------------------------------------------------
/**
    @param  ps          writes the nCmd object contents out to a file.
    @return             success or failure
*/
bool
BBFreeCamera::SaveCmds(nPersistServer* ps)
{
    if (nRoot::SaveCmds(ps))
    {
        nCmd* cmd = ps->GetCmd(this, 'SPOS');
        ps->PutCmd(cmd);
        ps->GetCmd(this, 'SORI');
        ps->PutCmd(cmd);
        ps->GetCmd(this, 'SSPM');
        ps->PutCmd(cmd);
        ps->GetCmd(this, 'SMOV');
        ps->PutCmd(cmd);
        ps->GetCmd(this, 'SROT');
        ps->PutCmd(cmd);
        return true;
    }
    return false;
}

