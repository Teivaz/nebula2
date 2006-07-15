#ifndef N_OPENDESPACE_H
#define N_OPENDESPACE_H
//----------------------------------------------------------------------------
/**
    @class nOpendeSpace
    @ingroup NOpenDEContribModule
    @brief Encapsulates some dSpace functions.
    
    (c) 2004    Vadim Macagon
*/
//----------------------------------------------------------------------------

#include "opende/nopendelayer.h"
#include "opende/nopendegeom.h"

//----------------------------------------------------------------------------
class nOpendeSpace : public nOpendeGeom
{
    public:
        /// constructor
        nOpendeSpace();
        /// destructor
        virtual ~nOpendeSpace();
        /// persistency
        virtual bool SaveCmds( nPersistServer* );
                    
        dSpaceID id() const;
                    
        /*
            Convenience methods.
        */
                
        void SetCleanup( bool mode );
        bool GetCleanup();
        void AddGeom( dGeomID );
        void RemoveGeom( dGeomID );
        bool ContainsGeom( dGeomID );
        void AddSpace( dSpaceID );
        void RemoveSpace( dSpaceID );
        bool ContainsSpace( dSpaceID );
        void AddGeom( const char* );
        void RemoveGeom( const char* );
        bool ContainsGeom( const char* );
        int GetNumGeoms();
        dGeomID GetGeom( int i );
        
        void Collide( void* data, dNearCallback* callback );
};

//----------------------------------------------------------------------------
inline
dSpaceID nOpendeSpace::id() const
{
    return dSpaceID(this->geomId);
}

//----------------------------------------------------------------------------
#endif // N_OPENDESPACE_H
