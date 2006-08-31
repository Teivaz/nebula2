//------------------------------------------------------------------------------
//  loader/entityloaderbase.cc
//  (C) 2006 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "loader/entityloaderbase.h"

namespace Loader
{

ImplementRtti(Loader::EntityLoaderBase, Foundation::RefCounted);

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
EntityLoaderBase::Load(Db::Reader* dbReader)
{
    n_assert(dbReader);
    return false;
}

} // namespace Loader
