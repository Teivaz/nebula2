//----------------------------------------------------------------------------
// (c) 2006    Vadim Macagon
//----------------------------------------------------------------------------
#include "lwexporter/nlwexportersettingspanel.h"
#include "lwexporter/nlwexportermaster.h"
#include "lwexporter/nlwexportersettings.h"
#include "lwexporter/nlwexportnodeconfigpanel.h"
#include "wx/gbsizer.h"
#include "lwwxui/wxfloatctrl.h"

//----------------------------------------------------------------------------
const char nLWExporterSettingsPanel::GENERIC_NAME[] = "N2_ExporterSettingsPanel";

enum
{
    ID_EXPORT_NODE_CONFIG_BTN = wxID_HIGHEST + 1,
};

BEGIN_EVENT_TABLE(nLWExporterSettingsPanel, wxDialog)
    EVT_BUTTON(wxID_OK, nLWExporterSettingsPanel::OnOKBtn)
    EVT_BUTTON(wxID_CANCEL, nLWExporterSettingsPanel::OnCancelBtn)
    EVT_BUTTON(ID_EXPORT_NODE_CONFIG_BTN, nLWExporterSettingsPanel::OnExportNodeConfigBtn)
END_EVENT_TABLE() 

//----------------------------------------------------------------------------
/**
*/
nLWExporterSettingsPanel::nLWExporterSettingsPanel(wxWindow* parent) :
    wxDialog(parent, wxID_ANY, "Exporter Settings", wxDefaultPosition)    
{
    wxStaticText* geometryScaleLabel = new wxStaticText(this, wxID_ANY, "Scale Geometry By");
    this->geometryScaleBox = new wxFloatCtrl(this, wxID_ANY);

    wxStaticText* scriptServerLabel = new wxStaticText(this, wxID_ANY, "Script Server");
    wxString scriptServers[] = { _T("nTclServer"), _T("nLuaServer") };
    this->scriptServerPopup = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 2, scriptServers);

    wxStaticText* configureExportNodeTypesLabel = new wxStaticText(this, wxID_ANY, "Export Node Types");
    wxButton* configureExportNodeTypesBtn = new wxButton(this, ID_EXPORT_NODE_CONFIG_BTN, "Configure...");

    // Output Format group-box

    wxStaticBox* outputFormatBox = new wxStaticBox(this, wxID_ANY, "Output In Binary Format");
    this->outputSceneNodeInBinaryCheckBox = new wxCheckBox(this, wxID_ANY, "Scene Node");
    this->outputMeshInBinaryCheckBox = new wxCheckBox(this, wxID_ANY, "Mesh");
    this->outputAnimationInBinaryCheckBox = new wxCheckBox(this, wxID_ANY, "Animation");

    // Preview group-box

    wxStaticBox* previewBox = new wxStaticBox(this, wxID_ANY, "Preview");
    wxStaticText* rendererLabel = new wxStaticText(this, wxID_ANY, "Renderer");
    this->rendererPopup = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxSize(100, 0));
    wxStaticText* renderSizeLabel = new wxStaticText(this, wxID_ANY, "Size");
    this->renderSizePopup = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxSize(100, 0));    
    this->renderFullScreenCheckBox = new wxCheckBox(this, wxID_ANY, "Full Screen");
    
    this->PopulateControls();

    // layout controls

    const int labelAlign = wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT;

    wxStaticBoxSizer* outputFormatBoxSizer = new wxStaticBoxSizer(outputFormatBox, wxVERTICAL);
    outputFormatBoxSizer->Add(this->outputSceneNodeInBinaryCheckBox);
    outputFormatBoxSizer->Add(this->outputMeshInBinaryCheckBox);
    outputFormatBoxSizer->Add(this->outputAnimationInBinaryCheckBox);

    wxGridBagSizer* previewBoxGrid = new wxGridBagSizer(5, 5);
    // row 0
    previewBoxGrid->Add(rendererLabel, wxGBPosition(0, 0), wxDefaultSpan, labelAlign);
    previewBoxGrid->Add(this->rendererPopup, wxGBPosition(0, 1));
    // row 1
    previewBoxGrid->Add(renderSizeLabel, wxGBPosition(1, 0), wxDefaultSpan, labelAlign);
    previewBoxGrid->Add(this->renderSizePopup, wxGBPosition(1, 1));
    // row 2
    previewBoxGrid->Add(this->renderFullScreenCheckBox, wxGBPosition(2, 1), wxDefaultSpan, wxEXPAND);

    wxStaticBoxSizer* previewBoxSizer = new wxStaticBoxSizer(previewBox, wxVERTICAL);
    previewBoxSizer->Add(previewBoxGrid, 0, wxALIGN_RIGHT);

    wxGridBagSizer* topSizer = new wxGridBagSizer(5, 5);
    // row 0
    topSizer->Add(geometryScaleLabel, wxGBPosition(0, 0), wxDefaultSpan, labelAlign|wxLEFT|wxTOP, 10);
    topSizer->Add(this->geometryScaleBox, wxGBPosition(0, 1), wxDefaultSpan, wxEXPAND|wxRIGHT|wxTOP, 10);
    // row 1
    topSizer->Add(scriptServerLabel, wxGBPosition(1, 0), wxDefaultSpan, labelAlign|wxLEFT, 10);
    topSizer->Add(this->scriptServerPopup, wxGBPosition(1, 1), wxDefaultSpan, wxEXPAND|wxRIGHT, 10);
    // row 2
    topSizer->Add(configureExportNodeTypesLabel, wxGBPosition(2, 0), wxDefaultSpan, labelAlign|wxLEFT, 10);
    topSizer->Add(configureExportNodeTypesBtn, wxGBPosition(2, 1), wxDefaultSpan, wxEXPAND|wxRIGHT, 10);
    // row 3
    topSizer->Add(outputFormatBoxSizer, wxGBPosition(3, 0), wxGBSpan(1, 2), wxEXPAND|wxLEFT|wxRIGHT, 10);
    // row 4
    topSizer->Add(previewBoxSizer, wxGBPosition(4, 0), wxGBSpan(1, 2), wxEXPAND|wxLEFT|wxRIGHT, 10);
    // row 5
    topSizer->Add(CreateButtonSizer(wxOK | wxCANCEL), wxGBPosition(5, 0), wxGBSpan(1, 2), wxALIGN_CENTER_HORIZONTAL|wxALL, 10);

    topSizer->Fit(this);
    this->SetSizer(topSizer);
}

//----------------------------------------------------------------------------
/**
*/
nLWExporterSettingsPanel::~nLWExporterSettingsPanel()
{
    // empty
}

//----------------------------------------------------------------------------
/**
*/
void
nLWExporterSettingsPanel::PopulateControls()
{
    nLWExporterSettings* settings = nLWExporterSettings::Instance();
    
    if (settings)
    {
        this->geometryScaleBox->SetFloatValue(settings->GetGeometryScale());
        this->scriptServerPopup->SetStringSelection(settings->GetScriptServer().Get());
        this->outputSceneNodeInBinaryCheckBox->SetValue(settings->GetOutputBinarySceneNode());
        this->outputMeshInBinaryCheckBox->SetValue(settings->GetOutputBinaryMesh());
        this->outputAnimationInBinaryCheckBox->SetValue(settings->GetOutputBinaryAnimation());
        for (int i = 0; i < settings->GetNumPreviewRenderers(); i++)
        {
            this->rendererPopup->AppendString(settings->GetPreviewRendererName(i).Get());
        }
        this->rendererPopup->SetStringSelection(settings->GetPreviewRenderer().Get());
        for (int i = 0; i < settings->GetNumPreviewSizes(); i++)
        {
            this->renderSizePopup->AppendString(settings->GetPreviewSize(i).Get());
        }
        nString previewSize;
        previewSize.Format("%dx%d", settings->GetPreviewWidth(), settings->GetPreviewHeight());
        this->renderSizePopup->SetStringSelection(previewSize.Get());
        this->renderFullScreenCheckBox->SetValue(settings->GetPreviewFullScreen());
    }
}

//----------------------------------------------------------------------------
/**
*/
void
nLWExporterSettingsPanel::OnExportNodeConfigBtn(wxCommandEvent&)
{
    nLWExportNodeConfigPanel nodeConfigPanel(this);
    nodeConfigPanel.ShowModal();
}

//----------------------------------------------------------------------------
/**
*/
void
nLWExporterSettingsPanel::OnOKBtn(wxCommandEvent&)
{
    this->Save();
    this->EndModal(wxOK);
}

//----------------------------------------------------------------------------
/**
*/
void
nLWExporterSettingsPanel::OnCancelBtn(wxCommandEvent&)
{
    this->EndModal(wxCANCEL);
}

//----------------------------------------------------------------------------
/**
*/
void 
nLWExporterSettingsPanel::Save()
{
    // read values from controls and save to file
    nLWExporterSettings* settings = nLWExporterSettings::Instance();
    if (settings)
    {
        settings->SetGeometryScale(this->geometryScaleBox->GetFloatValue());
        settings->SetScriptServer(this->scriptServerPopup->GetStringSelection().c_str());
        settings->SetOutputBinarySceneNode(this->outputSceneNodeInBinaryCheckBox->GetValue());
        settings->SetOutputBinaryMesh(this->outputMeshInBinaryCheckBox->GetValue());
        settings->SetOutputBinaryAnimation(this->outputAnimationInBinaryCheckBox->GetValue());
        settings->SetPreviewRenderer(this->rendererPopup->GetStringSelection().c_str());
        nString previewSize = this->renderSizePopup->GetStringSelection();
        nString previewWidth = previewSize.GetFirstToken("x");
        nString previewHeight = previewSize.GetNextToken("x");
        settings->SetPreviewWidth(previewWidth.AsInt());
        settings->SetPreviewHeight(previewHeight.AsInt());
        settings->SetPreviewFullScreen(this->renderFullScreenCheckBox->GetValue());
        settings->Save();
    }
}

//----------------------------------------------------------------------------
/**
*/
XCALL_(int) 
nLWExporterSettingsPanel::Activate(long version, GlobalFunc* global, 
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
    nLWExporterSettingsPanel panel(&parent);
    panel.ShowModal();
    parent.Enable(true);
    parent.SetHWND(0);
    
    return AFUNC_OK;
}

//----------------------------------------------------------------------------
// EOF
//----------------------------------------------------------------------------
