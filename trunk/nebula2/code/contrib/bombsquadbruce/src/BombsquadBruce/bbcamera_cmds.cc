//------------------------------------------------------------------------------
//  (C) 2003 Rafael Van Daele-Hunt
//------------------------------------------------------------------------------
#include "BombsquadBruce/bbcamera.h"
#include "kernel/npersistserver.h"
#include "mathlib/sphere.h"

static void n_trigger(void* slf, nCmd* cmd);
static void n_manipulatevisibleelements(void* slf, nCmd* cmd);
static void n_setcontrol(void* slf, nCmd* cmd);
static void n_setdebugshape(void* slf, nCmd* cmd);
static void n_setfarplane(void* slf, nCmd* cmd);
static void n_setnearplane(void* slf, nCmd* cmd);
static void n_setangleofview(void* slf, nCmd* cmd);
static void n_getposition(void* slf, nCmd* cmd);
static void n_getorientation(void* slf, nCmd* cmd);
static void n_setsphereculler(void* slf, nCmd* cmd);
static void n_setfrustumculler(void* slf, nCmd* cmd);
//------------------------------------------------------------------------------
/**
    @scriptclass
    bbcamera
    
    @superclass
    nroot

    @classinfo
    BBCamera links named user input channels to camera movement instructions.
    How the instructions are interpreted depends on which kind of BBCamera
    subclass is in use.
*/
void
n_initcmds(nClass* clazz)
{
    clazz->BeginCmds();
    clazz->AddCmd("v_trigger_f",                     'TRIG', n_trigger);
    clazz->AddCmd("v_manipulatevisibleelements_v",   'MANV', n_manipulatevisibleelements);
    clazz->AddCmd("v_setcontrol_ss",                 'CTRL', n_setcontrol );
    clazz->AddCmd("v_setdebugshape_s",               'SDBS', n_setdebugshape );
    clazz->AddCmd("v_setfarplane_f",                 'SFPL', n_setfarplane );
    clazz->AddCmd("v_setnearplane_f",                'SNPL', n_setnearplane );
    clazz->AddCmd("v_setangleofview_f",              'SFOV', n_setangleofview );
    clazz->AddCmd("fff_getposition_v",               'GPOS', n_getposition);
    clazz->AddCmd("ffff_getorientation_v",           'GORI', n_getorientation);
    clazz->AddCmd("v_setsphereculler_ffff",          'SSCU', n_setsphereculler);
    clazz->AddCmd("v_setfrustumculler_v",            'SFCU', n_setfrustumculler);
    clazz->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    trigger

    @input
    f (deltaTime)

    @output
    v

    @info
    Updates the camera's position, orientation, etc.
*/
static
void
n_trigger(void* slf, nCmd* cmd)
{
    BBCamera* self = (BBCamera*)slf;
    self->Trigger(cmd->In()->GetF());
}
//------------------------------------------------------------------------------
/**
    @cmd
    manipulatevisibleelements

    @input
    v

    @output
    v

    @info
    Calls DoManipulate (a script function!) on
    every element not culled by this camera.
*/
static
void
n_manipulatevisibleelements(void* slf, nCmd* cmd)
{
    BBCamera* self = (BBCamera*)slf;
    self->ManipulateVisibleElements();
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
Tells a camera control (e.g. SPIN_LEFT) to listen to a certain channel (e.g. mouseleft).
The channels can be freely named and defined in the scripts (how to do this falls outside
the scope of this comment); available controls are: SPIN_LEFT, SPIN_RIGHT, SPIN_UP, 
SPIN_DOWN, MOVE_LEFT, MOVE_RIGHT, MOVE_FWD, MOVE_BACK, ZOOM_IN, ZOOM_OUT.

*/

static
void
n_setcontrol(void* slf, nCmd* cmd)
{
    BBCamera* self = (BBCamera*)slf;
    std::string control( cmd->In()->GetS() );
    std::string channel( cmd->In()->GetS() );
    self->SetControl(control, channel);

}
//------------------------------------------------------------------------------
/**
@cmd
setdebugshape

@input
s (NOH path)

@output
v

@info
Sets a shapenode that will be displayed when this camera is being observed
by a debug camera.

*/

static
void
n_setdebugshape(void* slf, nCmd* cmd)
{
	BBCamera* self = (BBCamera*)slf;
    self->SetDebugShape(cmd->In()->GetS());
}
//------------------------------------------------------------------------------
/**
@cmd
setfarplane

@input
f (desired distance from camera to far plane)

@output
v

@info
Tells the camera where to place the far plane 
(the distance after which nothing is rendered)
*/
static
void
n_setfarplane(void* slf, nCmd* cmd)
{
	BBCamera* self = (BBCamera*)slf;
	self->SetFarPlane(cmd->In()->GetF());
}
//------------------------------------------------------------------------------
/**
@cmd
setnearplane

@input
f (desired distance from camera to near plane)

@output
v

@info
Tells the camera where to place the near plane 
(the distance after which nothing is rendered)
*/
static
void
n_setnearplane(void* slf, nCmd* cmd)
{
	BBCamera* self = (BBCamera*)slf;
	self->SetNearPlane(cmd->In()->GetF());
}
//------------------------------------------------------------------------------
/**
@cmd
setangleofview

@input
f (angle of view in degree)

@output
v

@info
Sets the angle of view
*/
static
void
n_setangleofview(void* slf, nCmd* cmd)
{
	BBCamera* self = (BBCamera*)slf;
	self->SetAngleOfView(cmd->In()->GetF());
}
//------------------------------------------------------------------------------
/**
    @cmd
    getposition

    @input
    v

    @output
    f, f, f

    @info
    Gets camera position in worldspace.
*/
static
void
n_getposition(void* slf, nCmd* cmd)
{
    BBCamera* self = (BBCamera*)slf;
    const vector3& pos = self->GetPosition();
    cmd->Out()->SetF(pos.x);
    cmd->Out()->SetF(pos.y);
    cmd->Out()->SetF(pos.z);

}
//------------------------------------------------------------------------------
/**
    @cmd
    getorientation

    @input
    v

    @output
    f, f, f, f

    @info
    Gets camera orientation as a quaternion.
*/
static
void
n_getorientation(void* slf, nCmd* cmd)
{
    BBCamera* self = (BBCamera*)slf;
    const quaternion& quat = self->GetOrientation();
    cmd->Out()->SetF(quat.x);
    cmd->Out()->SetF(quat.y);
    cmd->Out()->SetF(quat.z);
    cmd->Out()->SetF(quat.w);

}
//------------------------------------------------------------------------------
/**
    @cmd
    setsphereculler

    @input
    fff (offset) f (radius)

    @output
    v

    @info
    Activates spherical culling (deactivating the current culling),
    using a sphere of the given radius, offset from the camera's
    current position by the specified vector *in camera space*
    (that is, if offset is 0,0,1, the sphere will be centred one 
    unit in front of the camera, regardless of camera facing).
*/
static
void
n_setsphereculler(void* slf, nCmd* cmd)
{
    BBCamera* self = (BBCamera*)slf;
    vector3 offset;
    offset.x = cmd->In()->GetF();
    offset.y = cmd->In()->GetF();
    offset.z = cmd->In()->GetF();
    self->SetSphereCuller( sphere( offset, cmd->In()->GetF() ) );
}
//------------------------------------------------------------------------------
/**
    @cmd
    setfrustumculler

    @input
    v

    @output
    v

    @info
    Activates frustum culling (deactivating the current culling),
    using the camera's current position, orientation, near & far 
    planes, etc.
*/
static
void
n_setfrustumculler(void* slf, nCmd* cmd)
{
    BBCamera* self = (BBCamera*)slf;
    self->SetFrustumCuller();
}
//------------------------------------------------------------------------------
/**
    @param  ps          writes the nCmd object contents out to a file.
    @return             success or failure
*/
bool
BBCamera::SaveCmds(nPersistServer* ps)
{
    if (nRoot::SaveCmds(ps))
    {
//        ps->GetCmd(this, 'CTRL');
//        cmd->In()->SetS( 
//        ps->PutCmd(cmd);

        nCmd* cmd = ps->GetCmd(this, 'SFPL');
        cmd->In()->SetF( GetFarPlane() );
        ps->PutCmd(cmd);
      
        //ps->GetCmd(this, 'SFOV');
        //cmd->In()->SetF( GetFarPlane() );
        //ps->PutCmd(cmd)

        //ps->GetCmd(this, 'SFOV');
        //cmd->In()->SetF( ) );
        //ps->PutCmd(cmd);
        return true;
    }
    return false;
}

