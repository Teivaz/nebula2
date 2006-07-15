#ifndef N_OPENDETRANSFORMGEOM_H
#define N_OPENDETRANSFORMGEOM_H
//----------------------------------------------------------------------------
/**
    @class nOpendeTransformGeom
    @ingroup NOpenDEContribModule
    @brief Encapsulates some transform geom functions.
    
    (c) 2004    Vadim Macagon
*/
//----------------------------------------------------------------------------

#include "opende/nopendelayer.h"
#include "opende/nopendegeom.h"

//----------------------------------------------------------------------------
class nOpendeTransformGeom : public nOpendeGeom
{
    public:
        /// constructor
        nOpendeTransformGeom();
        /// destructor
        virtual ~nOpendeTransformGeom();
        /// persistency
        virtual bool SaveCmds( nPersistServer* );
        
        /// create the corresponding ODE object
        virtual void Create( const char* space = "none" );
                
        /*
            Convenience methods.
        */
        
        void SetGeom( dGeomID );
        dGeomID GetGeom();
        void SetGeom( const char* );
        const char* GetGeomName();
        void SetCleanup( bool mode );
        bool GetCleanup();
        void SetInfo( int mode );
        int GetInfo();
            
    private:
        nString geomName;
};

//----------------------------------------------------------------------------
#endif // N_OPENDETRANSFORMGEOM_H
