#define N_IMPLEMENTS nMapResourceLoader

//------------------------------------------------------------------------------
//  (C) 2003	Rafael Van Daele-Hunt
//------------------------------------------------------------------------------
#include "map/nmapresourceloader.h"

nNebulaScriptClass(nMapResourceLoader, "nresourceloader");

//------------------------------------------------------------------------------
/**
*/
nMapResourceLoader::nMapResourceLoader()
{
   
}

//------------------------------------------------------------------------------
/**
*/
nMapResourceLoader::~nMapResourceLoader()
{
}

//------------------------------------------------------------------------------
/**
    Base Load() function for the nMapResourceLoader

    @param nMapBlockName    the NOH path to the MapBlock that provides the data
    @param callingResource  ptr to the nResource calling nMapResourceLoader::Load()
    @return                 success/failure
*/
bool nMapResourceLoader::Load(const char *nMapBlockName, nResource *callingResource)
{
    nMesh2* mesh = static_cast<nMesh2*>(callingResource);
    n_assert(!mesh->IsValid());

    //char vbuf_name[80];
    //sprintf(vbuf_name, "%s%d", mapvbuf_name, num);
    nAutoRef<nMapNode> refMapNode( kernelServer, nMapName );

	int num_vertices = refMapNode->GetBlockSize() * refMapNode->GetBlockSize();
	meshTriStrip->SetNumVertices(num_vertices);
	// indices needed by the patch
	int num_indices = 6 * refMapNode->GetBlockSize() * refMapNode->GetBlockSize();
	// indices needed by the crack fixes
	num_indices += (refMapNode->GetBlockSize()-1) * (refMapNode->GetBlockSize()-1) * 4;
	meshTriStrip->SetNumIndices(num_indices);

	// mesh vertex and index buffers are created empty
    nString mapNameBackup( nMapName );
    // We set the nD3D9Mesh filename to NULL and call its Load() - that forces it to
    // allocate the vertex buffer and index buffer for us.
    meshTriStrip->SetFilename("");
    meshTriStrip->Load();
    meshTriStrip->SetFilename(mapNameBackup.Get());

	float * vbuf = meshTriStrip->LockVertices();
	n_assert(vbuf);

    float dim = float(map_data->GetDimension()-1);

    // Create a bounding box for the vertex buffer
    vector2 uv;
    for (int j = 0; j < refMapNode->GetBlockSize(); ++j)
    {
        int z = startZ + j;
        for (int i = 0; i < refMapNode->GetBlockSize(); ++i)
        {
            int x = startX + i;
            const MapPoint& pt = map_data->GetPoint(x, z);

            int index = i + j * refMapNode->GetBlockSize();

			*(vbuf++) = pt.coord.x;
			*(vbuf++) = pt.coord.y;
			*(vbuf++) = pt.coord.z;
			*(vbuf++) = pt.normal.x;
			*(vbuf++) = pt.normal.y;
			*(vbuf++) = pt.normal.z;

            float u = x / dim;
            float v = 1.0f - z / dim;

			*(vbuf++) = u;
			*(vbuf++) = v;

			*(vbuf++) = u * refMapNode->GetDetailScale();
			*(vbuf++) = v * refMapNode->GetDetailScale();
        }
    }
	meshTriStrip->UnlockVertices();
	meshTriStrip->GetGroup(0).SetFirstVertex(0);
	meshTriStrip->GetGroup(0).SetNumVertices(num_vertices);
	meshTriStrip->GetGroup(0).SetFirstIndex(0);
	meshTriStrip->GetGroup(0).SetNumIndices(num_indices);
	meshTriStrip->GetGroup(0).SetBoundingBox(boundingBox);











            
            // Lock the vertex buffer, and verify the validity of the pointer
            vertexBufferPtr = mesh->LockVertices();
            n_assert(vertexBufferPtr);

            // Lock the index buffer, and verify the validity of the pointer.
            indexBufferPtr = mesh->LockIndices();
            n_assert(indexBufferPtr);

            
            // Make sure the vb is created and ready to go.
    n_assert(indexBufferPtr);
    n_assert(vertexBufferPtr);
    n_assert(numMeshes);
    n_assert(numvertspermesh);
    n_assert(numgroupspermesh);
    n_assert(numtrispermesh);

    // Cache a few variables for the next series of loads
    int vertexWidth = mesh->GetVertexWidth();

   

    // unlock buffers
    if (vertexBufferPtr)
    {
        mesh->UnlockVertices();
    }
    if (indexBufferPtr)
    {
        mesh->UnlockIndices();
    }



    return true;
}

