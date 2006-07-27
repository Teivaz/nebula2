//------------------------------------------------------------------------------
//  ceui/resourceprovider.h
//  (c) 2006 Nebula2 Community
//------------------------------------------------------------------------------
#ifndef CEUI_RESOURCEPROVIDER_H
#define CEUI_RESOURCEPROVIDER_H

#include "cegui/CEGUIDefaultResourceProvider.h"

namespace CEUI
{

class ResourceProvider : public CEGUI::DefaultResourceProvider
{
public:
    /// constructor
    ResourceProvider();
    /// destructor
    virtual ~ResourceProvider();
    ///
    virtual void loadRawDataContainer(const CEGUI::String& fileName, CEGUI::RawDataContainer& output, const CEGUI::String& resGroup);
    ///
    virtual void unloadRawDataContainer(CEGUI::RawDataContainer& data);
};

}

#endif
