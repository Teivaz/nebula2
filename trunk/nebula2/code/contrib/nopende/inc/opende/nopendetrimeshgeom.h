#ifndef N_OPENDETRIMESHGEOM_H
#define N_OPENDETRIMESHGEOM_H
//----------------------------------------------------------------------------
/**
    @class nOpendeTriMeshGeom
    @ingroup NOpenDEContribModule
    @brief Encapsulates some trimesh geom functions.
    
    (c) 2004    Vadim Macagon
*/
//----------------------------------------------------------------------------
#include "opende/nopendelayer.h"
#include "opende/nopendegeom.h"

class nOpendeTriMesh;
//----------------------------------------------------------------------------
class nOpendeTriMeshGeom : public nOpendeGeom
{
    public:
        /// constructor
        nOpendeTriMeshGeom();
        /// destructor
        virtual ~nOpendeTriMeshGeom();
        /// persistency
        virtual bool SaveCmds( nPersistServer* );
        /// create the corresponding ODE object
        virtual void Create( const char* space = "none" );
        
        /// specify the actual tri mesh to use
        void SetMesh( const char* triMeshId );
        /// get the actual tri mesh that is used
        const char* GetMesh();
        
        /*
            Convenience methods.
        */
        
        void SetCallback( dTriCallback* callback );
        dTriCallback* GetCallback();
        void SetArrayCallback( dTriArrayCallback* arrayCallback );
        dTriArrayCallback* GetArrayCallback();
        void SetRayCallback( dTriRayCallback* callback );
        dTriRayCallback* GetRayCallback();
        void SetData( dTriMeshDataID data );
        
        void EnableTC( int geomClass, bool enable );
        void EnableTC( const char* geomClass, bool enable );
        bool IsTCEnabled( int geomClass );
        bool IsTCEnabled( const char* geomClass );
        void ClearTCCache();
        void GetTriangle( int index, vector3& v0, vector3& v1, vector3& v2 );
        void GetPoint( int index, float u, float v, vector3& out );
            
    private:
        nOpendeTriMesh* triMesh;
};

//----------------------------------------------------------------------------
#endif // N_OPENDETRIMESHGEOM_H
