#define N_IMPLEMENTS nResourceLoader
//------------------------------------------------------------------------------
//  (C) 2003 Megan Fox
//------------------------------------------------------------------------------
#include "resource/nresourceloader.h"

nNebulaScriptClass(nResourceLoader, "nroot");

//------------------------------------------------------------------------------
/**
*/
nResourceLoader::nResourceLoader()
{
}

//------------------------------------------------------------------------------
/**
*/
nResourceLoader::~nResourceLoader()
{
}

//------------------------------------------------------------------------------
/**
    Base Load() function for the nResourceLoader.  This is to be redefined by child classes

    @param path             the full path to the to-be-loaded file
    @param callingResource  ptr to the nResource calling nResourceLoader::Load()

    @return                 success/failure
*/
bool nResourceLoader::Load(const char *sFilename, nResource *callingResource)
{
    return false;
}

