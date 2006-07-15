#ifndef N_OPENDEGEOM_H
#define N_OPENDEGEOM_H
//----------------------------------------------------------------------------
/**
    @class nOpendeGeom
    @ingroup NOpenDEContribModule
    @brief Encapsulates some dGeom functions.
    
    (c) 2004    Vadim Macagon
*/
//----------------------------------------------------------------------------

#include "kernel/nroot.h"
#include "kernel/ndynautoref.h"
#include "opende/nopendelayer.h"

class nOpendeSpace;
class nOpendeBody;
//----------------------------------------------------------------------------
class nOpendeGeom : public nRoot
{
    public:
        /// constructor
        nOpendeGeom();
        /// destructor
        virtual ~nOpendeGeom();
        /// persistency
        virtual bool SaveCmds( nPersistServer* );
        /// create the corresponding ODE object
        virtual void Create( const char* space = "none" );
    
        dGeomID id() const;
                        
        /*
            Convenience methods.
        */
        
        void SetData( void* data );
        void* GetData();
        // get type info
        int GetGeomClass();
        const char* GetGeomClassName();
        
        dSpaceID GetSpace();
        void GetAABB( float aabb[6] );
        void SetCategoryBits( ulong );
        void SetCollideBits( ulong );
        ulong GetCategoryBits();
        ulong GetCollideBits();
        void Enable();
        void Disable();
        bool IsEnabled();
        
        // attach/detach to/from body
        void SetBody( dBodyID );
        dBodyID GetBody();
        void SetBody( const char* );
        const char* GetBodyName();
        
        void SetPosition( const vector3& );
        void SetRotation( const matrix33& );
        void SetQuaternion( const quaternion& );
        void GetPosition( vector3& );
        void GetRotation( matrix33& );
        void GetQuaternion( quaternion& );
        
        void Collide2( dGeomID other, void* data, dNearCallback* callback );

    protected:
        dSpaceID getSpace();
        
        /// the actual ODE object (do NOT destroy!)
        dGeomID geomId;
    
    private:
        nDynAutoRef<nOpendeSpace> ref_Space;
        nString bodyName;
};

//----------------------------------------------------------------------------
inline
dGeomID nOpendeGeom::id() const
{ 
    return this->geomId;
}

//----------------------------------------------------------------------------
#endif // N_OPENDEGEOM_H
