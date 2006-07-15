#ifndef N_OPENDESERVER_H
#define N_OPENDESERVER_H
//------------------------------------------------------------------------------
/**
    @class nOpendeServer
    @ingroup NOpenDEContribModule
    @brief A physics and a collision server rolled into one.

    This is a wrapper for ODE, it will provide some additional functionality
    that will simplify some things a bit (hopefully). 
    
    The server also maintains a collection of nOdeSurface(s), the idea being
    that you can define frequently used surface parameters via script on startup
    and re-use them over and over during the contact handling phase. The server
    also provides a rudimentary mechanism for retrieving surface parameters
    when objects of different materials come into contact.

    (C) 2004  Vadim Macagon
    
    nOpendeServer is licensed under the terms of the Nebula License.
*/
//------------------------------------------------------------------------------

#include "opende/nopendesurface.h"
#include "kernel/nroot.h"
#include "util/nhashlist.h"

class nOpendeTriMesh;
//------------------------------------------------------------------------------
class nOpendeServer : public nRoot
{
    public:
        /// constructor
        nOpendeServer();
        /// destructor
        virtual ~nOpendeServer();
    
        // tri mesh management
    
        nOpendeTriMesh* NewTriMesh( const char* id, const char* filename, 
                                    int group = 0 );
        void DeleteTriMesh( const char* id );
        nOpendeTriMesh* FindTriMesh( const char* id );
            
        // surface parameters
    
        void BeginSurfaces( int num );
        void AddSurface( int index, const char* name );
        void EnableSurfaceParam( int index, const char* param );
        void SetSurfaceParam( int index, const char* param, float value );
        void EndSurfaces();
            
        nOpendeSurface* GetSurface( int index );
        int GetSurfaceIndex( const char* name );

        static nOpendeServer* opendeServer;
    
    private:
        
        nHashList meshList;
            
        bool inBeginSurfaces;
        nOpendeSurface* surfaceArray;
        int numSurfaces;
};

//------------------------------------------------------------------------------
#endif // N_OPENDESERVER
