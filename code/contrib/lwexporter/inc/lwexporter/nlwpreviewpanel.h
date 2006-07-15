#ifndef N_LW_PREVIEW_PANEL_H
#define N_LW_PREVIEW_PANEL_H
//----------------------------------------------------------------------------
extern "C" {
#include <lwsdk/lwgeneric.h>
}

#include "lwwrapper/nlwglobals.h"
#include "lwexporter/nlwexporter.h"
#include "lwwrapper/nlwcmdexec.h"
#include "wx/wx.h"

//----------------------------------------------------------------------------
class nLWPreviewPanel : public wxDialog
{
public:
    nLWPreviewPanel(wxWindow*, const nLWCmdExec&);
    virtual ~nLWPreviewPanel();

    XCALL_(static int) Activate(long version, 
                                GlobalFunc* global, 
                                LWLayoutGeneric* local, 
                                void* /*serverData*/);

    static const char GENERIC_NAME[];

private:
    void OnOKBtn(wxCommandEvent&);
    void OnCancelBtn(wxCommandEvent&);

    void RunPreview(nLWExporter::ExportMode exportMode);

    wxRadioBox* radioBox;

    nLWCmdExec cmdExec;

    DECLARE_EVENT_TABLE()
};

//----------------------------------------------------------------------------
#endif // N_LW_PREVIEW_PANEL_H
