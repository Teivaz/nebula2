//----------------------------------------------------------------------------
//  nopendepickserver_main.cc
//
//  (c) 2004  Vadim Macagon
//
//  nOpendePickServer is licensed under the terms of the Nebula License.
//----------------------------------------------------------------------------

#include "kernel/nkernelserver.h"
#include "opende/nopendepickserver.h"
nNebulaClass(nOpendePickServer, "nroot");

#include "opende/nopendetrimesh.h"
#include "gfx2/ngfxserver2.h"

//----------------------------------------------------------------------------
/**
*/
nOpendePickServer::nOpendePickServer() :
    contactArray(0),
    ref_GfxServer("/sys/servers/gfx")
{
    this->rayGeom = nOpende::CreateRay( 0, 1 );
}

//----------------------------------------------------------------------------
/**
*/
nOpendePickServer::~nOpendePickServer()
{
    if ( this->rayGeom )
    {
        nOpende::GeomDestroy( this->rayGeom );
        this->rayGeom = 0;
    }
}

//----------------------------------------------------------------------------
/**
    @param [in] mouseX   Absolute x coordinate of cursor in pixels.
    @param [in] mouseY   Absolute y coordinate of cursor in pixels.
    @param [out] pickRay The generated pick ray.
*/
void nOpendePickServer::CreatePickRay( int mouseX, int mouseY, line3& pickRay )
{
    n_assert( this->ref_GfxServer.isvalid() );
    
    vector3 start, end;
        
    const nDisplayMode2& displayMode = this->ref_GfxServer->GetDisplayMode();
    nCamera2& camera = this->ref_GfxServer->GetCamera();
    
    if ( nCamera2::Perspective == camera.GetType() )
    {
        float nearp, farp, minx, maxx, miny, maxy;
        camera.GetViewVolume( minx, maxx, miny, maxy, nearp, farp );
        // normalize screen coords to [-1.0f, 1.0f]
        start.x = float(mouseX) / (displayMode.GetWidth() * 0.5f) - 1.0f;
        start.y = 1.0f - float(mouseY) / (displayMode.GetHeight() * 0.5f);
        // map the normalized coords to the near plane
        start.x *= maxx;
        start.y *= maxy;
        start.z = -nearp;
        // the ratio of the far plane to the near plane is the same as
        // the ratio of the x/y coord on the far plane to the x/y coord 
        // on the near plane
        float planeRatio = farp / nearp;
        // map the coords from the near plane to the far plane
        end.x = start.x * planeRatio;
        end.y = start.y * planeRatio;
        end.z = -farp;
        // transform ray endpoints to world space and create the ray
        matrix44 mat = this->ref_GfxServer->GetTransform( nGfxServer2::InvView );
        pickRay.set( mat * start, mat * end );
    }
}
  
//----------------------------------------------------------------------------
/**
    @param [in]  mouseX  Normalized x coordinate of cursor (-1.0 to 1.0)
    @param [in]  mouseY  Normalized y coordinate of cursor (-1.0 to 1.0)
    @param [out] pickRay The generated pick ray.
*/      
void nOpendePickServer::CreatePickRay( float mouseX, float mouseY, 
                                       line3& pickRay )
{
    n_assert( this->ref_GfxServer.isvalid() );
    
    vector3 start, end;
        
    const nDisplayMode2& displayMode = this->ref_GfxServer->GetDisplayMode();
    nCamera2& camera = this->ref_GfxServer->GetCamera();
    
    if ( nCamera2::Perspective == camera.GetType() )
    {
        float nearp, farp, minx, maxx, miny, maxy;
        camera.GetViewVolume( minx, maxx, miny, maxy, nearp, farp );
        // map the normalized coords to the near plane
        start.x *= maxx;
        start.y *= maxy;
        start.z = -nearp;
        // the ratio of the far plane to the near plane is the same as
        // the ratio of the x/y coord on the far plane to the x/y coord 
        // on the near plane
        float planeRatio = farp / nearp;
        // map the coords from the near plane to the far plane
        end.x = start.x * planeRatio;
        end.y = start.y * planeRatio;
        end.z = -farp;
        // transform ray endpoints to world space and create the ray
        matrix44 mat = this->ref_GfxServer->GetTransform( nGfxServer2::InvView );
        pickRay.set( mat * start, mat * end );
    }
}

//----------------------------------------------------------------------------
/**
    @brief
    @param space
    @param data
    @param callback
    @param line
    @param flags
*/
void nOpendePickServer::RayPick( dSpaceID space, void* data, 
                                 dNearCallback* callback,
                                 const line3& line, int flags )
{
    // setup ray geom from line
    nOpende::GeomRaySet( this->rayGeom, line.b.x, line.b.y, line.b.z,
                         line.m.x, line.m.y, line.m.z );
    nOpende::GeomRaySetLength( this->rayGeom, line.len() );
    int firstContact = (flags & FirstContact) ? 1:0;
    int backfaceCull = (flags & BackfaceCull) ? 1:0;
    nOpende::GeomRaySetParams( this->rayGeom, firstContact, backfaceCull );
    nOpende::GeomRaySetClosestHit( this->rayGeom, (flags & ClosestHit) != 0 );
        
    // do instersection testing
    nOpende::SpaceCollide2( this->rayGeom, (dGeomID)space, data, callback );
}

//----------------------------------------------------------------------------
/**
    @brief
    @param space
    @param line
    @param maxContacts
    @param contacts
    @param skip
    @param flags
    @return Number of contacts found.
*/
int nOpendePickServer::RayPickSimple( dSpaceID space, const line3& line,
                                      int maxContacts, dContactGeom* contacts, 
                                      int skip, int flags )
{
    n_assert( (maxContacts > 0) && contacts );
    
    // setup ray geom from line
    nOpende::GeomRaySet( this->rayGeom, line.b.x, line.b.y, line.b.z,
                         line.m.x, line.m.y, line.m.z );
    nOpende::GeomRaySetLength( this->rayGeom, line.len() );
    int firstContact = (flags & FirstContact) ? 1:0;
    int backfaceCull = (flags & BackfaceCull) ? 1:0;
    nOpende::GeomRaySetParams( this->rayGeom, firstContact, backfaceCull );
    nOpende::GeomRaySetClosestHit( this->rayGeom, (flags & ClosestHit) != 0 );
    
    // do instersection testing
    if ( flags & PickAll )
    {
        return nOpende::Collide( this->rayGeom, (dGeomID)space, 
                                 maxContacts, contacts, skip );
    }
    else if ( flags & PickClosest )
    {
        this->contactArray = contacts;
        // indicate no contact has been found yet
        this->contactArray[0].g2 = 0;
        // We don't want to be limited by maxContacts when looking
        // for the closest geom (or we may not find the closest one!)
        nOpende::SpaceCollide2( this->rayGeom, (dGeomID)space, this, 
                                &PickClosestCallback );
        int retVal = (0 == this->contactArray[0].g2) ? 0:1;
        this->contactArray = 0;
        return retVal;
    }
    
    return 0;
}

//----------------------------------------------------------------------------
/**
*/
void nOpendePickServer::PickClosestCallback( void* p, dGeomID g1, dGeomID g2 )
{
    if ( nOpende::GeomGetClass( g1 ) == dRayClass )
        ((nOpendePickServer*)p)->PickClosestIntersect( g1, g2 );
    else
        ((nOpendePickServer*)p)->PickClosestIntersect( g2, g1 );
}

//------------------------------------------------------------------------------
/**
*/
void nOpendePickServer::PickClosestIntersect( dGeomID ray, dGeomID geom )
{
    dContactGeom contactGeom;
  
    // generate at most 1 contact
    int numContacts = nOpende::Collide( ray, geom, 1, 
                                        &contactGeom, 
                                        sizeof(dContactGeom) );
  
    // store contact only if it's closer than the last one
    if ( numContacts > 0 )
    {
        if ( this->contactArray[0].g2 )
        {
            // only store new contact if it's closer to the ray origin
            if ( contactGeom.depth < this->contactArray[0].depth )
            {
                memcpy( this->contactArray, &contactGeom, 
                        sizeof(dContactGeom) );
            }
        }
        else // this is the first contact generated
            memcpy( this->contactArray, &contactGeom, sizeof(dContactGeom) );
    }
}
