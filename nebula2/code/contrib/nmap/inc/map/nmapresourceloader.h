#ifndef N_MAPRESOURCELOADER_H
#define N_MAPRESOURCELOADER_H
//------------------------------------------------------------------------------
/**
    @class nMapResourceLoader

    @brief Loads nMap height information into an nMesh2 for use by MapBlock

    Only one instance of nMapResourceLoader need be created per game.  
    Its NOH path should be passed to each MapBlock::meshTriStrip with SetResourceLoader().
    The MapBlock from which meshTriStrip is initialized must be assigned with meshTriStrip->SetFilename().
    Both should be set prior to any other use of meshTriStrip.

    (C)	2003	Rafael Van Daele-Hunt
*/

#undef N_DEFINES
#define N_DEFINES nMapResourceLoader
#include "kernel/ndefdllclass.h"

#ifndef N_RESOURCELOADER_H
#include "resource/nresourceloader.h"
#endif

class nResource;

//------------------------------------------------------------------------------
class nMapResourceLoader : public nResourceLoader
{
public:
    /// constructor
    nMapResourceLoader();
    /// destructor
    virtual ~nMapResourceLoader();

    /// persistency
    virtual bool SaveCmds(nPersistServer* persistServer);

    /// Primary load method, the one called by an nResource.
    bool Load(const char *nMapBlockPath, nResource *callingResource);

    /// pointer to nKernelServer
    static nKernelServer* kernelServer;

};
//------------------------------------------------------------------------------
#endif

