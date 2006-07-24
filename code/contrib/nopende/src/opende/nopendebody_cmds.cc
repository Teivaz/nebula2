//------------------------------------------------------------------------------
//  (c) 2004    Vadim Macagon
//------------------------------------------------------------------------------
#include "opende/nopendebody.h"
#include "kernel/npersistserver.h"

static void n_create( void* slf, nCmd* cmd );
static void n_setposition( void* slf, nCmd* cmd );
static void n_setrotation( void* slf, nCmd* cmd );
static void n_setquaternion( void* slf, nCmd* cmd );
static void n_setlinearvel( void* slf, nCmd* cmd );
static void n_setangularvel( void* slf, nCmd* cmd );
static void n_getposition( void* slf, nCmd* cmd );
static void n_getrotation( void* slf, nCmd* cmd );
static void n_getquaternion( void* slf, nCmd* cmd );
static void n_getlinearvel( void* slf, nCmd* cmd );
static void n_getangularvel( void* slf, nCmd* cmd );
static void n_addforce( void* slf, nCmd* cmd );
static void n_addtorque( void* slf, nCmd* cmd );
static void n_addrelforce( void* slf, nCmd* cmd );
static void n_addreltorque( void* slf, nCmd* cmd );
static void n_addforceatpos( void* slf, nCmd* cmd );
static void n_addforceatrelpos( void* slf, nCmd* cmd );
static void n_addrelforceatpos( void* slf, nCmd* cmd );
static void n_addrelforceatrelpos( void* slf, nCmd* cmd );
static void n_getforce( void* slf, nCmd* cmd );
static void n_gettorque( void* slf, nCmd* cmd );
static void n_setforce( void* slf, nCmd* cmd );
static void n_settorque( void* slf, nCmd* cmd );
static void n_getrelpointpos( void* slf, nCmd* cmd );
static void n_getrelpointvel( void* slf, nCmd* cmd );
static void n_getpointvel( void* slf, nCmd* cmd );
static void n_getposrelpoint( void* slf, nCmd* cmd );
static void n_vectortoworld( void* slf, nCmd* cmd );
static void n_vectorfromworld( void* slf, nCmd* cmd );
static void n_enable( void* slf, nCmd* cmd );
static void n_disable( void* slf, nCmd* cmd );
static void n_isenabled( void* slf, nCmd* cmd );
static void n_setfiniterotationmode( void* slf, nCmd* cmd );
static void n_getfiniterotationmode( void* slf, nCmd* cmd );
static void n_setfiniterotationaxis( void* slf, nCmd* cmd );
static void n_getfiniterotationaxis( void* slf, nCmd* cmd );
static void n_getnumjoints( void* slf, nCmd* cmd );
static void n_setgravitymode( void* slf, nCmd* cmd );
static void n_getgravitymode( void* slf, nCmd* cmd );
static void n_isconnectedto( void* slf, nCmd* cmd );
static void n_isconnectedtoexcluding( void* slf, nCmd* cmd );
static void n_resetmass( void* slf, nCmd* cmd );
static void n_setmassparams( void* slf, nCmd* cmd );
static void n_setspheremass( void* slf, nCmd* cmd );
static void n_setspheremasstotal( void* slf, nCmd* cmd );
static void n_setcapsulemass( void* slf, nCmd* cmd );
static void n_setcapsulemasstotal( void* slf, nCmd* cmd );
static void n_setcylindermass( void* slf, nCmd* cmd );
static void n_setcylindermasstotal( void* slf, nCmd* cmd );
static void n_setboxmass( void* slf, nCmd* cmd );
static void n_setboxmasstotal( void* slf, nCmd* cmd );
static void n_adjustmass( void* slf, nCmd* cmd );
static void n_translatemass( void* slf, nCmd* cmd );
static void n_rotatemass( void* slf, nCmd* cmd );
static void n_addmassof( void* slf, nCmd* cmd );

//------------------------------------------------------------------------------
/**
    @scriptclass
    nopendebody

    @superclass
    nroot

    @classinfo
    Encapsulates dBody functions.
*/
void
n_initcmds(nClass* clazz)
{
    clazz->BeginCmds();
    clazz->AddCmd( "v_Create_s",                'CREA', n_create );
    clazz->AddCmd( "v_SetPosition_fff",         'SPOS', n_setposition );
    clazz->AddCmd( "v_SetRotation_fffffffff",   'SROT', n_setrotation );
    clazz->AddCmd( "v_SetQuaternion_ffff",      'SQ__', n_setquaternion );
    clazz->AddCmd( "v_SetLinearVel_fff",        'SLV_', n_setlinearvel );
    clazz->AddCmd( "v_SetAngularVel_fff",       'SAV_', n_setangularvel );
    clazz->AddCmd( "fff_GetPosition_v",         'GPOS', n_getposition );
    clazz->AddCmd( "fffffffff_GetRotation_v",   'GROT', n_getrotation );
    clazz->AddCmd( "ffff_GetQuaternion_v",      'GQ__', n_getquaternion );
    clazz->AddCmd( "fff_GetLinearVel_v",            'GLV_', n_getlinearvel );
    clazz->AddCmd( "fff_GetAngularVel_v",           'GAV_', n_getangularvel );
    clazz->AddCmd( "v_AddForce_fff",            'AF__', n_addforce );
    clazz->AddCmd( "v_AddTorque_fff",           'AT__', n_addtorque );
    clazz->AddCmd( "v_AddRelForce_fff",         'ARF_', n_addrelforce );
    clazz->AddCmd( "v_AddRelTorque_fff",        'ART_', n_addreltorque );
    clazz->AddCmd( "v_AddForceAtPos_ffffff",    'AFP_', n_addforceatpos );
    clazz->AddCmd( "v_AddForceAtRelPos_ffffff", 'AFRP', n_addforceatrelpos );
    clazz->AddCmd( "v_AddRelForceAtPos_ffffff", 'ARFP', n_addrelforceatpos );
    clazz->AddCmd( "v_AddRelForceAtRelPos_ffffff", 'RFRP', n_addrelforceatrelpos );
    clazz->AddCmd( "fff_GetForce_v",            'GF__', n_getforce );
    clazz->AddCmd( "fff_GetTorque_v",           'GT__', n_gettorque );
    clazz->AddCmd( "v_SetForce_fff",            'SF__', n_setforce );
    clazz->AddCmd( "v_SetTorque_fff",           'ST__', n_settorque );
    clazz->AddCmd( "fff_GetRelPointPos_fff",    'GRPP', n_getrelpointpos );
    clazz->AddCmd( "fff_GetRelPointVel_fff",    'GRPV', n_getrelpointvel );
    clazz->AddCmd( "fff_GetPointVel_fff",       'GPV_', n_getpointvel );
    clazz->AddCmd( "fff_GetPosRelPoint_fff",    'GPRP', n_getposrelpoint );
    clazz->AddCmd( "fff_VectorToWorld_fff",     'VTW_', n_vectortoworld );
    clazz->AddCmd( "fff_VectorFromWorld_fff",   'VFW_', n_vectorfromworld );
    clazz->AddCmd( "v_Enable_v",                'ENBL', n_enable );
    clazz->AddCmd( "v_Disable_v",               'DIS_', n_disable );
    clazz->AddCmd( "b_IsEnabled_v",             'IE__', n_isenabled );
    clazz->AddCmd( "v_SetFiniteRotationMode_i", 'SFRM', n_setfiniterotationmode );
    clazz->AddCmd( "i_GetFiniteRotationMode_v", 'GFRM', n_getfiniterotationmode );
    clazz->AddCmd( "v_SetFiniteRotationAxis_fff", 'SFRA', n_setfiniterotationaxis );
    clazz->AddCmd( "fff_GetFiniteRotationAxis_v", 'GFRA', n_getfiniterotationaxis );
    clazz->AddCmd( "i_GetNumJoints_v",          'GNJ_', n_getnumjoints );
    clazz->AddCmd( "v_SetGravityMode_b",        'SGRV', n_setgravitymode );
    clazz->AddCmd( "b_GetGravityMode_v",        'GGRV', n_getgravitymode );
    clazz->AddCmd( "b_IsConnectedTo_s",         'ICT_', n_isconnectedto );
    clazz->AddCmd( "b_IsConnectedToExcluding_si", 'ICTE', n_isconnectedtoexcluding );
    clazz->AddCmd( "v_ResetMass_v",             'RSTM', n_resetmass );
    clazz->AddCmd( "v_SetMassParams_ffffffffff",'SMP_', n_setmassparams );
    clazz->AddCmd( "v_SetSphereMass_ff",        'SSM_', n_setspheremass );
    clazz->AddCmd( "v_SetSphereMassTotal_ff",   'SSTM', n_setspheremasstotal );
    clazz->AddCmd( "v_SetCapsuleMass_fiff",     'SCM_', n_setcapsulemass );
    clazz->AddCmd( "v_SetCapsuleMassTotal_fiff",'SCTM', n_setcapsulemasstotal );
    clazz->AddCmd( "v_SetCylinderMass_fiff",    'SCYM', n_setcylindermass );
    clazz->AddCmd( "v_SetCylinderMassTotal_fiff",'SCYT', n_setcylindermasstotal );
    clazz->AddCmd( "v_SetBoxMass_ffff",         'SBM_', n_setboxmass );
    clazz->AddCmd( "v_SetBoxMassTotal_ffff",    'SBTM', n_setboxmasstotal );
    clazz->AddCmd( "v_AdjustMass_f",            'ADJM', n_adjustmass );
    clazz->AddCmd( "v_TranslateMass_fff",       'TRNM', n_translatemass );
    clazz->AddCmd( "v_RotateMass_fffffffff",    'ROTM', n_rotatemass );
    clazz->AddCmd( "v_AddMassOf_s",             'ADDM', n_addmassof );
    clazz->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    Create

    @input
    s(nOpendeWorld)

    @output
    v

    @info
    Create the underlying ODE object.
*/
static
void n_create( void* slf, nCmd* cmd )
{
    nOpendeBody* self = (nOpendeBody*)slf;
    self->Create( cmd->In()->GetS() );
}

//------------------------------------------------------------------------------
/**
  @cmd
  SetPosition

  @input
  f(x), f(y), f(z)

  @output
  v

  @info
  Set the position of the body.
*/
static
void
n_setposition( void* slf, nCmd* cmd )
{
  nOpendeBody* self = (nOpendeBody*)slf;
  float x = cmd->In()->GetF();
  float y = cmd->In()->GetF();
  float z = cmd->In()->GetF();
  self->SetPosition( vector3(x, y, z) );
}

//------------------------------------------------------------------------------
/**
  @cmd
  GetPosition

  @input
  v

  @output
  f(x), f(y), f(z)

  @info
  Get the position of the body.
*/
static
void
n_getposition( void* slf, nCmd* cmd )
{
  nOpendeBody* self = (nOpendeBody*)slf;
  vector3 out = self->GetPosition();
  cmd->Out()->SetF( out.x );
  cmd->Out()->SetF( out.y );
  cmd->Out()->SetF( out.z );
}

//------------------------------------------------------------------------------
/**
  @cmd
  SetRotation

  @input
  f(M11), f(M12), f(M13),
  f(M21), f(M22), f(M23),
  f(M31), f(M32), f(M33)

  @output
  v

  @info
  Set the rotation of the body as a matrix33.
*/
static
void
n_setrotation( void* slf, nCmd* cmd )
{
  nOpendeBody* self = (nOpendeBody*)slf;
  matrix33 in;
  in.M11 = cmd->In()->GetF();
  in.M12 = cmd->In()->GetF();
  in.M13 = cmd->In()->GetF();
  in.M21 = cmd->In()->GetF();
  in.M22 = cmd->In()->GetF();
  in.M23 = cmd->In()->GetF();
  in.M31 = cmd->In()->GetF();
  in.M32 = cmd->In()->GetF();
  in.M33 = cmd->In()->GetF();
  self->SetRotation( in );
}

//------------------------------------------------------------------------------
/**
  @cmd
  GetRotation

  @input
  v

  @output
  f(M11), f(M12), f(M13),
  f(M21), f(M22), f(M23),
  f(M31), f(M32), f(M33)

  @info
  Get the rotation of the body as a matrix33.
*/
static
void
n_getrotation( void* slf, nCmd* cmd )
{
  nOpendeBody* self = (nOpendeBody*)slf;
  matrix33 out;
  self->GetRotation( out );
  cmd->Out()->SetF( out.M11 );
  cmd->Out()->SetF( out.M12 );
  cmd->Out()->SetF( out.M13 );
  cmd->Out()->SetF( out.M21 );
  cmd->Out()->SetF( out.M22 );
  cmd->Out()->SetF( out.M23 );
  cmd->Out()->SetF( out.M31 );
  cmd->Out()->SetF( out.M32 );
  cmd->Out()->SetF( out.M33 );
}

//------------------------------------------------------------------------------
/**
  @cmd
  SetQuaternion

  @input
  f(x), f(y), f(z), f(w)

  @output
  v

  @info
  Set the rotation of the body to the one represented by the quaternion.
*/
static
void
n_setquaternion( void* slf, nCmd* cmd )
{
  nOpendeBody* self = (nOpendeBody*)slf;
  quaternion q;
  q.x = cmd->In()->GetF();
  q.y = cmd->In()->GetF();
  q.z = cmd->In()->GetF();
  q.w = cmd->In()->GetF();
  self->SetQuaternion( q );
}

//------------------------------------------------------------------------------
/**
  @cmd
  GetQuaternion

  @input
  v

  @output
  f(x), f(y), f(z), f(w)

  @info
  Get a quaternion representing the rotation of the body.
*/
static
void
n_getquaternion( void* slf, nCmd* cmd )
{
  nOpendeBody* self = (nOpendeBody*)slf;
  quaternion out = self->GetQuaternion();
  cmd->Out()->SetF( out.x );
  cmd->Out()->SetF( out.y );
  cmd->Out()->SetF( out.z );
  cmd->Out()->SetF( out.w );
}

//------------------------------------------------------------------------------
/**
  @cmd
  SetLinearVel

  @input
  f(x), f(y), f(z)

  @output
  v

  @info
  Set linear velocity of the body.
*/
static
void
n_setlinearvel( void* slf, nCmd* cmd )
{
  nOpendeBody* self = (nOpendeBody*)slf;
  float x = cmd->In()->GetF();
  float y = cmd->In()->GetF();
  float z = cmd->In()->GetF();
  self->SetLinearVel( vector3(x, y, z) );
}

//------------------------------------------------------------------------------
/**
  @cmd
  GetLinearVel

  @input
  v

  @output
  f(x), f(y), f(z)

  @info
  Get the linear velocity of the body.
*/
static
void
n_getlinearvel( void* slf, nCmd* cmd )
{
  nOpendeBody* self = (nOpendeBody*)slf;
  vector3 vel = self->GetLinearVel();
  cmd->Out()->SetF( vel.x );
  cmd->Out()->SetF( vel.y );
  cmd->Out()->SetF( vel.z );
}

//------------------------------------------------------------------------------
/**
  @cmd
  SetAngularVel

  @input
  f(x), f(y), f(z)

  @output
  v

  @info
  Set the angular velocity of the body.
*/
static
void
n_setangularvel( void* slf, nCmd* cmd )
{
  nOpendeBody* self = (nOpendeBody*)slf;
  float x = cmd->In()->GetF();
  float y = cmd->In()->GetF();
  float z = cmd->In()->GetF();
  self->SetAngularVel( vector3(x, y, z) );
}

//------------------------------------------------------------------------------
/**
  @cmd
  GetAngularVel

  @input
  v

  @output
  f(x), f(y), f(z)

  @info
  Get the angular velocity of the body.
*/
static
void
n_getangularvel( void* slf, nCmd* cmd )
{
  nOpendeBody* self = (nOpendeBody*)slf;
  vector3 vel = self->GetAngularVel();
  cmd->Out()->SetF( vel.x );
  cmd->Out()->SetF( vel.y );
  cmd->Out()->SetF( vel.z );
}

//------------------------------------------------------------------------------
/**
  @cmd
  AddForce

  @input
  f(fx), f(fy), f(fz)

  @output
  v

  @info
  See ODE manual. (dBodyAddForce)
*/
static
void
n_addforce( void* slf, nCmd* cmd )
{
  nOpendeBody* self = (nOpendeBody*)slf;
  float x = cmd->In()->GetF();
  float y = cmd->In()->GetF();
  float z = cmd->In()->GetF();
  self->AddForce( vector3(x, y, z) );
}

//------------------------------------------------------------------------------
/**
  @cmd
  AddTorque

  @input
  f(fx), f(fy), f(fz)

  @output
  v

  @info
  See ODE manual. (dBodyAddTorque)
*/
static
void
n_addtorque( void* slf, nCmd* cmd )
{
  nOpendeBody* self = (nOpendeBody*)slf;
  float x = cmd->In()->GetF();
  float y = cmd->In()->GetF();
  float z = cmd->In()->GetF();
  self->AddTorque( vector3(x, y, z) );
}

//------------------------------------------------------------------------------
/**
  @cmd
  AddRelForce

  @input
  f(fx), f(fy), f(fz)

  @output
  v

  @info
  See ODE manual. (dBodyAddRelForce)
*/
static
void
n_addrelforce( void* slf, nCmd* cmd )
{
  nOpendeBody* self = (nOpendeBody*)slf;
  float x = cmd->In()->GetF();
  float y = cmd->In()->GetF();
  float z = cmd->In()->GetF();
  self->AddRelForce( vector3(x, y, z) );
}

//------------------------------------------------------------------------------
/**
  @cmd
  AddRelTorque

  @input
  f(fx), f(fy), f(fz)

  @output
  v

  @info
  See ODE manual. (dBodyAddRelTorque)
*/
static
void
n_addreltorque( void* slf, nCmd* cmd )
{
  nOpendeBody* self = (nOpendeBody*)slf;
  float x = cmd->In()->GetF();
  float y = cmd->In()->GetF();
  float z = cmd->In()->GetF();
  self->AddRelTorque( vector3(x, y, z) );
}

//------------------------------------------------------------------------------
/**
  @cmd
  AddForceAtPos

  @input
  f(fx), f(fy), f(fz), f(px), f(py), f(pz)

  @output
  v

  @info
  See ODE manual. (dBodyAddForceAtPos)
*/
static
void
n_addforceatpos( void* slf, nCmd* cmd )
{
  nOpendeBody* self = (nOpendeBody*)slf;
  float fx = cmd->In()->GetF();
  float fy = cmd->In()->GetF();
  float fz = cmd->In()->GetF();
  float px = cmd->In()->GetF();
  float py = cmd->In()->GetF();
  float pz = cmd->In()->GetF();
  self->AddForceAtPos( vector3(fx, fy, fz), vector3( px, py, pz ) );
}

//------------------------------------------------------------------------------
/**
  @cmd
  AddForceAtRelPos

  @input
  f(fx), f(fy), f(fz), f(px), f(py), f(pz)

  @output
  v

  @info
  See ODE manual. (dBodyAddForceAtRelPos)
*/
static
void
n_addforceatrelpos( void* slf, nCmd* cmd )
{
  nOpendeBody* self = (nOpendeBody*)slf;
  float fx = cmd->In()->GetF();
  float fy = cmd->In()->GetF();
  float fz = cmd->In()->GetF();
  float px = cmd->In()->GetF();
  float py = cmd->In()->GetF();
  float pz = cmd->In()->GetF();
  self->AddForceAtRelPos( vector3(fx, fy, fz), vector3(px, py, pz) );
}

//------------------------------------------------------------------------------
/**
  @cmd
  AddRelForceAtPos

  @input
  f(fx), f(fy), f(fz), f(px), f(py), f(pz)

  @output
  v

  @info
  See ODE manual. (dBodyAddRelForceAtPos)
*/
static
void
n_addrelforceatpos( void* slf, nCmd* cmd )
{
  nOpendeBody* self = (nOpendeBody*)slf;
  float fx = cmd->In()->GetF();
  float fy = cmd->In()->GetF();
  float fz = cmd->In()->GetF();
  float px = cmd->In()->GetF();
  float py = cmd->In()->GetF();
  float pz = cmd->In()->GetF();
  self->AddRelForceAtPos( vector3(fx, fy, fz), vector3(px, py, pz) );
}

//------------------------------------------------------------------------------
/**
  @cmd
  AddRelForceAtRelPos

  @input
  f(fx), f(fy), f(fz), f(px), f(py), f(pz)

  @output
  v

  @info
  See ODE manual. (dBodyAddRelForceAtRelPos)
*/
static
void
n_addrelforceatrelpos( void* slf, nCmd* cmd )
{
  nOpendeBody* self = (nOpendeBody*)slf;
  float fx = cmd->In()->GetF();
  float fy = cmd->In()->GetF();
  float fz = cmd->In()->GetF();
  float px = cmd->In()->GetF();
  float py = cmd->In()->GetF();
  float pz = cmd->In()->GetF();
  self->AddRelForceAtRelPos( vector3(fx, fy, fz), vector3(px, py, pz) );
}

//------------------------------------------------------------------------------
/**
  @cmd
  GetForce

  @input
  v

  @output
  f(fx), f(fy), f(fz)

  @info
  Get the accumulated force vector for the body.
*/
static
void
n_getforce( void* slf, nCmd* cmd )
{
  nOpendeBody* self = (nOpendeBody*)slf;
  vector3 f = self->GetForce();
  cmd->Out()->SetF( f.x );
  cmd->Out()->SetF( f.y );
  cmd->Out()->SetF( f.z );
}

//------------------------------------------------------------------------------
/**
  @cmd
  GetTorque

  @input
  v

  @output
  f(fx), f(fy), f(fz)

  @info
  Get the accumulated torque vector for the body.
*/
static
void
n_gettorque( void* slf, nCmd* cmd )
{
  nOpendeBody* self = (nOpendeBody*)slf;
  vector3 f = self->GetTorque();
  cmd->Out()->SetF( f.x );
  cmd->Out()->SetF( f.y );
  cmd->Out()->SetF( f.z );
}

//------------------------------------------------------------------------------
/**
  @cmd
  SetForce

  @input
  f(fx), f(fy), f(fz)

  @output
  v

  @info
  Set the accumulated force vector for the body.
*/
static
void
n_setforce( void* slf, nCmd* cmd )
{
  nOpendeBody* self = (nOpendeBody*)slf;
  float fx = cmd->In()->GetF();
  float fy = cmd->In()->GetF();
  float fz = cmd->In()->GetF();
  self->SetForce( vector3(fx, fy, fz) );
}

//------------------------------------------------------------------------------
/**
  @cmd
  SetTorque

  @input
  f(fx), f(fy), f(fz)

  @output
  v

  @info
  Set the accumulated torque vector for the body.
*/
static
void
n_settorque( void* slf, nCmd* cmd )
{
  nOpendeBody* self = (nOpendeBody*)slf;
  float fx = cmd->In()->GetF();
  float fy = cmd->In()->GetF();
  float fz = cmd->In()->GetF();
  self->SetTorque( vector3(fx, fy, fz) );
}

//------------------------------------------------------------------------------
/**
  @cmd
  GetRelPointPos

  @input
  f(BodyRelativeX), f(BodyRelativeY), f(BodyRelativeZ)

  @output
  f(GlobalX), f(GlobalY), f(GlobalZ)

  @info
  See ODE manual. (dBodyGetRelPointPos)
*/
static
void
n_getrelpointpos( void* slf, nCmd* cmd )
{
  nOpendeBody* self = (nOpendeBody*)slf;
  float brx = cmd->In()->GetF();
  float bry = cmd->In()->GetF();
  float brz = cmd->In()->GetF();
  vector3 res;
  self->GetRelPointPos( vector3(brx, bry, brz), res );
  cmd->Out()->SetF( res.x );
  cmd->Out()->SetF( res.y );
  cmd->Out()->SetF( res.z );
}

//------------------------------------------------------------------------------
/**
  @cmd
  GetRelPointVel

  @input
  f(BodyRelativeX), f(BodyRelativeY), f(BodyRelativeZ)

  @output
  f(GlobalVelX), f(GlobalVelY), f(GlobalVelZ)

  @info
  See ODE manual. (dBodyGetRelPointVel)
*/
static
void
n_getrelpointvel( void* slf, nCmd* cmd )
{
  nOpendeBody* self = (nOpendeBody*)slf;
  float brx = cmd->In()->GetF();
  float bry = cmd->In()->GetF();
  float brz = cmd->In()->GetF();
  vector3 res;
  self->GetRelPointVel( vector3(brx, bry, brz), res );
  cmd->Out()->SetF( res.x );
  cmd->Out()->SetF( res.y );
  cmd->Out()->SetF( res.z );
}

//------------------------------------------------------------------------------
/**
  @cmd
  GetPointVel

  @input
  f(GlobalX), f(GlobalY), f(GlobalZ)

  @output
  f(GlobalVelX), f(GlobalVelY), f(GlobalVelZ)

  @info
  See ODE manual. (dBodyGetPointVel)
*/
static
void
n_getpointvel( void* slf, nCmd* cmd )
{
  nOpendeBody* self = (nOpendeBody*)slf;
  float gx = cmd->In()->GetF();
  float gy = cmd->In()->GetF();
  float gz = cmd->In()->GetF();
  vector3 res;
  self->GetPointVel( vector3(gx, gy, gz), res );
  cmd->Out()->SetF( res.x );
  cmd->Out()->SetF( res.y );
  cmd->Out()->SetF( res.z );
}

//------------------------------------------------------------------------------
/**
  @cmd
  GetPosRelPoint

  @input
  f(GlobalX), f(GlobalY), f(GlobalZ)

  @output
  f(BodyRelativeX), f(BodyRelativeY), f(BodyRelativeZ)

  @info
  See ODE manual. (dBodyGetPosRelPoint)
*/
static
void
n_getposrelpoint( void* slf, nCmd* cmd )
{
  nOpendeBody* self = (nOpendeBody*)slf;
  float gx = cmd->In()->GetF();
  float gy = cmd->In()->GetF();
  float gz = cmd->In()->GetF();
  vector3 res;
  self->GetPosRelPoint( vector3(gx, gy, gz), res );
  cmd->Out()->SetF( res.x );
  cmd->Out()->SetF( res.y );
  cmd->Out()->SetF( res.z );
}

//------------------------------------------------------------------------------
/**
  @cmd
  VectorToWorld

  @input
  f(BodyRelativeX), f(BodyRelativeY), f(BodyRelativeZ)

  @output
  f(GlobalX), f(GlobalY), f(GlobalZ)

  @info
  Transform a vector expressed in the body coordinate system to
  the global/world coordinate system.
*/
static
void
n_vectortoworld( void* slf, nCmd* cmd )
{
  nOpendeBody* self = (nOpendeBody*)slf;
  float brx = cmd->In()->GetF();
  float bry = cmd->In()->GetF();
  float brz = cmd->In()->GetF();
  vector3 res;
  self->VectorToWorld( vector3(brx, bry, brz), res );
  cmd->Out()->SetF( res.x );
  cmd->Out()->SetF( res.y );
  cmd->Out()->SetF( res.z );
}

//------------------------------------------------------------------------------
/**
  @cmd
  VectorFromWorld

  @input
  f(GlobalX), f(GlobalY), f(GlobalZ)

  @output
  f(BodyRelativeX), f(BodyRelativeY), f(BodyRelativeZ)

  @info
  Transform a vector expressed in the global/world coordinate system
  to the body coordinate system.
*/
static
void
n_vectorfromworld( void* slf, nCmd* cmd )
{
  nOpendeBody* self = (nOpendeBody*)slf;
  float gx = cmd->In()->GetF();
  float gy = cmd->In()->GetF();
  float gz = cmd->In()->GetF();
  vector3 res;
  self->VectorFromWorld( vector3(gx, gy, gz), res );
  cmd->Out()->SetF( res.x );
  cmd->Out()->SetF( res.y );
  cmd->Out()->SetF( res.z );
}

//------------------------------------------------------------------------------
/**
  @cmd
  Enable

  @input
  v

  @output
  v

  @info
  Enable the body.
*/
static
void n_enable( void* slf, nCmd* cmd )
{
    nOpendeBody* self = (nOpendeBody*)slf;
    self->Enable();
}

//------------------------------------------------------------------------------
/**
  @cmd
  Disable

  @input
  v

  @output
  v

  @info
  Disable the body.
*/
static
void n_disable( void* slf, nCmd* cmd )
{
    nOpendeBody* self = (nOpendeBody*)slf;
    self->Disable();
}

//------------------------------------------------------------------------------
/**
  @cmd
  IsEnabled

  @input
  v

  @output
  b(True/False)

  @info
  Check if the body is enabled.
*/
static
void
n_isenabled( void* slf, nCmd* cmd )
{
  nOpendeBody* self = (nOpendeBody*)slf;
  cmd->Out()->SetB( self->IsEnabled() );
}

//------------------------------------------------------------------------------
/**
  @cmd
  SetFiniteRotationMode

  @input
  i(Mode)

  @output
  v

  @info
  See ODE manual. (dBodySetFiniteRotationMode)
*/
static
void
n_setfiniterotationmode( void* slf, nCmd* cmd )
{
  nOpendeBody* self = (nOpendeBody*)slf;
  self->SetFiniteRotationMode( cmd->In()->GetI() );
}

//------------------------------------------------------------------------------
/**
  @cmd
  GetFiniteRotationMode

  @input
  v

  @output
  i(Mode)

  @info
  Get the finite rotation mode for the body.
*/
static
void
n_getfiniterotationmode( void* slf, nCmd* cmd )
{
  nOpendeBody* self = (nOpendeBody*)slf;
  cmd->Out()->SetI( self->GetFiniteRotationMode() );
}

//------------------------------------------------------------------------------
/**
  @cmd
  SetFiniteRotationAxis

  @input
  f(x), f(y), f(z)

  @output
  v

  @info
  See ODE manual. (dBodySetFiniteRotationAxis)
*/
static
void
n_setfiniterotationaxis( void* slf, nCmd* cmd )
{
  nOpendeBody* self = (nOpendeBody*)slf;
  float x = cmd->In()->GetF();
  float y = cmd->In()->GetF();
  float z = cmd->In()->GetF();
  self->SetFiniteRotationAxis( vector3(x, y, z) );
}

//------------------------------------------------------------------------------
/**
  @cmd
  GetFiniteRotationAxis

  @input
  v

  @output
  f(x), f(y), f(z)

  @info
  Get the finite rotation axis of the body.
*/
static
void
n_getfiniterotationaxis( void* slf, nCmd* cmd )
{
  nOpendeBody* self = (nOpendeBody*)slf;
  vector3 res;
  self->GetFiniteRotationAxis( res );
  cmd->Out()->SetF( res.x );
  cmd->Out()->SetF( res.y );
  cmd->Out()->SetF( res.z );
}

//------------------------------------------------------------------------------
/**
  @cmd
  GetNumJoints

  @input
  v

  @output
  i(Number of Joints)

  @info
  Get the number of joints that are attached to the body.
*/
static
void
n_getnumjoints( void* slf, nCmd* cmd )
{
  nOpendeBody* self = (nOpendeBody*)slf;
  cmd->Out()->SetI( self->GetNumJoints() );
}

//------------------------------------------------------------------------------
/**
  @cmd
  SetGravityMode

  @input
  b(True/False)

  @output
  v

  @info
  Specify whether the body should be influenced by gravity.
*/
static
void
n_setgravitymode( void* slf, nCmd* cmd )
{
  nOpendeBody* self = (nOpendeBody*)slf;
  self->SetGravityMode( cmd->In()->GetB() );
}

//------------------------------------------------------------------------------
/**
  @cmd
  GetGravityMode

  @input
  v

  @output
  b(True/False)

  @info
  Check if the body is influenced by gravity.
*/
static
void
n_getgravitymode( void* slf, nCmd* cmd )
{
  nOpendeBody* self = (nOpendeBody*)slf;
  cmd->Out()->SetB( self->GetGravityMode() );
}

//------------------------------------------------------------------------------
/**
  @cmd
  IsConnectedTo

  @input
  s(nOpendeBody name)

  @output
  b(True/False)

  @info
  Check if the bodies are connected by a joint.
*/
static
void
n_isconnectedto( void* slf, nCmd* cmd )
{
  nOpendeBody* self = (nOpendeBody*)slf;
  nOpendeBody* other = (nOpendeBody*)self->kernelServer->Lookup( cmd->In()->GetS() );
  n_assert( other && "Body 2 not found!" );
  cmd->Out()->SetB( self->IsConnectedTo( other->id ) );
}

//------------------------------------------------------------------------------
/**
  @cmd
  IsConnectedToExcluding

  @input
  s(nOpendeBody name), i(JointType)

  @output
  b(True/False)

  @info
  Check if the bodies are connected by a joint that is not of the type JointType.
*/
static
void
n_isconnectedtoexcluding( void* slf, nCmd* cmd )
{
  nOpendeBody* self = (nOpendeBody*)slf;
  nOpendeBody* other = (nOpendeBody*)self->kernelServer->Lookup( cmd->In()->GetS() );
  n_assert( other && "Body 2 not found!" );
  int jointType = cmd->In()->GetI();
  cmd->Out()->SetB( self->IsConnectedToExcluding( other->id, jointType ) );
}

//------------------------------------------------------------------------------
/**
  @cmd
  ResetMass

  @input
  v

  @output
  v

  @info
*/
static
void
n_resetmass( void* slf, nCmd* cmd )
{
  nOpendeBody* self = (nOpendeBody*)slf;
  self->ResetMass();
}

//------------------------------------------------------------------------------
/**
  @cmd
  SetMassParams

  @input
  f(theMass), f(cgx), f(cgy), f(cgz),
  f(I11), f(I22), f(I33),
  f(I12), f(I13), f(I23)

  @output
  v

  @info
*/
static
void
n_setmassparams( void* slf, nCmd* cmd )
{
  nOpendeBody* self = (nOpendeBody*)slf;
  float theMass = cmd->In()->GetF();
  vector3 cg;
  cg.x = cmd->In()->GetF();
  cg.y = cmd->In()->GetF();
  cg.z = cmd->In()->GetF();
  float I11 = cmd->In()->GetF();
  float I22 = cmd->In()->GetF();
  float I33 = cmd->In()->GetF();
  float I12 = cmd->In()->GetF();
  float I13 = cmd->In()->GetF();
  float I23 = cmd->In()->GetF();
  self->SetMassParams( theMass, cg, I11, I22, I33, I12, I13, I23 );
}

//------------------------------------------------------------------------------
/**
  @cmd
  SetSphereMass

  @input
  f(density), f(radius)

  @output
  v

  @info
*/
static
void
n_setspheremass( void* slf, nCmd* cmd )
{
  nOpendeBody* self = (nOpendeBody*)slf;
  float d = cmd->In()->GetF();
  float r = cmd->In()->GetF();
  self->SetSphereMass( d, r );
}

//------------------------------------------------------------------------------
/**
  @cmd
  SetSphereMassTotal

  @input
  f(totalMass), f(radius)

  @output
  v

  @info
*/
static
void
n_setspheremasstotal( void* slf, nCmd* cmd )
{
    nOpendeBody* self = (nOpendeBody*)slf;
    float m = cmd->In()->GetF();
    float r = cmd->In()->GetF();
    self->SetSphereMassTotal( m, r );
}

//------------------------------------------------------------------------------
/**
  @cmd
  SetCapsuleMass

  @input
  f(density), i(direction), f(radius), f(length)

  @output
  v

  @info
*/
static
void
n_setcapsulemass( void* slf, nCmd* cmd )
{
  nOpendeBody* self = (nOpendeBody*)slf;
  float d = cmd->In()->GetF();
  int dir = cmd->In()->GetI();
  float r = cmd->In()->GetF();
  float length = cmd->In()->GetF();
  self->SetCapsuleMass( d, dir, r, length );
}

//------------------------------------------------------------------------------
/**
  @cmd
  SetCapsuleMassTotal

  @input
  f(totalMass), i(direction), f(radius), f(length)

  @output
  v

  @info
*/
static
void
n_setcapsulemasstotal( void* slf, nCmd* cmd )
{
  nOpendeBody* self = (nOpendeBody*)slf;
  float t = cmd->In()->GetF();
  int dir = cmd->In()->GetI();
  float r = cmd->In()->GetF();
  float length = cmd->In()->GetF();
  self->SetCapsuleMassTotal( t, dir, r, length );
}

//------------------------------------------------------------------------------
/**
  @cmd
  SetCylinderMass

  @input
  f(density), i(direction), f(radius), f(length)

  @output
  v

  @info
*/
static
void
n_setcylindermass( void* slf, nCmd* cmd )
{
  nOpendeBody* self = (nOpendeBody*)slf;
  float d = cmd->In()->GetF();
  int dir = cmd->In()->GetI();
  float r = cmd->In()->GetF();
  float length = cmd->In()->GetF();
  self->SetCylinderMass( d, dir, r, length );
}

//------------------------------------------------------------------------------
/**
  @cmd
  SetCylinderMassTotal

  @input
  f(totalMass), i(direction), f(radius), f(length)

  @output
  v

  @info
*/
static
void
n_setcylindermasstotal( void* slf, nCmd* cmd )
{
  nOpendeBody* self = (nOpendeBody*)slf;
  float t = cmd->In()->GetF();
  int dir = cmd->In()->GetI();
  float r = cmd->In()->GetF();
  float length = cmd->In()->GetF();
  self->SetCylinderMassTotal( t, dir, r, length );
}

//------------------------------------------------------------------------------
/**
  @cmd
  SetBoxMass

  @input
  f(density), f(lx), f(ly), f(lz)

  @output
  v

  @info
*/
static
void
n_setboxmass( void* slf, nCmd* cmd )
{
  nOpendeBody* self = (nOpendeBody*)slf;
  float d = cmd->In()->GetF();
  float lx = cmd->In()->GetF();
  float ly = cmd->In()->GetF();
  float lz = cmd->In()->GetF();
  self->SetBoxMass( d, lx, ly, lz );
}

//------------------------------------------------------------------------------
/**
  @cmd
  SetBoxMassTotal

  @input
  f(totalMass), f(lx), f(ly), f(lz)

  @output
  v

  @info
*/
static
void
n_setboxmasstotal( void* slf, nCmd* cmd )
{
  nOpendeBody* self = (nOpendeBody*)slf;
  float t = cmd->In()->GetF();
  float lx = cmd->In()->GetF();
  float ly = cmd->In()->GetF();
  float lz = cmd->In()->GetF();
  self->SetBoxMassTotal( t, lx, ly, lz );
}

//------------------------------------------------------------------------------
/**
  @cmd
  AdjustMass

  @input
  f(newMass)

  @output
  v

  @info
*/
static
void
n_adjustmass( void* slf, nCmd* cmd )
{
  nOpendeBody* self = (nOpendeBody*)slf;
  float n = cmd->In()->GetF();
  self->AdjustMass( n );
}

//------------------------------------------------------------------------------
/**
  @cmd
  TranslateMass

  @input
  f(x), f(y), f(z)

  @output
  v

  @info
*/
static
void
n_translatemass( void* slf, nCmd* cmd )
{
  nOpendeBody* self = (nOpendeBody*)slf;
  vector3 t;
  t.x = cmd->In()->GetF();
  t.y = cmd->In()->GetF();
  t.z = cmd->In()->GetF();
  self->TranslateMass( t );
}

//------------------------------------------------------------------------------
/**
  @cmd
  RotateMass

  @input
  f(M11), f(M12), f(M13),
  f(M21), f(M22), f(M23),
  f(M31), f(M32), f(M33)

  @output
  v

  @info
*/
static
void
n_rotatemass( void* slf, nCmd* cmd )
{
  nOpendeBody* self = (nOpendeBody*)slf;
  matrix33 m;
  m.M11 = cmd->In()->GetF();
  m.M12 = cmd->In()->GetF();
  m.M13 = cmd->In()->GetF();
  m.M21 = cmd->In()->GetF();
  m.M22 = cmd->In()->GetF();
  m.M23 = cmd->In()->GetF();
  m.M31 = cmd->In()->GetF();
  m.M32 = cmd->In()->GetF();
  m.M33 = cmd->In()->GetF();
  self->RotateMass( m );
}

//------------------------------------------------------------------------------
/**
  @cmd
  AddMassOf

  @input
  s(nOpendeBody name)

  @output
  v

  @info
*/
static
void
n_addmassof( void* slf, nCmd* cmd )
{
  nOpendeBody* self = (nOpendeBody*)slf;
  self->AddMassOf( cmd->In()->GetS() );
}

//------------------------------------------------------------------------------
/**
    @param  ps          writes the nCmd object contents out to a file.
    @return             success or failure
*/
bool
nOpendeBody::SaveCmds( nPersistServer* ps )
{
    if ( nRoot::SaveCmds( ps ) )
    {
        // TODO

        // Create
        //nCmd* cmd = ps->GetCmd( this, 'CREA' );
        //cmd->In()->SetS( );
        //ps->PutCmd( cmd );



        return true;
    }
    return false;
}
