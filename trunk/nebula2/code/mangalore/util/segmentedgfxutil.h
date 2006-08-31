#ifndef UTIL_SEGMENTEDGFXUTIL_H
#define UTIL_SEGMENTEDGFXUTIL_H
//------------------------------------------------------------------------------
/**
    @class Util::SegmentedGfxUtil

    Check if a Nebula2 graphics resource consists of hierarchy nodes below
    the toplevel node, and if yes, create one graphics entity for each
    hierarchy node. Otherwise create just a single graphics entity. This
    segmentation helps in visibility culling large environmental objects.

    (C) 2005 Radon Labs GmbH
*/
#include "graphics/entity.h"
#include "graphics/resource.h"

//------------------------------------------------------------------------------
namespace Util
{
class SegmentedGfxUtil
{
public:
    /// constructor
    SegmentedGfxUtil();
    /// create and setup one or more graphics entities from hierarchy nodes
    nArray<Ptr<Graphics::Entity> > CreateAndSetupGraphicsEntities(const nString& resName, const matrix44& worldMatrix, bool attachToLevel=true);
private:
    nArray<Ptr<Graphics::Resource> > resourceCache;
};

} // namespace Util
//------------------------------------------------------------------------------
#endif
