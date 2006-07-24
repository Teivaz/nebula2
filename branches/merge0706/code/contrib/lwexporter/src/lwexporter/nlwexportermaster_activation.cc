//----------------------------------------------------------------------------
// (c) 2006    Vadim Macagon
//----------------------------------------------------------------------------
#include "lwexporter/nlwexportermaster.h"
#include "lwwrapper/nlwcallbacktemplates.h"
#include "lwwrapper/nlwglobals.h"

const char nLWExporterMaster::HANDLER_NAME[] = "N2_LayoutExporter";

//----------------------------------------------------------------------------
/**
*/
XCALL_(int)
nLWExporterMaster::Activate_Handler(long version, GlobalFunc* global,
                                  LWMasterHandler* local, void* serverData)
{
    if (version != LWINTERFACE_VERSION)
        return AFUNC_BADVERSION;

    if (!local)
        return AFUNC_BADLOCAL;

    if (!local->inst)
        return AFUNC_BADLOCAL;

    if (!nLWGlobals::IsGlobalFuncSet())
        nLWGlobals::SetGlobalFunc(global);

    local->inst->priv = global;
    local->inst->create = nLWExporterMaster::OnCreate;
    local->inst->destroy = nLWExporterMaster::OnDestroy;
    local->inst->copy = nLWExporterMaster::OnCopy;
    local->inst->load = nLWCallbacks::Instance::Load<nLWExporterMaster>;
    local->inst->save = nLWCallbacks::Instance::Save<nLWExporterMaster>;
    local->inst->descln = nLWCallbacks::Instance::GetDescription<nLWExporterMaster>;

    if (!local->item)
        return AFUNC_BADLOCAL;

    local->item->useItems = nLWCallbacks::Item::UseItems<nLWExporterMaster>;
    local->item->changeID = nLWCallbacks::Item::ChangeID<nLWExporterMaster>;

    local->event = nLWCallbacks::MasterHandler::Event<nLWExporterMaster>;
    local->flags = nLWCallbacks::MasterHandler::Flags<nLWExporterMaster>;

    local->type = LWMAST_SCENE;

    return AFUNC_OK;
}

//----------------------------------------------------------------------------
/**
*/
XCALL_(int)
nLWExporterMaster::Activate_Interface(long version, GlobalFunc* global,
                                      LWInterface* local, void* serverData)
{
    if (version != LWINTERFACE_VERSION)
        return AFUNC_BADVERSION;

    if (!local)
        AFUNC_BADLOCAL;

    if (!nLWGlobals::IsGlobalFuncSet())
        nLWGlobals::SetGlobalFunc(global);

    local->panel = 0;
    local->options = nLWCallbacks::Interface::DisplayUI<nLWExporterMaster>;
    local->command = 0;

    return AFUNC_OK;
}

//----------------------------------------------------------------------------
/**
*/
int
nLWExporterMaster::ApplyToSceneFromGeneric(LWLayoutGeneric* local)
{
    n_assert(local);
    if (!local)
        return AFUNC_BADLOCAL;

    nString command;
    command.Format("ApplyServer %s %s", LWMASTER_HCLASS, nLWExporterMaster::HANDLER_NAME);
    if (local->evaluate(local->data, command.Get()) != 1)
    {
        return AFUNC_BADAPP;
    }

    return AFUNC_OK;
}

//----------------------------------------------------------------------------
/**
    @brief Find the nLWExporterMaster server index.
    @param serverIndex Will contain the index of the exporter master handler,
                       if AFUNC_OK is returned.
    @return AFUNC_OK on success, something else otherwise.

    This function will try to find the server index for the exporter's master
    handler, if that fails because there is no master handler in the scene it
    will try to create one.
*/
static
int
GetMasterHandlerServerIndex(int& serverIndex, GlobalFunc* global, LWLayoutGeneric* local)
{
    LWItemInfo* itemInfo = (LWItemInfo*)global(LWITEMINFO_GLOBAL, GFUSE_TRANSIENT);
    if (!itemInfo)
        return AFUNC_BADGLOBAL;

    // find the index of our master handler
    serverIndex = 1;
    const char* serverName = "";
    while (serverName)
    {
        serverName = itemInfo->server(0, LWMASTER_HCLASS, serverIndex);
        if (serverName)
        {
            if (stricmp(serverName, nLWExporterMaster::HANDLER_NAME) == 0)
                break;
            else
                serverIndex++; // keep looking
        }
    }

    // if we didn't find our master handler apply it to the scene now
    if (!serverName)
    {
        int retVal = nLWExporterMaster::ApplyToSceneFromGeneric(local);
        if (retVal != AFUNC_OK)
            return retVal;
    }

    // serverIndex now contains the index of our master handler
    return AFUNC_OK;
}

//----------------------------------------------------------------------------
/**
    @brief Invoke the exporter's master handler.
    @param serverIndex Index of the master handler in the scene server list.
                       @see GetMasterHandlerServerIndex.
    @return AFUNC_OK on success, something else otherwise.

    When the master handler is invoked it will perform the action that was
    set using @see nLWExporterMaster::SetNextAction.
*/
static
int
InvokeMasterHandlerFromGeneric(int serverIndex, LWLayoutGeneric* local)
{
    nString command;
    command.Format("EditServer %s %d", LWMASTER_HCLASS, serverIndex);
    if (local->evaluate(local->data, command.Get()) != 1)
        return AFUNC_BADAPP;

    return AFUNC_OK;
}

//----------------------------------------------------------------------------
// EOF
//----------------------------------------------------------------------------
