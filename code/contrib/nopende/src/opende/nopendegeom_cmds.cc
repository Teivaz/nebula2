//------------------------------------------------------------------------------
//  (c) 2004    Vadim Macagon
//------------------------------------------------------------------------------
#include "opende/nopendegeom.h"
#include "kernel/npersistserver.h"

static void n_Create( void* slf, nCmd* cmd );
static void n_SetBody( void* slf, nCmd* cmd );
static void n_GetBody( void* slf, nCmd* cmd );
static void n_SetPosition( void* slf, nCmd* cmd );
static void n_SetRotation( void* slf, nCmd* cmd );
static void n_SetQuaternion( void* slf, nCmd* cmd );
static void n_GetPosition( void* slf, nCmd* cmd );
static void n_GetRotation( void* slf, nCmd* cmd );
static void n_GetQuaternion( void* slf, nCmd* cmd );
static void n_GetGeomClassName( void* slf, nCmd* cmd );
static void n_GetAABB( void* slf, nCmd* cmd );
static void n_SetCategoryBits( void* slf, nCmd* cmd );
static void n_SetCollideBits( void* slf, nCmd* cmd );
static void n_GetCategoryBits( void* slf, nCmd* cmd );
static void n_GetCollideBits( void* slf, nCmd* cmd );
static void n_Enable( void* slf, nCmd* cmd );
static void n_Disable( void* slf, nCmd* cmd );
static void n_IsEnabled( void* slf, nCmd* cmd );

//------------------------------------------------------------------------------
/**
    @scriptclass
    nopendegeom
    
    @superclass
    nroot

    @classinfo
    Encapsulates some dGeom functions.
*/
void
n_initcmds(nClass* clazz)
{
    clazz->BeginCmds();
    clazz->AddCmd( "v_Create_s",                    'CREA', n_Create );
    clazz->AddCmd( "v_SetBody_s",                   'SBOD', n_SetBody );
    clazz->AddCmd( "s_GetBody_v",                   'GBOD', n_GetBody );
    clazz->AddCmd( "v_SetPosition_fff",             'SPOS', n_SetPosition );
    clazz->AddCmd( "v_SetRotation_fffffffff",       'SROT', n_SetRotation );
    clazz->AddCmd( "v_SetQuaternion_ffff",          'SQUA', n_SetQuaternion );
    clazz->AddCmd( "fff_GetPosition_v",             'GPOS', n_GetPosition );
    clazz->AddCmd( "fffffffff_GetRotation_v",       'GROT', n_GetRotation );
    clazz->AddCmd( "ffff_GetQuaternion_v",          'GQUA', n_GetQuaternion );
    clazz->AddCmd( "s_GetGeomClassName_v",          'GGCN', n_GetGeomClassName );
    clazz->AddCmd( "ffffff_GetAABB_v",              'GBOX', n_GetAABB );
    clazz->AddCmd( "v_SetCategoryBits_i",           'SCTB', n_SetCategoryBits );
    clazz->AddCmd( "v_SetCollideBits_i",            'SCLB', n_SetCollideBits );
    clazz->AddCmd( "i_GetCategoryBits_v",           'GCTB', n_GetCategoryBits );
    clazz->AddCmd( "i_GetCollideBits_v",            'GCLB', n_GetCollideBits );
    clazz->AddCmd( "v_Enable_v",                    'ENBL', n_Enable );
    clazz->AddCmd( "v_Disable_v",                   'DSBL', n_Disable );
    clazz->AddCmd( "b_IsEnabled_v",                 'IENB', n_IsEnabled );
    clazz->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    Create

    @input
    s(NOH path to an nOpendeWorld instance)

    @output
    v

    @info
    Create the underlying ODE object.
*/
static void 
n_Create( void* slf, nCmd* cmd )
{
    nOpendeGeom* self = (nOpendeGeom*)slf;
    self->Create( cmd->In()->GetS() );
}

//------------------------------------------------------------------------------
/**
    @cmd
    SetBody

    @input
    s(NOH path to an nOpendeBody instance)

    @output
    v

    @info
    Attach the geom to the specified body, can be "none" to indicate the geom
    is independent of any bodies (or to detach it if it is already attached).
*/
static void 
n_SetBody( void* slf, nCmd* cmd )
{
    nOpendeGeom* self = (nOpendeGeom*)slf;
    self->SetBody( cmd->In()->GetS() );
}

//------------------------------------------------------------------------------
/**
    @cmd
    GetBody

    @input
    v

    @output
    s(NOH path to an nOpendeBody instance)

    @info
    Get the body that the geom is attached to, will be "none" if the geom is
    not attached to any body.
*/
static void 
n_GetBody( void* slf, nCmd* cmd )
{
    nOpendeGeom* self = (nOpendeGeom*)slf;
    cmd->Out()->SetS( self->GetBodyName() );
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
*/
static void 
n_SetPosition( void* slf, nCmd* cmd )
{
    nOpendeGeom* self = (nOpendeGeom*)slf;
    vector3 v;
    v.x = cmd->In()->GetF();
    v.y = cmd->In()->GetF();
    v.z = cmd->In()->GetF();
    self->SetPosition( v );
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
*/
static void 
n_SetRotation( void* slf, nCmd* cmd )
{
    nOpendeGeom* self = (nOpendeGeom*)slf;
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
    self->SetRotation( m );
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
*/
static void 
n_SetQuaternion( void* slf, nCmd* cmd )
{
    nOpendeGeom* self = (nOpendeGeom*)slf;
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
    GetPosition

    @input
    v

    @output
    f(x), f(y), f(z)

    @info
*/
static void 
n_GetPosition( void* slf, nCmd* cmd )
{
    nOpendeGeom* self = (nOpendeGeom*)slf;
    vector3 v;
    self->GetPosition( v );
    cmd->Out()->SetF( v.x );
    cmd->Out()->SetF( v.y );
    cmd->Out()->SetF( v.z );
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
*/
static void 
n_GetRotation( void* slf, nCmd* cmd )
{
    nOpendeGeom* self = (nOpendeGeom*)slf;
    matrix33 m;
    self->GetRotation( m );
    cmd->Out()->SetF( m.M11 );
    cmd->Out()->SetF( m.M12 );
    cmd->Out()->SetF( m.M13 );
    cmd->Out()->SetF( m.M21 );
    cmd->Out()->SetF( m.M22 );
    cmd->Out()->SetF( m.M23 );
    cmd->Out()->SetF( m.M31 );
    cmd->Out()->SetF( m.M32 );
    cmd->Out()->SetF( m.M33 );
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
*/
static void 
n_GetQuaternion( void* slf, nCmd* cmd )
{
    nOpendeGeom* self = (nOpendeGeom*)slf;
    quaternion q;
    self->GetQuaternion( q );
    cmd->Out()->SetF( q.x );
    cmd->Out()->SetF( q.y );
    cmd->Out()->SetF( q.z );
    cmd->Out()->SetF( q.w );
}

//------------------------------------------------------------------------------
/**
    @cmd
    GetGeomClassName

    @input
    v

    @output
    s(GeomClassName)

    @info
    GeomClassName will be one of:
        - sphere
        - box
        - capsule
        - cylinder 
        - plane
        - transform
        - ray
        - mesh
*/
static void 
n_GetGeomClassName( void* slf, nCmd* cmd )
{
    nOpendeGeom* self = (nOpendeGeom*)slf;
    cmd->Out()->SetS( self->GetGeomClassName() );
}

//------------------------------------------------------------------------------
/**
    @cmd
    GetAABB

    @input
    v

    @output
    f(aabb[0]), f(aabb[1]), f(aabb[2]), f(aabb[3]), f(aabb[4]), f(aabb[5])

    @info
*/
static void 
n_GetAABB( void* slf, nCmd* cmd )
{
    nOpendeGeom* self = (nOpendeGeom*)slf;
    float aabb[6];
    self->GetAABB( aabb );
    cmd->Out()->SetF( aabb[0] );
    cmd->Out()->SetF( aabb[1] );
    cmd->Out()->SetF( aabb[2] );
    cmd->Out()->SetF( aabb[3] );
    cmd->Out()->SetF( aabb[4] );
    cmd->Out()->SetF( aabb[5] );
}

//------------------------------------------------------------------------------
/**
    @cmd
    SetCategoryBits

    @input
    i(bits)

    @output
    v

    @info
*/
static void 
n_SetCategoryBits( void* slf, nCmd* cmd )
{
    nOpendeGeom* self = (nOpendeGeom*)slf;
    self->SetCategoryBits( ulong(cmd->In()->GetI()) );
}

//------------------------------------------------------------------------------
/**
    @cmd
    SetCollideBits

    @input
    i(bits)

    @output
    v

    @info
*/
static void 
n_SetCollideBits( void* slf, nCmd* cmd )
{
    nOpendeGeom* self = (nOpendeGeom*)slf;
    self->SetCollideBits( ulong(cmd->In()->GetI()) );
}

//------------------------------------------------------------------------------
/**
    @cmd
    GetCategoryBits

    @input
    v

    @output
    i(bits)

    @info
*/
static void 
n_GetCategoryBits( void* slf, nCmd* cmd )
{
    nOpendeGeom* self = (nOpendeGeom*)slf;
    // FIXME: possibly unsafe cast from ulong to int
    cmd->Out()->SetI( int(self->GetCategoryBits()) );
}

//------------------------------------------------------------------------------
/**
    @cmd
    GetCollideBits

    @input
    v

    @output
    i(bits)

    @info
*/
static void 
n_GetCollideBits( void* slf, nCmd* cmd )
{
    nOpendeGeom* self = (nOpendeGeom*)slf;
    // FIXME: possibly unsafe cast from ulong to int
    cmd->Out()->SetI( int(self->GetCollideBits()) );
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
*/
static void 
n_Enable( void* slf, nCmd* cmd )
{
    nOpendeGeom* self = (nOpendeGeom*)slf;
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
*/
static void 
n_Disable( void* slf, nCmd* cmd )
{
    nOpendeGeom* self = (nOpendeGeom*)slf;
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
*/
static void 
n_IsEnabled( void* slf, nCmd* cmd )
{
    nOpendeGeom* self = (nOpendeGeom*)slf;
    cmd->Out()->SetB( self->IsEnabled() );
}

//------------------------------------------------------------------------------
/**
    @param  ps          writes the nCmd object contents out to a file.
    @return             success or failure
*/
bool nOpendeGeom::SaveCmds( nPersistServer* ps )
{
    if ( nRoot::SaveCmds( ps ) )
    {
        nCmd* cmd;
        
        // Create
        cmd = ps->GetCmd( this, 'CREA' );
        if ( this->getSpace() )
            cmd->In()->SetS( this->ref_Space.getname() );
        else
            cmd->In()->SetS( "none" );
        ps->PutCmd( cmd );
    
        // SetBody
        cmd = ps->GetCmd( this, 'SBOD' );
        cmd->In()->SetS( this->bodyName.Get() );
        ps->PutCmd( cmd );
    
        // SetPosition
        cmd = ps->GetCmd( this, 'SPOS' );
        vector3 v;
        this->GetPosition( v );
        cmd->In()->SetF( v.x );
        cmd->In()->SetF( v.y );
        cmd->In()->SetF( v.z );
        ps->PutCmd( cmd );
        
        // SetQuaternion
        cmd = ps->GetCmd( this, 'SQUA' );
        quaternion q;
        this->GetQuaternion( q );
        cmd->In()->SetF( q.x );
        cmd->In()->SetF( q.y );
        cmd->In()->SetF( q.z );
        cmd->In()->SetF( q.w );
        ps->PutCmd( cmd );
        
        // SetCategoryBits
        cmd = ps->GetCmd( this, 'SCTB' );
        cmd->In()->SetI( int(this->GetCategoryBits()) );
        ps->PutCmd( cmd );
        
        // SetCollideBits
        cmd = ps->GetCmd( this, 'SCLB' );
        cmd->In()->SetI( int(this->GetCollideBits()) );
        ps->PutCmd( cmd );
        
        // Enable
        if ( this->IsEnabled() )
            cmd = ps->GetCmd( this, 'ENBL' );
        else
            cmd = ps->GetCmd( this, 'DSBL' );
        ps->PutCmd( cmd );
    
        return true;
    }
    return false;
}
