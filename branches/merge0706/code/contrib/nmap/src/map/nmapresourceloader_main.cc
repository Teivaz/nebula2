#define N_IMPLEMENTS nMapResourceLoader

//------------------------------------------------------------------------------
//  (C) 2004    Rafael Van Daele-Hunt
//------------------------------------------------------------------------------
#include "map/nmapresourceloader.h"
#include "map/mapblock.h"
#include "map/nmap.h"

nNebulaScriptClass(nMapResourceLoader, "nresourceloader");

//------------------------------------------------------------------------------
const char* const nMapResourceLoader::SEPARATOR = "!";
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

@param cmdString        a string of the following format:
                        the NOH path to the nMapNode, one of whose MapBlocks
                            holds the calling resource
                        SEPARATOR
                        the value of the MapBlock's startX member
                        SEPARATOR
                        the value of the MapBlock's startZ member
@param callingResource  ptr to the nResource calling nMapResourceLoader::Load()
@return                 success/failure
*/
bool
nMapResourceLoader::Load(const char *cmdString, nResource *callingResource)
{
    nMesh2* meshTriStrip = static_cast<nMesh2*>(callingResource);
    n_verify(meshTriStrip->CreateEmpty());

    nString stringParser(cmdString);
    nAutoRef<nMapNode> map = stringParser.GetFirstToken(SEPARATOR);
    int startX = atoi(stringParser.GetNextToken(SEPARATOR));
    int startZ = atoi(stringParser.GetNextToken(SEPARATOR));

    const nMap* map_data = map->GetMap();
    float * vbuf = meshTriStrip->LockVertices();
    n_assert(vbuf);

    float dim = float(map_data->GetDimension()-1);

    // Create a bounding box for the vertex buffer
    vector2 uv;
    for (int j = 0; j < map->GetBlockSize(); ++j)
    {
        int z = startZ + j;
        for (int i = 0; i < map->GetBlockSize(); ++i)
        {
            int x = startX + i;
            const MapPoint& pt = map_data->GetPoint(x, z);

            int index = i + j * map->GetBlockSize();

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

            *(vbuf++) = u * map->GetDetailScale();
            *(vbuf++) = v * map->GetDetailScale();
        }
    }
    meshTriStrip->UnlockVertices();
    return true;
}
