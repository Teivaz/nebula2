//----------------------------------------------------------------------------
// (c) 2006    Vadim Macagon
//----------------------------------------------------------------------------
extern "C" 
{
#include <lwsdk/lwmodule.h>
#include <lwsdk/lwserver.h>
#include <lwsdk/lwmaster.h>
#include <lwsdk/lwmeshedt.h>
#include <lwsdk/lwcustobj.h>
#include <lwsdk/lwshader.h>
}

#include "lwexporter/nlwexportermaster.h"
#include "lwexporter/nlwobjectexportsettings.h"
#include "lwexporter/nlwshadereditor.h"
#include "lwexporter/nlwexportermasterpanel.h"
#include "lwexporter/nlwexportersettingspanel.h"
#include "lwexporter/nlwexportpanel.h"
#include "lwexporter/nlwpreviewpanel.h"
#include "lwexporter/nlwanimexporthandler.h"

//extern XCALL_(int) Activate (long, GlobalFunc *, void *, void *);

// Human friendly names for the plugin(s)
ServerUserName MasterUN[] = {
    { "N2 Exporter Scene Master", LANGID_USENGLISH | SRVTAG_USERNAME },
    { "Nebula 2", LANGID_USENGLISH | SRVTAG_CMDGROUP },
};
ServerUserName AboutPanelUN[] = {
    { "N2 Exporter About Panel", LANGID_USENGLISH | SRVTAG_USERNAME },
    { "About", LANGID_USENGLISH | SRVTAG_BUTTONNAME },
    { "Nebula 2", LANGID_USENGLISH | SRVTAG_CMDGROUP },
};
ServerUserName SettingsPanelUN[] = {
    { "N2 Exporter Settings Panel", LANGID_USENGLISH | SRVTAG_USERNAME },
    { "Exporter Settings", LANGID_USENGLISH | SRVTAG_BUTTONNAME },
    { "Nebula 2", LANGID_USENGLISH | SRVTAG_CMDGROUP },
};
ServerUserName PreviewPanelUN[] = {
    { "N2 Exporter Preview Panel", LANGID_USENGLISH | SRVTAG_USERNAME },
    { "Preview", LANGID_USENGLISH | SRVTAG_BUTTONNAME },
    { "Nebula 2", LANGID_USENGLISH | SRVTAG_CMDGROUP },
};
ServerUserName ExportPanelUN[] = {
    { "N2 Exporter Export Panel", LANGID_USENGLISH | SRVTAG_USERNAME },
    { "Export", LANGID_USENGLISH | SRVTAG_BUTTONNAME },
    { "Nebula 2", LANGID_USENGLISH | SRVTAG_CMDGROUP },
};
ServerUserName ObjExpSettingsUN[] = {
    { "N2 Exporter Object Export Settings Handler", LANGID_USENGLISH | SRVTAG_USERNAME },
    { "Nebula 2", LANGID_USENGLISH | SRVTAG_CMDGROUP },
};
ServerUserName ObjExpSettingsShortcutUN[] = {
    { "N2 Exporter Object Export Settings", LANGID_USENGLISH | SRVTAG_USERNAME },
    { "Object Settings", LANGID_USENGLISH | SRVTAG_BUTTONNAME },
    { "Nebula 2", LANGID_USENGLISH | SRVTAG_CMDGROUP },
};
ServerUserName ShaderEditorUN[] = {
    { "Nebula 2 Shader", LANGID_USENGLISH | SRVTAG_USERNAME },
    { "Nebula 2 Shader", LANGID_USENGLISH | SRVTAG_BUTTONNAME },
    { "Nebula 2", LANGID_USENGLISH | SRVTAG_CMDGROUP },
};
ServerUserName AnimExportUN[] = {
    { "Nebula 2 Animation Export Handler", LANGID_USENGLISH | SRVTAG_USERNAME },
    { "Nebula 2", LANGID_USENGLISH | SRVTAG_CMDGROUP },
};

ServerRecord ServerDesc[] = {
    //{ ServerClass, ServerName, Activate, UserNames },
    { 
        LWMASTER_HCLASS, nLWExporterMaster::HANDLER_NAME, 
        (ActivateFunc*)&nLWExporterMaster::Activate_Handler, 
        MasterUN 
    },
    { 
        LWMASTER_ICLASS, nLWExporterMaster::HANDLER_NAME, 
        (ActivateFunc*)&nLWExporterMaster::Activate_Interface, 
        MasterUN 
    },
    { 
        LWLAYOUTGENERIC_CLASS, nLWExporterMasterPanel::GENERIC_NAME,
        (ActivateFunc*)&nLWExporterMasterPanel::Activate, 
        AboutPanelUN 
    },
    {
        LWLAYOUTGENERIC_CLASS, nLWExporterSettingsPanel::GENERIC_NAME,
        (ActivateFunc*)&nLWExporterSettingsPanel::Activate,
        SettingsPanelUN
    },
    {
        LWLAYOUTGENERIC_CLASS, nLWPreviewPanel::GENERIC_NAME,
        (ActivateFunc*)&nLWPreviewPanel::Activate, 
        PreviewPanelUN 
    },
    {
        LWLAYOUTGENERIC_CLASS, nLWExportPanel::GENERIC_NAME,
        (ActivateFunc*)&nLWExportPanel::Activate, 
        ExportPanelUN 
    },
    {
        LWCUSTOMOBJ_HCLASS, nLWObjectExportSettings::HANDLER_NAME,
        (ActivateFunc*)&nLWObjectExportSettings::Activate_Handler,
        ObjExpSettingsUN
    },
    {
        LWCUSTOMOBJ_ICLASS, nLWObjectExportSettings::HANDLER_NAME,
        (ActivateFunc*)&nLWObjectExportSettings::Activate_Interface,
        ObjExpSettingsUN
    },
    {
        LWLAYOUTGENERIC_CLASS, nLWObjectExportSettings::SHORTCUT_GENERIC_NAME,
        (ActivateFunc*)&nLWObjectExportSettings::Activate_SettingsPanel, 
        ObjExpSettingsShortcutUN 
    },
    {
        LWSHADER_HCLASS, nLWShaderEditor::HANDLER_NAME,
        (ActivateFunc*)&nLWShaderEditor::Activate_Handler,
        ShaderEditorUN
    },
    {
        LWSHADER_ICLASS, nLWShaderEditor::HANDLER_NAME,
        (ActivateFunc*)&nLWShaderEditor::Activate_Interface,
        ShaderEditorUN
    },
    {
        LWITEMMOTION_HCLASS, nLWAnimExportHandler::HANDLER_NAME,
        (ActivateFunc*)&nLWAnimExportHandler::Activate_Handler,
        AnimExportUN
    },
    { 0 }
};

// Default Startup function returns any non-zero value for success.
void* Startup(void)
{
    return (void*) 4;
}

// Default Shutdown implementation does nothing.
void Shutdown (void* serverData)
{
}

extern "C"
ModuleDescriptor _mod_descrip =
{
    MOD_SYSSYNC,
    MOD_SYSVER,
    MOD_MACHINE,
    Startup,
    Shutdown,
    ServerDesc
};


// wxWidgets
#include "wx/wx.h"
#include "windows.h"

class wxPluginApp : public wxApp
{
    bool OnInit();
};

bool wxPluginApp::OnInit()
{
    return true;
} 

IMPLEMENT_APP_NO_MAIN(wxPluginApp)

BOOL WINAPI DllMain(HANDLE hModule, DWORD ul_reason_for_call, 
                    LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
        case DLL_PROCESS_ATTACH:
        {   
            wxSetInstance((HINSTANCE)hModule);
            int argc = 0;
            char** argv = 0;
            wxEntryStart(argc, argv);
            if (!wxTheApp || !wxTheApp->CallOnInit())
                return FALSE;
        }
        break;

        case DLL_PROCESS_DETACH:
            wxEntryCleanup();
        break;
    }

    return TRUE;
}
