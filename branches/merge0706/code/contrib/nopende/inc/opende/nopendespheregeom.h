#ifndef N_OPENDESPHEREGEOM_H
#define N_OPENDESPHEREGEOM_H
//----------------------------------------------------------------------------
/**
    @class nOpendeSphereGeom
    @ingroup NOpenDEContribModule
    @brief Encapsulates some sphere geom functions.
    
    (c) 2004    Vadim Macagon
*/
//----------------------------------------------------------------------------

#include "opende/nopendelayer.h"
#include "opende/nopendegeom.h"

//----------------------------------------------------------------------------
class nOpendeSphereGeom : public nOpendeGeom
{
    public:
        /// constructor
        nOpendeSphereGeom();
        /// destructor
        virtual ~nOpendeSphereGeom();
        /// persistency
        virtual bool SaveCmds( nPersistServer* );
        
        /// create the corresponding ODE object
        virtual void Create( const char* space = "none" );
                
        /*
            Convenience methods.
        */
        
        void SetRadius( float );
        float GetRadius();
        float PointDepth( const vector3& );
};

//----------------------------------------------------------------------------
#endif // N_OPENDESPHEREGEOM_H
