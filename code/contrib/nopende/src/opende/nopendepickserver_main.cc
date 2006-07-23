//----------------------------------------------------------------------------
//  nopendepickserver_main.cc
//
//  (c) 2004  Vadim Macagon
//
//  nOpendePickServer is licensed under the terms of the Nebula License.
//----------------------------------------------------------------------------

#include "kernel/nkernelserver.h"
#include "opende/nopendepickserver.h"
nNebulaClass(nOpendePickServer, "kernel::nroot");

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
    @param mouseX   [in] Absolute x coordinate of cursor in pixels.
    @param mouseY   [in] Absolute y coordinate of cursor in pixels.
    @param pickRay  [out] The generated pick ray.

    You'd typically obtain the mouseX/mouseY from the input server like so:

    @code
    int mouseX = 0;
    int mouseY = 0;
    nInputEvent* event = inputServer->FirstEvent();
    while (event)
    {
        if (N_INPUT_MOUSE_MOVE == event->GetType())
        {
                mouseX = event->GetAbsXPos();
                mouseY = event->GetAbsYPos();
        }

        event = inputServer->NextEvent(event);
    }
    @endcode
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
    @param mouseX  [in] x coordinate of cursor (0.0 to 1.0)
    @param mouseY  [in] y coordinate of cursor (0.0 to 1.0)
    @param pickRay [out] The generated pick ray.

    You'd typically obtain the mouseX/mouseY from the input server like so:

    @code
    int mouseX = 0;
    int mouseY = 0;
    nInputEvent* event = inputServer->FirstEvent();
    while (event)
    {
        if (N_INPUT_MOUSE_MOVE == event->GetType())
        {
                mouseX = event->GetRelXPos();
                mouseY = event->GetRelYPos();
        }

        event = inputServer->NextEvent(event);
    }
    @endcode
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
        // normalize the mouse coords to (-1, 1)
        start.x = 2.0f * mouseX - 1.0f;
        start.y = 1.0f - 2.0f * mouseY;
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
    @brief Do user controlled ray picking.
    @param space Space containing geoms that should be tested against the pick
                 ray.
    @param data User defined data.
    @param callback User defined callback that will be called for every
                    pair of (pick ray, geom).
    @param line The pick ray.
    @param flags Can be any combination (OR'ed together) of the FirstContact,
                 BackfaceCull or ClosestHit members of the RayPickFlag enum,
                 though ClosestHit only works if FirstContact is not specified.

    You should use this method only when RayPickSimple() can't achieve what
    you want.
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
    @brief Do ray picking.
    @param space Space containing geoms that should be tested against the pick
                 ray.
    @param line The pick ray.
    @param maxContacts Maximum number of elements that can fit in the contacts
                       array.
    @param contacts Array of dContactGeoms.
    @param skip Typically should be sizeof(dContactGeom), see documentation
                for dCollide for further info (section 10.5.2 in the OpenDE
                manual).
    @param flags A valid combination (OR'ed together) of the the members of
                 the RayPickFlag enum. A valid combination must consist of:
                 - Any combination of FirstContact, BackfaceCull or ClosestHit,
                   though ClosestHit only works if FirstContact is not specified.
                 - Exactly one of PickAll, or PickClosest.
                 .
                 Valid examples include:<br>
                 -# (nOpendePickServer::FirstContact | nOpendePickServer::PickClosest)
                 -# (nOpendePickServer::ClosestHit | nOpendePickServer::PickClosest)
                 -# (nOpendePickServer::PickAll)
                 .
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
