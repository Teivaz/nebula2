#ifndef N_LW_EXPORTER_MASTER_PANEL_H
#define N_LW_EXPORTER_MASTER_PANEL_H
//----------------------------------------------------------------------------
extern "C" {
#include <lwsdk/lwgeneric.h>
}

#include "lwwrapper/nlwglobals.h"
#include "wx/wx.h"

//----------------------------------------------------------------------------
class nLWExporterMasterPanel : public wxDialog
{
public:
    nLWExporterMasterPanel(wxWindow*);
    virtual ~nLWExporterMasterPanel();

    void OnOK(wxCommandEvent&);
    void OnClose(wxCloseEvent&);

    XCALL_(static int) Activate(long version, 
                                GlobalFunc* global, 
                                LWLayoutGeneric* local, 
                                void* /*serverData*/);

    static const char GENERIC_NAME[];

    DECLARE_EVENT_TABLE()
};

//----------------------------------------------------------------------------
#endif // N_LW_EXPORTER_MASTER_PANEL_H
