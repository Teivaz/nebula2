//------------------------------------------------------------------------------
//  (C) 2003 Rafael Van Daele-Hunt
//------------------------------------------------------------------------------
#include "BombsquadBruce/ccchasecamera.h"
#include "kernel/npersistserver.h"

static void n_settargetentitypath(void* slf, nCmd* cmd);
static void n_setcameraoffset(void* slf, nCmd* cmd);
static void n_setfocaloffset(void* slf, nCmd* cmd);
static void n_setspinmode(void* slf, nCmd* cmd);
static void n_setminzoomdist(void* slf, nCmd* cmd);
static void n_setmaxzoomdist(void* slf, nCmd* cmd);
static void n_setmovespeed(void* slf, nCmd* cmd);
static void n_setrotatespeed(void* slf, nCmd* cmd);
static void n_setorbitspeed(void* slf, nCmd* cmd);
static void n_setzoomspeed(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    ccchasecamera
    
    @superclass
    cccamera

    @classinfo
    A CCChaseCameras is entity-centric: it follows a given CCRoot entity
    (specified with settargetentitypath) around, always maintaining the
    same position (specified with setcameraoffst) relative to the entity,
    and looking at some point (specified with setfocaloffset) expressed 
    relative to, and probably near, the entity.
*/
void
n_initcmds(nClass* clazz)
{
    clazz->BeginCmds();
    clazz->AddCmd("v_settargetentitypath_s", 'TGTE', n_settargetentitypath);
    clazz->AddCmd("v_setcameraoffset_fff", 'SCOF', n_setcameraoffset);
    clazz->AddCmd("v_setfocaloffset_fff", 'SFOF', n_setfocaloffset);
    clazz->AddCmd("v_setspinmode_s", 'SSPM', n_setspinmode );
    clazz->AddCmd("v_setminzoomdist_f", 'MINZ', n_setminzoomdist );
    clazz->AddCmd("v_setmaxzoomdist_f", 'MAXZ', n_setmaxzoomdist );
    clazz->AddCmd("v_setmovespeed_f", 'SMOV', n_setmovespeed );
    clazz->AddCmd("v_setrotatespeed_f", 'SROT', n_setrotatespeed );
    clazz->AddCmd("v_setorbitspeed_f", 'SORB', n_setorbitspeed );
    clazz->AddCmd("v_setzoomspeed_f", 'STZM', n_setzoomspeed );
    clazz->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    settargetentitypath

    @input
    s (CCRoot path)

    @output
    v

    @info
    Sets the target entity to track.
*/
static
void
n_settargetentitypath(void* slf, nCmd* cmd)
{
    CCChaseCamera* self = (CCChaseCamera*)slf;
    self->SetTargetEntityPath(cmd->In()->GetS());
}


//------------------------------------------------------------------------------
/**
    @cmd
    setcameraoffset

    @input
    f, f, f

    @output
    v

    @info
    Sets the position that the camera will maintain relative to the target entity.
*/
static
void
n_setcameraoffset(void* slf, nCmd* cmd)
{
    CCChaseCamera* self = (CCChaseCamera*)slf;
    float x = cmd->In()->GetF();
    float y = cmd->In()->GetF();
    float z = cmd->In()->GetF();
    self->SetCameraOffset(x, y, z);
}
//------------------------------------------------------------------------------
/**
    @cmd
    setfocaloffset

    @input
    f, f, f

    @output
    v

    @info
    Sets the position of the point that the camera will look at, 
    relative to the target entity.  Thus, if you want to look right 
    at the entity, setfocaloffset(0,0,0).
*/
static
void
n_setfocaloffset(void* slf, nCmd* cmd)
{
    CCChaseCamera* self = (CCChaseCamera*)slf;
    float x = cmd->In()->GetF();
    float y = cmd->In()->GetF();
    float z = cmd->In()->GetF();
    self->SetFocalOffset(x, y, z);
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
    CCChaseCamera* self = (CCChaseCamera*)slf;
    nString modeStr( cmd->In()->GetS() );
	if(modeStr=="NONE")
	{
        self->SetSpinMode( CCChaseCamera::NONE );
	}
    else if(modeStr=="ORBIT")
	{
		self->SetSpinMode( CCChaseCamera::ORBIT );
	}
	else if(modeStr=="ROTATE")
	{
		self->SetSpinMode( CCChaseCamera::ROTATE );
	}
    else 
        n_printf("Unknown chase camera mode '%s': valid is 'NONE|ORBIT|ROTATE'.\n",modeStr.Get());
}


//------------------------------------------------------------------------------
/**
@cmd
setminzoomdist

@input
f

@output
v

@info
Sets the smallest permitted distance between focal point and camera position.
*/

static
void
n_setminzoomdist(void* slf, nCmd* cmd)
{
	CCChaseCamera* self = (CCChaseCamera*)slf;
	self->SetMinZoomDist(cmd->In()->GetF());
}
//------------------------------------------------------------------------------
/**
@cmd
setmaxzoomdist

@input
f

@output
v

@info
Sets the largest permitted distance between focal point and camera position.
Set to -1 for unlimited max distance.
*/

static
void
n_setmaxzoomdist(void* slf, nCmd* cmd)
{
	CCChaseCamera* self = (CCChaseCamera*)slf;
	self->SetMaxZoomDist(cmd->In()->GetF());
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
	CCChaseCamera* self = (CCChaseCamera*)slf;
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
	CCChaseCamera* self = (CCChaseCamera*)slf;
	self->SetRotateSpeed(cmd->In()->GetF());
}
//------------------------------------------------------------------------------
/**
@cmd
setorbitspeed

@input
f

@output
v

@info
Sets the rate at which the camera orbits (for a given mouse sensitivity)
*/

static
void
n_setorbitspeed(void* slf, nCmd* cmd)
{
	CCChaseCamera* self = (CCChaseCamera*)slf;
	self->SetOrbitSpeed(cmd->In()->GetF());
}
//------------------------------------------------------------------------------
/**
@cmd
setzoomspeed

@input
f

@output
v

@info
Sets the rate at which the camera zooms (for a given mouse sensitivity)
*/

static
void
n_setzoomspeed(void* slf, nCmd* cmd)
{
	CCChaseCamera* self = (CCChaseCamera*)slf;
	self->SetZoomSpeed(cmd->In()->GetF());
}

//------------------------------------------------------------------------------
/**
    @param  ps          writes the nCmd object contents out to a file.
    @return             success or failure
*/
bool
CCChaseCamera::SaveCmds(nPersistServer* ps)
{
    if (nRoot::SaveCmds(ps))
    {
        nCmd* cmd = ps->GetCmd(this, 'TGTE');
        ps->PutCmd(cmd);
        ps->GetCmd(this, 'SCOF');
        ps->PutCmd(cmd);
        ps->GetCmd(this, 'SFOF');
        ps->PutCmd(cmd);
        ps->GetCmd(this, 'SSPM');
        ps->PutCmd(cmd);
        ps->GetCmd(this, 'MINZ');
        ps->PutCmd(cmd);
        ps->GetCmd(this, 'MAXZ');
        ps->PutCmd(cmd);
        ps->GetCmd(this, 'SMOV');
        ps->PutCmd(cmd);
        ps->GetCmd(this, 'SORB');
        ps->PutCmd(cmd);
        ps->GetCmd(this, 'SROT');
        ps->PutCmd(cmd);
        ps->GetCmd(this, 'STZM');
        ps->PutCmd(cmd);
        
        return true;
    }
    return false;
}

