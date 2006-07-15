#ifndef N_OPENDETRIMESH_H
#define N_OPENDETRIMESH_H
//------------------------------------------------------------------------------
/**
    @class nOpendeTriMesh
    @ingroup NOpenDEContribModule
    @brief Stores collision mesh data and handles collision between the ODE
           tri-list geom and other geoms.

    (c) 2004 Vadim Macagon
    Based on Radon Labs nOpcodeShape & nCollideShape.
    
    nOpendeTriMesh is licensed under the terms of the Nebula License.
*/
//------------------------------------------------------------------------------
#include "kernel/ntypes.h"
#include "util/nhashnode.h"
#include "mathlib/vector.h"

#ifndef N_ODE_H
#define N_ODE_H
#include <ode/ode.h>
#endif

class nFileServer2;
class nFile;
//------------------------------------------------------------------------------
class nOpendeTriMesh : public nHashNode
{
    public:     
        /// constructor
        nOpendeTriMesh( const char* id );
        /// destructor
        ~nOpendeTriMesh();

        /// increment refcount by one
        int AddRef();
        /// decrement refcount by one
        int RemRef();
        /// get current refcount
        int GetRef();

        /// begin defining mesh
        void Begin( int numVertices, int numTriangles );
        /// add a vertex
        void SetVertex( int index, vector3& v );
        /// add a triangle
        void SetTriangle( int index, int p0Index, int p1Index, int p2Index );
        /// finish defining geometry
        void End();
    
        /// has the mesh been loaded?
        bool IsLoaded();
    
        /// load geometry from file
        bool Load( nFileServer2* fileServer, const char* filename, int group );

        /// the ODE object (don't destroy!)
        dTriMeshDataID data;
        
    private:
    
        bool LoadN3d2( nFileServer2*, const char* fname, int group );
        bool LoadNvx2( nFileServer2*, const char* fname, int group );
    
        int refCount;

        int numVertices;
        int numFaces;
        float* vertexData;
        int*   faceData;
        bool isLoaded;
};

//------------------------------------------------------------------------------
#endif // N_OPENDETRIMESH_H
