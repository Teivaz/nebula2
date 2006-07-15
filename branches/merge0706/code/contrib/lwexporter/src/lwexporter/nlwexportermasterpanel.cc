//----------------------------------------------------------------------------
// (c) 2006    Vadim Macagon
//----------------------------------------------------------------------------
#include "lwexporter/nlwexportermasterpanel.h"
#include "lwexporter/nlwexportermaster.h"

const char nLWExporterMasterPanel::GENERIC_NAME[] = "N2_ExporterAboutPanel";

BEGIN_EVENT_TABLE(nLWExporterMasterPanel, wxDialog)
    EVT_BUTTON(wxID_OK, nLWExporterMasterPanel::OnOK)
    EVT_CLOSE(nLWExporterMasterPanel::OnClose)
END_EVENT_TABLE() 

//----------------------------------------------------------------------------
/**
*/
nLWExporterMasterPanel::nLWExporterMasterPanel(wxWindow* parent) :
    wxDialog(parent, wxID_ANY, "Nebula 2 Exporter", wxDefaultPosition)
{
    wxStaticText* textCtrl = new wxStaticText(this, wxID_ANY, 
                                              "Build: Pre-Alpha\n" \
                                              "Website: http://www.nebuladevice.org\n\n" \
                                              "\251 2006 Vadim Macagon");

    wxBoxSizer* topSizer = new wxBoxSizer(wxVERTICAL);
    topSizer->Add(textCtrl, 0, wxALL, 10);
    topSizer->Add(CreateButtonSizer(wxOK), 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 10);

    this->SetSizer(topSizer);
    topSizer->Fit(this);
}

//----------------------------------------------------------------------------
/**
*/
nLWExporterMasterPanel::~nLWExporterMasterPanel()
{
    // empty
}

//----------------------------------------------------------------------------
/**
*/
void 
nLWExporterMasterPanel::OnOK(wxCommandEvent& WXUNUSED(event))
{
    this->EndModal(wxOK);
}

//----------------------------------------------------------------------------
/**
*/
void 
nLWExporterMasterPanel::OnClose(wxCloseEvent& WXUNUSED(event))
{
    this->EndModal(wxOK);
} 

//----------------------------------------------------------------------------
/**
*/
XCALL_(int) 
nLWExporterMasterPanel::Activate(long version, GlobalFunc* global, 
                                 LWLayoutGeneric* local, void* /*serverData*/)
{
    if (version != LWINTERFACE_VERSION)
        return AFUNC_BADVERSION;

    if (!nLWGlobals::IsGlobalFuncSet())
        nLWGlobals::SetGlobalFunc(global);

    if (!nLWExporterMaster::Instance())
    {
        nLWExporterMaster::ApplyToSceneFromGeneric(local);
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

    HostDisplayInfo* hdi = nLWGlobals::GetHostDisplayInfo();
    if (!hdi)
        return AFUNC_BADAPP_SILENT;

    wxWindow parent;
    parent.SetHWND(hdi->window);
    parent.Enable(false);
    nLWExporterMasterPanel panel(&parent);
    panel.ShowModal();
    parent.Enable(true);
    parent.SetHWND(0);

    return AFUNC_OK;
}

//----------------------------------------------------------------------------
// EOF
//----------------------------------------------------------------------------
