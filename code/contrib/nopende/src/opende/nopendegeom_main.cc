//----------------------------------------------------------------------------
//  (c) 2004    Vadim Macagon
//----------------------------------------------------------------------------
#include "opende/nopendegeom.h"

nNebulaScriptClass(nOpendeGeom, "nroot");

#include "opende/nopendespace.h"
#include "opende/nopendebody.h"

//----------------------------------------------------------------------------
/**
*/
nOpendeGeom::nOpendeGeom() :
    geomId(0),
    bodyName("none")
{
    //
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeGeom::Create( const char* space )
{
    n_assert( space );
    n_assert( 0 == this->id() );
    this->ref_Space = space;
    // geom will be created in subclass
}

//----------------------------------------------------------------------------
/**
*/
nOpendeGeom::~nOpendeGeom()
{
    if ( this->id() )
    {
        nOpende::GeomDestroy( this->id() );
        this->geomId = 0;
    }
}

//----------------------------------------------------------------------------
/**
*/
dSpaceID nOpendeGeom::getSpace()
{
    if ( strcmp( this->ref_Space.getname(), "none" ) == 0 )
        return 0;
    else
    {
        n_assert( this->ref_Space.isvalid() );
        n_assert( ((nRoot*)this->ref_Space.get())->
                  IsA( this->kernelServer->FindClass( "nopendespace" ) ) );
        return this->ref_Space->id();
    }
}

//----------------------------------------------------------------------------
/**
    @brief Attach the geom to a body.

    Note that using this method the connection will not be persisted, if
    you want the connection to be persistent use the string based AttachTo().
*/
void nOpendeGeom::SetBody( dBodyID body )
{
    n_assert( this->id() && "nOpendeGeom::id not valid!" );
    nOpende::GeomSetBody( this->id(), body );
    this->bodyName = "none";
}

//----------------------------------------------------------------------------
/**
    @brief Attach the geom to a body.

    @param body NOH path to an nOpendeBody instance, can be "none" to indicate
                the geom should be independent from any body (or to detach it
                from a body if it is currently attached).
*/
void nOpendeGeom::SetBody( const char* body )
{
    n_assert( this->id() && "nOpendeGeom::id not valid!" );

    if ( strcmp( body, "none" ) != 0 )
    {
        nRoot* temp = this->kernelServer->Lookup( body );
        n_assert( temp &&
                  temp->IsA( this->kernelServer->FindClass( "nopendebody" ) ) &&
                  "body in call to nOpendeGeom::SetBody() is invalid!" );
        nOpende::GeomSetBody( this->id(), ((nOpendeBody*)temp)->id );
    }

    this->bodyName = body;
}

//----------------------------------------------------------------------------
/**
*/
dBodyID nOpendeGeom::GetBody()
{
    n_assert( this->id() && "nOpendeGeom::id not valid!" );
    return nOpende::GeomGetBody( this->id() );
}

//----------------------------------------------------------------------------
/**
*/
const char* nOpendeGeom::GetBodyName()
{
    n_assert( this->id() && "nOpendeGeom::id not valid!" );
    return this->bodyName.Get();
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeGeom::SetData( void* data )
{
    n_assert( this->id() && "nOpendeGeom::id not valid!" );
    nOpende::GeomSetData( this->id(), data );
}

//----------------------------------------------------------------------------
/**
*/
void* nOpendeGeom::GetData()
{
    n_assert( this->id() && "nOpendeGeom::id not valid!" );
    return nOpende::GeomGetData( this->id() );
}

//----------------------------------------------------------------------------
/**
    @return One of the return values from dGeomGetClass().
*/
int nOpendeGeom::GetGeomClass()
{
    n_assert( this->id() && "nOpendeGeom::id not valid!" );
    return nOpende::GeomGetClass( this->id() );
}

//----------------------------------------------------------------------------
/**
    @brief Get a string representation of the geom class.
    @return One of the following (value / notes):
        - sphere
        - box
        - capsule / corresponds to dCCylinderClass.
        - cylinder
        - plane
        - transform / corresponds to dGeomTransformClass.
        - ray
        - mesh
*/
const char* nOpendeGeom::GetGeomClassName()
{
    n_assert( this->id() && "nOpendeGeom::id not valid!" );

    switch ( nOpende::GeomGetClass( this->id() ) )
    {
        case dSphereClass:
            return "sphere";
        case dBoxClass:
            return "box";
        case dCCylinderClass:
            return "capsule";
        case dCylinderClass:
            return "cylinder";
        case dPlaneClass:
            return "plane";
        case dGeomTransformClass:
            return "transform";
        case dRayClass:
            return "ray";
        case dTriMeshClass:
            return "mesh";
        default:
            n_assert( "Flying monkies!!!" ); // should never get here
            return "uknown";
    }
}

//----------------------------------------------------------------------------
dSpaceID nOpendeGeom::GetSpace()
{
    return nOpende::GeomGetSpace( this->id() );
}

//----------------------------------------------------------------------------
void nOpendeGeom::GetAABB( float aabb[6] )
{
    n_assert( this->id() && "nOpendeGeom::id not valid!" );
    nOpende::GeomGetAABB( this->id(), aabb );
}

//----------------------------------------------------------------------------
void nOpendeGeom::SetCategoryBits( ulong bits )
{
    n_assert( this->id() && "nOpendeGeom::id not valid!" );
    nOpende::GeomSetCategoryBits( this->id(), bits );
}

//----------------------------------------------------------------------------
void nOpendeGeom::SetCollideBits( ulong bits )
{
    n_assert( this->id() && "nOpendeGeom::id not valid!" );
    nOpende::GeomSetCollideBits( this->id(), bits );
}

//----------------------------------------------------------------------------
ulong nOpendeGeom::GetCategoryBits()
{
    n_assert( this->id() && "nOpendeGeom::id not valid!" );
    return nOpende::GeomGetCategoryBits( this->id() );
}

//----------------------------------------------------------------------------
ulong nOpendeGeom::GetCollideBits()
{
    n_assert( this->id() && "nOpendeGeom::id not valid!" );
    return nOpende::GeomGetCollideBits( this->id() );
}

//----------------------------------------------------------------------------
void nOpendeGeom::Enable()
{
    n_assert( this->id() && "nOpendeGeom::id not valid!" );
    nOpende::GeomEnable( this->id() );
}

//----------------------------------------------------------------------------
void nOpendeGeom::Disable()
{
    n_assert( this->id() && "nOpendeGeom::id not valid!" );
    nOpende::GeomDisable( this->id() );
}

//----------------------------------------------------------------------------
bool nOpendeGeom::IsEnabled()
{
    n_assert( this->id() && "nOpendeGeom::id not valid!" );
    return nOpende::GeomIsEnabled( this->id() );
}

//----------------------------------------------------------------------------
void nOpendeGeom::SetPosition( const vector3& p )
{
    n_assert( this->id() && "nOpendeGeom::id not valid!" );
    nOpende::GeomSetPosition( this->id(), p );
}

//----------------------------------------------------------------------------
void nOpendeGeom::SetRotation( const matrix33& r )
{
    n_assert( this->id() && "nOpendeGeom::id not valid!" );
    nOpende::GeomSetRotation( this->id(), r );
}

//----------------------------------------------------------------------------
void nOpendeGeom::SetQuaternion( const quaternion& q )
{
    n_assert( this->id() && "nOpendeGeom::id not valid!" );
    nOpende::GeomSetQuaternion( this->id(), q );
}

//----------------------------------------------------------------------------
void nOpendeGeom::GetPosition( vector3& p )
{
    n_assert( this->id() && "nOpendeGeom::id not valid!" );
    p = nOpende::GeomGetPosition( this->id() );
}

//----------------------------------------------------------------------------
void nOpendeGeom::GetRotation( matrix33& r )
{
    n_assert( this->id() && "nOpendeGeom::id not valid!" );
    nOpende::GeomGetRotation( this->id(), r );
}

//----------------------------------------------------------------------------
void nOpendeGeom::GetQuaternion( quaternion& q )
{
    n_assert( this->id() && "nOpendeGeom::id not valid!" );
    nOpende::GeomGetQuaternion( this->id(), q );
}

//----------------------------------------------------------------------------
/**
    @brief Generate contact information between two potentially intersecting
           geoms.

    This is really nothing more than a "shortcut" for calling
    dSpaceCollide2( this->id, other, data, callback );
*/
void nOpendeGeom::Collide2( dGeomID other, void* data, dNearCallback* callback )
{
    n_assert( this->id() && "nOpendeGeom::id not valid!" );
    nOpende::SpaceCollide2( this->id(), other, data, callback );
}

//----------------------------------------------------------------------------
// EOF
//----------------------------------------------------------------------------
