#ifndef N_LW_EXPORTER_MASTER_H
#define N_LW_EXPORTER_MASTER_H
//----------------------------------------------------------------------------
extern "C" {
#include <lwsdk/lwserver.h>
#include <lwsdk/lwmaster.h>
#include <lwsdk/lwgeneric.h>
}

#include "kernel/nrefcounted.h"

class nKernelServer;
class nLogHandler;
class nLWViewerRemote;

//----------------------------------------------------------------------------
class nLWExporterMaster : public nRefCounted
{
public:
    nLWExporterMaster();
    ~nLWExporterMaster();

    static nLWExporterMaster* Instance();

    /// called by a plugin to let the master know it exists
    static void PluginCreated();
    /// called by a plugin to let the master know it died
    static void PluginDestroyed();

    // these need to be public so the callback template functions can call them
    LWError OnLoad(const LWLoadState* loadState);
    LWError OnSave(const LWSaveState* saveState);
    const char* OnGetDescription();
    const LWItemID* OnUseItems();
    void OnChangeID(const LWItemID* idList);
    double OnEvent(const LWMasterAccess* access);
    unsigned int OnFlags();
    LWError OnDisplayUI();

    static int ApplyToSceneFromGeneric(LWLayoutGeneric* local);

    XCALL_(static int) Activate_Handler(long version, 
                                        GlobalFunc* global,
                                        LWMasterHandler* local, 
                                        void* serverData);

    XCALL_(static int) Activate_Interface(long version, 
                                          GlobalFunc* global,
                                          LWInterface* local, 
                                          void* serverData);

    static const char HANDLER_NAME[];

private:
    XCALL_(static LWInstance) OnCreate(void* priv, void* context, LWError* error);
    XCALL_(static void) OnDestroy(LWInstance instance);
    XCALL_(static LWError) OnCopy(LWInstance dest, LWInstance source);

    nKernelServer* kernelServer;
    nLogHandler* logHandler;
    nLWViewerRemote* viewerRemote;

    static nLWExporterMaster* singleton;
};

//----------------------------------------------------------------------------
#endif // N_LW_EXPORTER_MASTER_H
