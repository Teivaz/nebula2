#ifndef N_OPENDEBOXGEOM_H
#define N_OPENDEBOXGEOM_H
//----------------------------------------------------------------------------
/**
    @class nOpendeBoxGeom
    @ingroup NOpenDEContribModule
    @brief Encapsulates some box geom functions.
    
    (c) 2004    Vadim Macagon
*/
//----------------------------------------------------------------------------

#include "opende/nopendelayer.h"
#include "opende/nopendegeom.h"

//----------------------------------------------------------------------------
class nOpendeBoxGeom : public nOpendeGeom
{
    public:
        /// constructor
        nOpendeBoxGeom();
        /// destructor
        virtual ~nOpendeBoxGeom();
        /// persistency
        virtual bool SaveCmds( nPersistServer* );
        
        /// create the corresponding ODE object
        virtual void Create( const char* space = "none" );
                
        /*
            Convenience methods.
        */
        
        void SetLengths( float lx, float ly, float lz );
        void GetLengths( vector3& );
        float PointDepth( const vector3& );
};

//----------------------------------------------------------------------------
#endif // N_OPENDEBOXGEOM_H
