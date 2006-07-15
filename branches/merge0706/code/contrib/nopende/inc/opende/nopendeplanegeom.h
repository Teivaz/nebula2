#ifndef N_OPENDEPLANEGEOM_H
#define N_OPENDEPLANEGEOM_H
//----------------------------------------------------------------------------
/**
    @class nOpendePlaneGeom
    @ingroup NOpenDEContribModule
    @brief Encapsulates some plane geom functions.
    
    (c) 2004    Vadim Macagon
*/
//----------------------------------------------------------------------------

#include "opende/nopendelayer.h"
#include "opende/nopendegeom.h"

//----------------------------------------------------------------------------
class nOpendePlaneGeom : public nOpendeGeom
{
    public:
        /// constructor
        nOpendePlaneGeom();
        /// destructor
        virtual ~nOpendePlaneGeom();
        /// persistency
        virtual bool SaveCmds( nPersistServer* );
        
        /// create the corresponding ODE object
        virtual void Create( const char* space );
                
        /*
            Convenience methods.
        */
        
        void SetParams( float a, float b, float c, float d );
        void GetParams( vector4& result );
        float PointDepth( const vector3& );
};

//----------------------------------------------------------------------------
#endif // N_OPENDEPLANEGEOM_H
