//------------------------------------------------------------------------------
//  foundation/refcounted.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "foundation/refcounted.h"
#include "foundation/server.h"

namespace Foundation
{
ImplementRootRtti(Foundation::RefCounted);

//------------------------------------------------------------------------------
/**
*/
RefCounted::RefCounted() : refCount(0)
{
    // add object to global ref counted list
    Server::refCountedList.AddTail(this);
}

//------------------------------------------------------------------------------
/**
    NOTE: the destructor of derived classes MUST be virtual!
*/
RefCounted::~RefCounted()
{
    n_assert(0 == this->refCount);

    // remove from global refcounted list
    this->Remove();
}

} // namespace Foundation
