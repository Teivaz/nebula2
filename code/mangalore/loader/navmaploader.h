#ifndef LOADER_NAVMAPLOADER_H
#define LOADER_NAVMAPLOADER_H
//------------------------------------------------------------------------------
/**
    @class Loader::NavMapLoader

    Loads navigation maps into the world.

    (C) 2006 RadonLabs GmbH
*/
#include "kernel/ntypes.h"
#include "util/nstring.h"

//------------------------------------------------------------------------------
namespace Loader
{
class NavMapLoader
{
public:
    /// load navigation map into the level
    static bool Load(const nString& levelName);
};

} // namespace Loader
//------------------------------------------------------------------------------
#endif
