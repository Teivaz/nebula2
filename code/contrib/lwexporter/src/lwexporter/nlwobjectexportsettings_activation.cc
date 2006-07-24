//----------------------------------------------------------------------------
// (c) 2006    Vadim Macagon
//----------------------------------------------------------------------------
extern "C"
{
#include <lwsdk/lwhost.h>
}

#include "lwexporter/nlwobjectexportsettings.h"
#include "lwwrapper/nlwcallbacktemplates.h"
#include "lwwrapper/nlwglobals.h"
#include "lwexporter/nlwexportermaster.h"

const char nLWObjectExportSettings::HANDLER_NAME[] = "N2_ExporterObjExpSettings";
const char nLWObjectExportSettings::SHORTCUT_GENERIC_NAME[] = "N2_ExporterObjExpSettingsShortcut";

//----------------------------------------------------------------------------
/**
*/
XCALL_(int)
nLWObjectExportSettings::Activate_Handler(long version,
                                          GlobalFunc* global,
                                          LWCustomObjHandler* local,
                                          void* /*serverData*/)
{
    if (version != LWINTERFACE_VERSION)
        return AFUNC_BADVERSION;

    if (!local)
        return AFUNC_BADLOCAL;

    if (!local->inst)
        return AFUNC_BADLOCAL;

    // This stuff below is now obsolete.
    // This doesn't actually work for me in Lightwave 7.5c because priv
    // is not passed to OnCreate(), even though it should according to the
    // docs and this approach works for the master and shader handlers.
    // I'm leaving it uncommented just in case it works in other versions.
    //local->inst->priv = global;
    // work around the bug mentioned above
    //nLWObjectExportSettings::globalFunc = global;

    if (!nLWGlobals::IsGlobalFuncSet())
        nLWGlobals::SetGlobalFunc(global);

    if (!nLWExporterMaster::Instance())
        return AFUNC_BADAPP_SILENT;

    local->inst->create = nLWObjectExportSettings::OnLWCreate;
    local->inst->destroy = nLWCallbacks::Instance::Destroy<nLWObjectExportSettings>;
    local->inst->copy = nLWObjectExportSettings::OnLWCopy;
    local->inst->load = nLWCallbacks::Instance::Load<nLWObjectExportSettings>;
    local->inst->save = nLWCallbacks::Instance::Save<nLWObjectExportSettings>;
    local->inst->descln = nLWCallbacks::Instance::GetDescription<nLWObjectExportSettings>;

    if (!local->item)
        return AFUNC_BADLOCAL;

    local->item->useItems = nLWCallbacks::Item::UseItems<nLWObjectExportSettings>;
    local->item->changeID = nLWCallbacks::Item::ChangeID<nLWObjectExportSettings>;

    local->evaluate = nLWCallbacks::CustomObjHandler::Evaluate<nLWObjectExportSettings>;
    local->flags = nLWCallbacks::CustomObjHandler::Flags<nLWObjectExportSettings>;

    return AFUNC_OK;
}

//----------------------------------------------------------------------------
/**
*/
XCALL_(int)
nLWObjectExportSettings::Activate_Interface(long version,
                                            GlobalFunc* global,
                                            LWInterface* local,
                                            void* serverData)
{
    if (version != LWINTERFACE_VERSION)
        return AFUNC_BADVERSION;

    if (!local)
        return AFUNC_BADLOCAL;

    if (!nLWGlobals::IsGlobalFuncSet())
        nLWGlobals::SetGlobalFunc(global);

    if (!nLWExporterMaster::Instance())
        return AFUNC_BADAPP_SILENT;

    local->panel = 0;
    local->options = nLWCallbacks::Interface::DisplayUI<nLWObjectExportSettings>;
    local->command = 0;

    return AFUNC_OK;
}

//----------------------------------------------------------------------------
/**
    @brief Check if the given item has an nLWObjectExportSettings custom
           object handler attached.
    @return true if the handler is found, false otherwise.
*/
bool
nLWObjectExportSettings::HasHandler(LWItemID lwItemId)
{
    nLWGlobals::ItemInfo itemInfo;
    n_assert(itemInfo.IsValid())
    if (!itemInfo.IsValid())
        return false;

    // find the index of our handler
    int serverIndex = 1;
    const char* serverName = "";
    while (serverName)
    {
        serverName = itemInfo.Get()->server(lwItemId, LWCUSTOMOBJ_HCLASS, serverIndex);
        if (serverName)
        {
            if (stricmp(serverName, nLWObjectExportSettings::HANDLER_NAME) == 0)
                break;
            else
                serverIndex++; // keep looking
        }
    }

    if (serverName) // serverName will by NULL if the handler wasn't found
        return true;

    return false;
}

//----------------------------------------------------------------------------
/**
    @brief Find the nLWObjectExportSettings server index on the given item.
    @param serverIndex Will contain the index of the custom object handler,
                       if AFUNC_OK is returned.
    @return AFUNC_OK on success, something else otherwise.

    This function will try to find the server index for the object export
    settings custom object hander, if that fails because there is no such
    handler attached to the object it will try to create one.
*/
static
int
GetCustomObjectHandlerServerIndex(LWItemID lwItemId,
                                  int& serverIndex,
                                  GlobalFunc* global,
                                  LWLayoutGeneric* local)
{
    LWItemInfo* itemInfo = (LWItemInfo*)global(LWITEMINFO_GLOBAL, GFUSE_TRANSIENT);
    if (!itemInfo)
        return AFUNC_BADGLOBAL;

    // find the index of our handler
    serverIndex = 1;
    const char* serverName = "";
    while (serverName)
    {
        serverName = itemInfo->server(lwItemId, LWCUSTOMOBJ_HCLASS, serverIndex);
        if (serverName)
        {
            if (stricmp(serverName, nLWObjectExportSettings::HANDLER_NAME) == 0)
                break;
            else
                serverIndex++; // keep looking
        }
    }

    // if we didn't find our handler apply it to the selected object now
    if (!serverName)
    {
        nString command;
        command.Format("ApplyServer %s %s",
                       LWCUSTOMOBJ_HCLASS,
                       nLWObjectExportSettings::HANDLER_NAME);
        if (local->evaluate(local->data, command.Get()) != 1)
            return AFUNC_BADAPP;
    }

    // serverIndex now contains the index of our handler
    return AFUNC_OK;
}

//----------------------------------------------------------------------------
/**
    @brief Invoke the object export settings custom object handler.
    @param serverIndex Index of the handler in the scene server list.
                       @see GetCustomObjectHandlerServerIndex.
    @return AFUNC_OK on success, something else otherwise.
*/
static
int
InvokeCustomObjectHandlerFromGeneric(int serverIndex, LWLayoutGeneric* local)
{
    nString command;
    command.Format("EditServer %s %d", LWCUSTOMOBJ_HCLASS, serverIndex);
    if (local->evaluate(local->data, command.Get()) != 1)
        return AFUNC_BADAPP;

    return AFUNC_OK;
}

//----------------------------------------------------------------------------
/**
*/
XCALL_(int)
nLWObjectExportSettings::Activate_SettingsPanel(long version,
                                                GlobalFunc* global,
                                                LWLayoutGeneric* local,
                                                void* /*serverData*/)
{
    if (version != LWINTERFACE_VERSION)
        return AFUNC_BADVERSION;

    int retval = AFUNC_OK;

    if (!nLWGlobals::IsGlobalFuncSet())
        nLWGlobals::SetGlobalFunc(global);

    if (!nLWExporterMaster::Instance())
    {
        int retVal = nLWExporterMaster::ApplyToSceneFromGeneric(local);
        if (AFUNC_OK != retVal)
        {
            return retVal;
        }
        if (!nLWExporterMaster::Instance())
        {
            nLWGlobals::MessageFuncs messageFuncs;
            if (messageFuncs.IsValid())
            {
                messageFuncs.Get()->error("Failed to create N2 Exporter Master!", 0);
            }
            return AFUNC_BADAPP_SILENT;
        }
    }

    // get the currently selected item ID
    nLWGlobals::InterfaceInfo interfaceInfo;
    if (!interfaceInfo.IsValid())
        return AFUNC_BADGLOBAL;

    int itemCount = 0;
    while (interfaceInfo.Get()->selItems[itemCount])
        itemCount++;

    if (1 == itemCount)
    {
        nLWGlobals::ItemInfo itemInfo;
        if (!itemInfo.IsValid())
            return AFUNC_BADGLOBAL;

        // object export settings can only be set for mesh objects
        if (LWI_OBJECT != itemInfo.Get()->type(interfaceInfo.Get()->selItems[0]))
            itemCount = 0;
    }

    if (itemCount != 1)
    {
        LWMessageFuncs* msgFuncs = (LWMessageFuncs*)global(LWMESSAGEFUNCS_GLOBAL, GFUSE_TRANSIENT);
        msgFuncs->error("You must select one, and only one mesh object.", 0);
        return AFUNC_BADAPP_SILENT;
    }

    int serverIndex = 0;
    retval = GetCustomObjectHandlerServerIndex(interfaceInfo.Get()->selItems[0],
                                               serverIndex,
                                               global, local);
    if (AFUNC_OK == retval)
        retval = InvokeCustomObjectHandlerFromGeneric(serverIndex, local);

    return retval;
}

