#ifndef N_OPENDERAYGEOM_H
#define N_OPENDERAYGEOM_H
//----------------------------------------------------------------------------
/**
    @class nOpendeRayGeom
    @ingroup NOpenDEContribModule
    @brief Encapsulates some ray geom functions.
    
    (c) 2004    Vadim Macagon
*/
//----------------------------------------------------------------------------

#include "opende/nopendelayer.h"
#include "opende/nopendegeom.h"

//----------------------------------------------------------------------------
class nOpendeRayGeom : public nOpendeGeom
{
    public:
        /// constructor
        nOpendeRayGeom();
        /// destructor
        virtual ~nOpendeRayGeom();
        /// persistency
        virtual bool SaveCmds( nPersistServer* );
        
        /// create the corresponding ODE object
        virtual void Create( const char* space = "none" );
                
        /*
            Convenience methods.
        */
        
        void SetLength( float );
        float GetLength();
        void Set( const vector3& pos, const vector3& dir );
        void Get( vector3& start, vector3& dir );
        void SetParams( bool firstContact, bool backfaceCull );
        void GetParams( bool* firstContact, bool* backfaceCull );
        void SetClosestHit( bool );
        bool GetClosestHit();
};

//----------------------------------------------------------------------------
#endif // N_OPENDERAYGEOM_H
