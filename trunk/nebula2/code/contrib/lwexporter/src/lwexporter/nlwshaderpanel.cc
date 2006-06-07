//----------------------------------------------------------------------------
// (c) 2006    Vadim Macagon
//----------------------------------------------------------------------------
#include "lwexporter/nlwshaderpanel.h"
#include "tinyxml/tinyxml.h"
#include "lwexporter/nlwshaderpanelfactory.h"
#include "lwexporter/nlwshaderexportsettings.h"
#include "lwexporter/nlwshaderpanelparam.h"
#include "wx/gbsizer.h"
#include "lwexporter/nlwviewerremote.h"

//----------------------------------------------------------------------------
enum
{
    ID_SHADER_POPUP = wxID_HIGHEST + 1,
};

BEGIN_EVENT_TABLE(nLWShaderPanel, wxDialog)
    EVT_BUTTON(wxID_OK, nLWShaderPanel::OnOKBtn)
    EVT_BUTTON(wxID_CANCEL, nLWShaderPanel::OnCancelBtn)
    EVT_CHOICE(ID_SHADER_POPUP, nLWShaderPanel::OnShaderPopup)
END_EVENT_TABLE() 

//----------------------------------------------------------------------------
/**
*/
nLWShaderPanel::nLWShaderPanel(wxWindow* parent, 
                               const nLWShaderPanelTemplate* panelTemplate,
                               const nArray<nString>& shaderNames) :
    wxDialog(parent, wxID_ANY, "Shader Settings...", wxDefaultPosition),
    shaderPopup(0),
    curShaderSettings(0)
{
    // need this to make validators work properly in complex controls
    this->SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);

    this->shaderName = panelTemplate->GetShaderName();
    this->shaderFile = panelTemplate->GetShaderFile();

    // copy the shader params from the template
    const nArray<nLWShaderPanelParam*>& templateParamArray = panelTemplate->GetShaderParamArray();
    for (int i = 0; i < templateParamArray.Size(); i++)
    {
        this->shaderParamArray.Append(templateParamArray[i]->Clone());
    }

    // populate dialog with controls based on the template
    wxGridBagSizer* topSizer = new wxGridBagSizer(5, 5);

    // row 0
    wxStaticText* shaderNameLabel = new wxStaticText(this, wxID_ANY, "Shader");
    this->shaderPopup = new wxChoice(this, ID_SHADER_POPUP);
    for (int i = 0; i < shaderNames.Size(); i++)
    {
        this->shaderPopup->AppendString(shaderNames[i].Get());
    }
    this->shaderPopup->SetStringSelection(this->shaderName.Get());

    topSizer->Add(shaderNameLabel, wxGBPosition(0, 0), wxDefaultSpan, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP, 10);
    topSizer->Add(this->shaderPopup, wxGBPosition(0, 1), wxDefaultSpan, wxRIGHT|wxTOP, 10);

    // shader controls
    int sizerRow = 1;
    for (int i = 0; i < this->shaderParamArray.Size(); i++)
    {
        nLWShaderPanelParam* param = this->shaderParamArray[i];
        if (param->Visible())
        {
            if (param->NeedsLabelControl())
            {
                wxStaticText* labelCtrl = new wxStaticText(this, wxID_ANY, param->GetLabel().Get());
                if (labelCtrl)
                {
                    topSizer->Add(labelCtrl, wxGBPosition(sizerRow, 0), wxDefaultSpan, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT, 10);
                }
            }
            topSizer->Add(param->CreateControl(this), wxGBPosition(sizerRow, 1), wxDefaultSpan, wxEXPAND|wxRIGHT, 10);
            ++sizerRow;
        }
    }

    // last row
    topSizer->Add(CreateButtonSizer(wxOK|wxCANCEL), 
                  wxGBPosition(sizerRow, 0), wxGBSpan(1, 2), 
                  wxALIGN_CENTER_HORIZONTAL|wxALL, 10);

    topSizer->Fit(this);
    this->SetSizer(topSizer);
}

//----------------------------------------------------------------------------
/**
*/
nLWShaderPanel::~nLWShaderPanel()
{
    for (int i = 0; i < this->shaderParamArray.Size(); i++)
    {
        n_delete(this->shaderParamArray[i]);
    }
    this->shaderParamArray.Reset();
}

//----------------------------------------------------------------------------
/**
*/
void
nLWShaderPanel::ResetShaderPopup()
{
    if (this->shaderPopup)
    {
        this->shaderPopup->SetStringSelection(this->shaderName.Get());
    }
}

//----------------------------------------------------------------------------
/**
*/
void 
nLWShaderPanel::SetShaderSettings(nLWShaderExportSettings* shaderSettings)
{
    this->curShaderSettings = shaderSettings;

    // copy the values from the shader settings to the controls
    if (shaderSettings && (shaderSettings->GetShaderName() == this->shaderName))
    {
        for (int i = 0; i < this->shaderParamArray.Size(); i++)
        {
            this->shaderParamArray[i]->CopyParamFrom(shaderSettings);
        }
    }

    // update nViewer to match current shader settings if necessary
    if (nLWViewerRemote::Instance()->IsOpen())
    {
        for (int i = 0; i < this->shaderParamArray.Size(); i++)
        {
            this->shaderParamArray[i]->SendParamToViewer();
        }
    }
}

//----------------------------------------------------------------------------
/**
    @brief Called when the user selects a new shader from the shader popup. 
    
    The current panel will be closed and a new panel will be displayed.
*/
void
nLWShaderPanel::OnShaderPopup(wxCommandEvent& WXUNUSED(event))
{
    nLWShaderPanelFactory* factory = nLWShaderPanelFactory::Instance();
    n_assert(factory);
    if (factory)
    {
        factory->SetNextShaderPanelToDisplay(this->shaderPopup->GetStringSelection().c_str());
    }
    this->EndModal(wxCANCEL);
}

//----------------------------------------------------------------------------
/**
*/
void
nLWShaderPanel::OnOKBtn(wxCommandEvent& WXUNUSED(event))
{
    // copy the values from the controls to the shader settings
    if (this->curShaderSettings)
    {
        this->curShaderSettings->Clear();
        this->curShaderSettings->SetShaderName(this->shaderName);
        this->curShaderSettings->SetShaderFile(this->shaderFile);
        for (int i = 0; i < this->shaderParamArray.Size(); i++)
        {
            this->shaderParamArray[i]->CopyParamTo(this->curShaderSettings);
        }
    }
    this->EndModal(wxOK);
}

//----------------------------------------------------------------------------
/**
*/
void
nLWShaderPanel::OnCancelBtn(wxCommandEvent& WXUNUSED(event))
{
    // TODO: update nViewer with the original settings for the shader
    this->EndModal(wxCANCEL);
}

//----------------------------------------------------------------------------
// EOF
//----------------------------------------------------------------------------
