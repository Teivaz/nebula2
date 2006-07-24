//----------------------------------------------------------------------------
// (c) 2006    Vadim Macagon
//----------------------------------------------------------------------------
#include "lwexporter/nlwsettingsregistry.h"
#include "lwexporter/nlwobjectexportsettings.h"
#include "lwexporter/nlwshaderexportsettings.h"

nLWSettingsRegistry* nLWSettingsRegistry::singleton = 0;

//----------------------------------------------------------------------------
/**
*/
nLWSettingsRegistry::nLWSettingsRegistry()
{
    nLWSettingsRegistry::singleton = this;
}

//----------------------------------------------------------------------------
/**
*/
nLWSettingsRegistry::~nLWSettingsRegistry()
{
    n_assert(this->objExpSettingsArray.Size() == 0);
    n_assert(this->surfaceShaderMap.empty());
}

//----------------------------------------------------------------------------
/**
*/
nLWSettingsRegistry*
nLWSettingsRegistry::Instance()
{
    if (!nLWSettingsRegistry::singleton)
    {
        n_new(nLWSettingsRegistry);
    }
    n_assert(nLWSettingsRegistry::singleton);
    return nLWSettingsRegistry::singleton;
}

//----------------------------------------------------------------------------
/**
*/
void
nLWSettingsRegistry::FreeInstance()
{
    if (nLWSettingsRegistry::singleton)
    {
        n_delete(nLWSettingsRegistry::singleton);
        nLWSettingsRegistry::singleton = 0;
    }
}

//----------------------------------------------------------------------------
/**
    @brief Add the given object export settings handler to the registry.
*/
void
nLWSettingsRegistry::Add(nLWObjectExportSettings* settings)
{
    n_assert(settings);
    if (!settings)
        return;

    int key = reinterpret_cast<int>(settings->GetItemId());
    n_assert(!this->objExpSettingsArray.HasKey(key));
    this->objExpSettingsArray.Add(key, settings);
}

//----------------------------------------------------------------------------
/**
    @brief Remove the given object export settings handler from the registry.
*/
void
nLWSettingsRegistry::Remove(nLWObjectExportSettings* settings)
{
    n_assert(settings);
    if (!settings)
        return;

    int key = reinterpret_cast<int>(settings->GetItemId());
    n_assert(this->objExpSettingsArray.HasKey(key));
    this->objExpSettingsArray.Rem(key);
}

//----------------------------------------------------------------------------
/**
    @brief Associate an existing handler in the registry with a new item id.
*/
void
nLWSettingsRegistry::Update(LWItemID oldId, LWItemID newId)
{
    int oldKey = reinterpret_cast<int>(oldId);
    nLWObjectExportSettings* settings = 0;
    if (!this->objExpSettingsArray.Find(oldKey, settings))
    {
        n_assert("nLWSettingsRegistry::Update(): oldKey not found in registry.");
        return;
    }
    this->objExpSettingsArray.Rem(oldKey);

    int newKey = reinterpret_cast<int>(newId);
    if (this->objExpSettingsArray.HasKey(newKey))
    {
        n_assert("nLWSettingsRegistry::Update(): newKey already used! You're screwed!");
        return;
    }
    this->objExpSettingsArray.Add(newKey, settings);
}

//----------------------------------------------------------------------------
/**
    @brief Find an object export settings handler in the master registry
           for the given item.
    @return Pointer to a handler or NULL if one isn't found for the given item.
*/
nLWObjectExportSettings*
nLWSettingsRegistry::Get(LWItemID itemId) const
{
    int key = reinterpret_cast<int>(itemId);
    nLWObjectExportSettings* settings = 0;
    this->objExpSettingsArray.Find(key, settings);
    return settings;
}

//----------------------------------------------------------------------------
/**
*/
void
nLWSettingsRegistry::Add(LWSurfaceID surfId, nLWShaderExportSettings* settings)
{
    this->surfaceShaderMap[surfId] = settings;
}

//----------------------------------------------------------------------------
/**
*/
void
nLWSettingsRegistry::Remove(LWSurfaceID surfId)
{
    this->surfaceShaderMap.erase(surfId);
}

//----------------------------------------------------------------------------
/**
*/
nLWShaderExportSettings*
nLWSettingsRegistry::Get(LWSurfaceID surfId) const
{
    nLWSurfaceShaderMap::const_iterator it = this->surfaceShaderMap.find(surfId);
    if (it != this->surfaceShaderMap.end())
    {
        return it->second;
    }
    return 0;
}

//----------------------------------------------------------------------------
/**
*/
/*
void
nLWSettingsRegistry::Add(const nString& objFileName, const nString& surfName,
                         nLWShaderExportSettings* settings)
{
    nHashNode* objNode = objFileNameList.Find(objFileName.Get());
    if (objNode)
    {
        nHashList* surfaceNameList = (nHashList*)objNode->GetPtr();
        if (surfaceNameList)
        {
            nHashNode* surfNode = surfaceNameList->Find(surfName.Get());
            n_assert(!surfNode);
            if (!surfNode)
            {
                surfNode = n_new(nHashNode(surfName.Get()));
                surfNode->SetPtr(settings);
                surfaceNameList->AddTail(surfNode);
            }
        }
    }
}
*/

//----------------------------------------------------------------------------
/**
*/
/*
void
nLWSettingsRegistry::Remove(const nString& objFileName,
                            const nString& surfName)
{
    nHashNode* objNode = objFileNameList.Find(objFileName.Get());
    if (objNode)
    {
        nHashList* surfaceNameList = (nHashList*)objNode->GetPtr();
        if (surfaceNameList)
        {
            nHashNode* surfNode = surfaceNameList->Find(surfName.Get());
            if (surfNode)
            {
                surfNode->Remove();
                n_delete(surfNode);
            }

            if (surfaceNameList->IsEmpty())
            {
                objNode->Remove();
                n_delete(surfaceNameList);
                n_delete(objNode);
            }
        }
    }
}
*/

//----------------------------------------------------------------------------
/**
*/
/*
nLWShaderExportSettings*
nLWSettingsRegistry::Get(const nString& objFileName,
                         const nString& surfName) const
{
    nHashNode* objNode = objFileNameList.Find(objFileName.Get());
    if (objNode)
    {
        nHashList* surfaceNameList = (nHashList*)objNode->GetPtr();
        if (surfaceNameList)
        {
            nHashNode* surfNode = surfaceNameList->Find(surfName.Get());
            if (surfNode)
            {
                return (nLWShaderExportSettings*)surfNode->GetPtr();
            }
        }
    }
    return 0;
}
*/

//----------------------------------------------------------------------------
// EOF
//----------------------------------------------------------------------------
