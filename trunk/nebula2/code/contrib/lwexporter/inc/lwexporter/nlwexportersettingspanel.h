#ifndef N_LW_EXPORTER_SETTINGS_PANEL_H
#define N_LW_EXPORTER_SETTINGS_PANEL_H
//----------------------------------------------------------------------------
extern "C" 
{
#include <lwsdk/lwgeneric.h>
}

#include "lwwrapper/nlwglobals.h"
#include "wx/wx.h"

class wxFloatCtrl;

//----------------------------------------------------------------------------
/**
    @class nLWExporterSettingsPanel
    @brief Dialog that allows the user to edit the exporter settings that are
           stored in N2LightwaveExporter.xml.
*/
class nLWExporterSettingsPanel : public wxDialog
{
public:
    nLWExporterSettingsPanel(wxWindow*);
    virtual ~nLWExporterSettingsPanel();

    void Save();

    XCALL_(static int) Activate(long version, 
                                GlobalFunc* global, 
                                LWLayoutGeneric* local, 
                                void* /*serverData*/);

    static const char GENERIC_NAME[];

private:
    void PopulateControls();

    void OnExportNodeConfigBtn(wxCommandEvent&);
    void OnOKBtn(wxCommandEvent&);
    void OnCancelBtn(wxCommandEvent&);

    wxFloatCtrl* geometryScaleBox;
    wxChoice* scriptServerPopup;
    wxCheckBox* outputSceneNodeInBinaryCheckBox;
    wxCheckBox* outputMeshInBinaryCheckBox;
    wxCheckBox* outputAnimationInBinaryCheckBox;
    wxChoice* rendererPopup;
    wxChoice* renderSizePopup;
    wxCheckBox* renderFullScreenCheckBox;

    DECLARE_EVENT_TABLE()
};

//----------------------------------------------------------------------------
#endif // N_LW_EXPORTER_SETTINGS_PANEL_H
