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
//------------------------------------------------------------------------------
class nOpendePickServer : public nRoot
{
    public:
        enum RayPickFlag { 
            FirstContact = (1<<0), 
            ClosestHit   = (1<<1),
            BackfaceCull = (1<<2),
            
            PickAll      = (1<<3),
            PickClosest  = (1<<4)
        };
    
        /// constructor
        nOpendePickServer();
        /// destructor
        virtual ~nOpendePickServer();
    
        
        void RayPick( dSpaceID, void* data, dNearCallback*, 
                      const line3&, int flags );
                      
        int RayPickSimple( dSpaceID, const line3&, int maxContacts,
                           dContactGeom* contacts, int skip, int flags );
                
           
        static nKernelServer* kernelServer;
    
    private:
        static void PickClosestCallback( void* data, dGeomID g1, dGeomID g2 );
        void PickClosestIntersect( dGeomID ray, dGeomID geom );
    
        dGeomID rayGeom;
        dContactGeom* contactArray;
};

//------------------------------------------------------------------------------
#endif // N_OPENDEPICKSERVER
