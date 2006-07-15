#ifndef N_MAPRESOURCELOADER_H
#define N_MAPRESOURCELOADER_H
//------------------------------------------------------------------------------
/**
    @class nMapResourceLoader
    @ingroup NMapContribModule
    @brief Loads nMap height information into an nMesh2 for use by MapBlock

    Only one instance of nMapResourceLoader need be created per game.  
    Its NOH path should be passed to each MapBlock::meshTriStrip with SetResourceLoader().
    The MapBlock from which meshTriStrip is initialized must be assigned with meshTriStrip->SetFilename().
    Both should be set prior to any other use of meshTriStrip.

    (C)	2004	Rafael Van Daele-Hunt
*/

#include "resource/nresourceloader.h"
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
    bool Load(const char *cmdString, nResource *callingResource);

    static const char* const SEPARATOR;
};
//------------------------------------------------------------------------------
#endif

