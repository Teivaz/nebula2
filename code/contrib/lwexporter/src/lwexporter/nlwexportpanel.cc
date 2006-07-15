//----------------------------------------------------------------------------
// (c) 2006    Vadim Macagon
//----------------------------------------------------------------------------
#include "lwexporter/nlwexportpanel.h"
#include "lwexporter/nlwexportermaster.h"
#include "lwwrapper/nlwlayoutmonitor.h"

//----------------------------------------------------------------------------
const char nLWExportPanel::GENERIC_NAME[] = "N2_ExportPanel";

BEGIN_EVENT_TABLE(nLWExportPanel, wxDialog)
    EVT_BUTTON(wxID_OK, nLWExportPanel::OnOKBtn)
    EVT_BUTTON(wxID_CANCEL, nLWExportPanel::OnCancelBtn)
END_EVENT_TABLE() 

//----------------------------------------------------------------------------
/**
*/
nLWExportPanel::nLWExportPanel(wxWindow* parent, const nLWCmdExec& cmdExec) :
    wxDialog(parent, wxID_ANY, "Export", wxDefaultPosition),
    radioBox(0),
    cmdExec(cmdExec)
{
    wxString choices[] = { _T("Selected Objects"), _T("Entire Scene") };
    this->radioBox = new wxRadioBox(this, wxID_ANY, "", 
                                    wxDefaultPosition, wxDefaultSize, 
                                    2, choices,
                                    2, wxRA_SPECIFY_ROWS);
    
    wxBoxSizer* topSizer = new wxBoxSizer(wxVERTICAL);
    topSizer->Add(this->radioBox, 0, wxEXPAND|wxLEFT|wxRIGHT, 10);
    topSizer->Add(CreateButtonSizer(wxOK|wxCANCEL), 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 10);

    topSizer->Fit(this);
    this->SetSizer(topSizer);
}

//----------------------------------------------------------------------------
/**
*/
nLWExportPanel::~nLWExportPanel()
{
    // empty
}

//----------------------------------------------------------------------------
/**
*/
void
nLWExportPanel::OnOKBtn(wxCommandEvent& WXUNUSED(event))
{
    switch (this->radioBox->GetSelection())
    {
        case 0:
            this->RunExport(nLWExporter::EXPORT_SELECTION);
        break;

        case 1:
            this->RunExport(nLWExporter::EXPORT_SCENE);
        break;
    }
    
    this->EndModal(wxOK);
}

//----------------------------------------------------------------------------
/**
*/
void
nLWExportPanel::OnCancelBtn(wxCommandEvent& WXUNUSED(event))
{
    this->EndModal(wxCANCEL);
}

//----------------------------------------------------------------------------
/**
*/
void 
nLWExportPanel::RunExport(nLWExporter::ExportMode exportMode)
{
    nLWExporter exporter(&this->cmdExec);
    exporter.SetExportMode(exportMode);
    exporter.Export();
}

//----------------------------------------------------------------------------
/**
*/
XCALL_(int) 
nLWExportPanel::Activate(long version, GlobalFunc* global, 
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
    nLWExportPanel panel(&parent, nLWCmdExec(local));
    panel.ShowModal();
    parent.Enable(true);
    parent.SetHWND(0);

    return AFUNC_OK;
}

//----------------------------------------------------------------------------
// EOF
//----------------------------------------------------------------------------
