#ifndef LOADER_LEVELLOADER_H
#define LOADER_LEVELLOADER_H
//------------------------------------------------------------------------------
/**
    @class Loader::LevelLoader

    Helper class for loading a complete level from the world database.

    (C) 2006 RadonLabs GmbH
*/
#include "foundation/refcounted.h"

//------------------------------------------------------------------------------
namespace Loader
{

class LevelLoader : public Foundation::RefCounted
{
    DeclareRtti;
    DeclareFactory(LevelLoader);

public:
    /// load a complete level from the world database
    static bool Load(const nString& levelName);
};

RegisterFactory(LevelLoader);

} // namespace Loader
//------------------------------------------------------------------------------
#endif
