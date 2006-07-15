//------------------------------------------------------------------------------
//  (C) 2003 Megan Fox
//------------------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "resource/nresourceloader.h"

nNebulaClass(nResourceLoader, "nroot");

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
*/
bool nResourceLoader::InitResource(const char* /*filename*/, nResource* /*callingResource*/)
{
    return false;
}

//------------------------------------------------------------------------------
/**
    Base Load() function for the nResourceLoader.  This is to be redefined by child classes

    @param filename         the full path to the to-be-loaded file
    @param callingResource  ptr to the nResource calling nResourceLoader::Load()

    @return                 success/failure
*/
bool nResourceLoader::Load(const char* /*filename*/, nResource* /*callingResource*/)
{
    return false;
}

