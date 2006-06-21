//------------------------------------------------------------------------------
//  loader/entityloaderbase.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "loader/entityloaderbase.h"

namespace Loader
{

//------------------------------------------------------------------------------
/**
*/
EntityLoaderBase::EntityLoaderBase()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
EntityLoaderBase::~EntityLoaderBase()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
bool
EntityLoaderBase::Load(const nString& /*levelName*/)
{    
    return false;
}

} // namespace Loader


