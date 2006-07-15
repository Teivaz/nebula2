#ifndef LOADER_NAVMAPLOADER_H
#define LOADER_NAVMAPLOADER_H
//------------------------------------------------------------------------------
/**
    @class Loader::NavMapLoader

    Loads navigation maps into the world.

    (C) 2006 RadonLabs GmbH
*/
#include "foundation/refcounted.h"

//------------------------------------------------------------------------------
namespace Loader
{

class NavMapLoader : Foundation::RefCounted
{
    DeclareRtti;
    DeclareFactory(NavMapLoader);

public:
    /// load navigation map into the level
    static bool Load(const nString& levelName);
};

RegisterFactory(NavMapLoader);

} // namespace Loader
//------------------------------------------------------------------------------
#endif
