#ifndef LOADER_LEVELLOADER_H
#define LOADER_LEVELLOADER_H
//------------------------------------------------------------------------------
/**
    @class Loader::LevelLoader
    
    Helper class for loading a complete level from the world database.
    
    (C) 2005 Radon Labs GmbH
*/
#include "kernel/ntypes.h"
#include "util/nstring.h"

//------------------------------------------------------------------------------
namespace Loader
{
class LevelLoader
{
public:
    /// load a complete level from the world database
    static bool Load(const nString& levelName);
};

}; // namespace Loader
//------------------------------------------------------------------------------
#endif