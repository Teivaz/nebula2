#ifndef N_OPENDECAPSULEGEOM_H
#define N_OPENDECAPSULEGEOM_H
//----------------------------------------------------------------------------
/**
    @class nOpendeCapsuleGeom
    @ingroup NOpenDEContribModule
    @brief Encapsulates some ccylinder geom functions.
    
    (c) 2004    Vadim Macagon
*/
//----------------------------------------------------------------------------

#include "opende/nopendelayer.h"
#include "opende/nopendegeom.h"

//----------------------------------------------------------------------------
class nOpendeCapsuleGeom : public nOpendeGeom
{
    public:
        /// constructor
        nOpendeCapsuleGeom();
        /// destructor
        virtual ~nOpendeCapsuleGeom();
        /// persistency
        virtual bool SaveCmds( nPersistServer* );
        
        /// create the corresponding ODE object
        virtual void Create( const char* space = "none" );
                
        /*
            Convenience methods.
        */
        
        void SetParams( float radius, float length );
        void GetParams( float* radius, float* length );
        float PointDepth( const vector3& );
};

//----------------------------------------------------------------------------
#endif // N_OPENDECAPSULEGEOM_H
