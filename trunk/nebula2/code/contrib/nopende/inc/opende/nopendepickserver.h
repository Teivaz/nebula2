#ifndef N_OPENDEPICKSERVER_H
#define N_OPENDEPICKSERVER_H
//------------------------------------------------------------------------------
/**
    @class nOpendePickServer
    @ingroup NOpenDEContribModule
    @brief A rather simple class to help with ray-picking.

    (C) 2004  Vadim Macagon
    
    nOpendePickServer is licensed under the terms of the Nebula License.
*/
//------------------------------------------------------------------------------

#include "kernel/nroot.h"
#include "kernel/ndynautoref.h"
#include "mathlib/line.h"
#include "opende/nopendelayer.h"

class nOpendeSpace;
class nOpendeTriMesh;
class nGfxServer2;
//------------------------------------------------------------------------------
class nOpendePickServer : public nRoot
{
    public:
        enum RayPickFlag { 
            FirstContact = (1<<0), /**< Only applies to ray vs. trimesh
                                        collision queries. 
                                        Presence of this flag indicates
                                        that OPCODE will stop a collision
                                        query as soon as the first contact is
                                        found. Lack of this flag means OPCODE
                                        will work in All Contacts mode and
                                        the collision query continues until 
                                        all triangles (in a trimesh) that 
                                        intersect with the ray have been found.
                                   */
            ClosestHit   = (1<<1), /**< Only applies to ray vs. trimesh
                                        collision queries, in All Contacts mode.
                                        OPCODE will sort TriMesh faces by 
                                        distance on-the-fly and only the closest 
                                        face (to the ray origin) will be reported.
                                   */
            BackfaceCull = (1<<2), /**< Only applies to ray vs. trimesh
                                        collision queries. If this flag is 
                                        present the ray will only hit front faces,
                                        back faces of the TriMesh will be ignored.
                                    */
            
            PickAll      = (1<<3), /**< Used to indicate that all 
                                        intersections of the ray with geoms 
                                        (up to the max that can be stored in 
                                        the provided contacts array) should be 
                                        found and returned 
                                        (in no particular order!). 
                                   */
            PickClosest  = (1<<4) /**< Used to indicate that only the contact 
                                       information for the intersection that is 
                                       closest to the ray origin should be 
                                       returned.
                                  */
        };
    
        /// constructor
        nOpendePickServer();
        /// destructor
        virtual ~nOpendePickServer();
        
        /// generate a pick ray from the specified screen coordinates
        void CreatePickRay( int mouseX, int mouseY, line3& pickRay );
        /// generate a pick ray from the specified screen coordinates
        void CreatePickRay( float mouseX, float mouseY, line3& pickRay );
        
        void RayPick( dSpaceID, void* data, dNearCallback*, 
                      const line3&, int flags );
                      
        int RayPickSimple( dSpaceID, const line3&, int maxContacts,
                           dContactGeom* contacts, int skip, int flags );
                
    private:
        static void PickClosestCallback( void* data, dGeomID g1, dGeomID g2 );
        void PickClosestIntersect( dGeomID ray, dGeomID geom );
    
        dGeomID rayGeom;
        dContactGeom* contactArray;
        nAutoRef<nGfxServer2> ref_GfxServer;
};

//------------------------------------------------------------------------------
#endif // N_OPENDEPICKSERVER
