//----------------------------------------------------------------------------
// (c) 2006    Vadim Macagon
//----------------------------------------------------------------------------
#include "lwexporter/nlwexportnodeconfigpanel.h"
#include "lwexporter/nlwexportnodetype.h"
#include "tools/nmeshbuilder.h"
#include "lwexporter/nlwexportersettings.h"
#include "wx/statline.h"

//----------------------------------------------------------------------------
enum
{
    ID_EXPORT_TYPE_POPUP = wxID_HIGHEST + 1,
    ID_NEW_EXPORT_TYPE,
    ID_REN_EXPORT_TYPE,
    ID_DEL_EXPORT_TYPE,
};

BEGIN_EVENT_TABLE(nLWExportNodeConfigPanel, wxDialog)
    EVT_CHOICE(ID_EXPORT_TYPE_POPUP, nLWExportNodeConfigPanel::OnExportTypePopup)
    EVT_BUTTON(ID_NEW_EXPORT_TYPE, nLWExportNodeConfigPanel::OnNewExportTypeBtn)
    EVT_BUTTON(ID_REN_EXPORT_TYPE, nLWExportNodeConfigPanel::OnRenExportTypeBtn)
    EVT_BUTTON(ID_DEL_EXPORT_TYPE, nLWExportNodeConfigPanel::OnDelExportTypeBtn)
    EVT_BUTTON(wxID_OK, nLWExportNodeConfigPanel::OnOKBtn)
    EVT_BUTTON(wxID_CANCEL, nLWExportNodeConfigPanel::OnCancelBtn)
END_EVENT_TABLE()

//----------------------------------------------------------------------------
/**
*/
nLWExportNodeConfigPanel::nLWExportNodeConfigPanel(wxWindow* parent) :
    wxDialog(parent, wxID_ANY, "Configure Export Node Types...", wxDefaultPosition),
    exportNodeTypePopup(0),
    renameExportNodeTypeBtn(0),
    deleteExportNodeTypeBtn(0),
    sceneNodeTypePopup(0),
    texturesCheckBox(0),
    animationCheckBox(0),
    coordCheckBox(0),
    tangentCheckBox(0),
    binormalCheckBox(0),
    normalCheckBox(0),
    uv0CheckBox(0),
    uv1CheckBox(0),
    uv2CheckBox(0),
    uv3CheckBox(0),
    weightsCheckBox(0),
    colorCheckBox(0),
    curExportNodeType(0)
{
    wxStaticText* exportNodeTypeLabel = new wxStaticText(this, wxID_ANY, "Export Node Type");
    this->exportNodeTypePopup = new wxChoice(this, ID_EXPORT_TYPE_POPUP);
    wxButton* newExportNodeTypeBtn = new wxButton(this, ID_NEW_EXPORT_TYPE, "New");
    this->renameExportNodeTypeBtn = new wxButton(this, ID_REN_EXPORT_TYPE, "Rename");
    this->deleteExportNodeTypeBtn = new wxButton(this, ID_DEL_EXPORT_TYPE, "Delete");

    // line to separate export node controls from scene node controls
    wxStaticLine* staticLine = new wxStaticLine(this);

    wxStaticText* sceneNodeTypeLabel = new wxStaticText(this, wxID_ANY, "Scene Node Type");
    this->sceneNodeTypePopup = new wxChoice(this, wxID_ANY);

    wxStaticBox* exportBox = new wxStaticBox(this, wxID_ANY, "Export");

    this->texturesCheckBox = new wxCheckBox(this, wxID_ANY, "Textures");
    this->animationCheckBox = new wxCheckBox(this, wxID_ANY, "Animation");

    wxStaticBox* vertexBox = new wxStaticBox(this, wxID_ANY, "Vertex Components");

    this->coordCheckBox = new wxCheckBox(this, wxID_ANY, "Coord");
    this->tangentCheckBox = new wxCheckBox(this, wxID_ANY, "Tangent");
    this->binormalCheckBox = new wxCheckBox(this, wxID_ANY, "Binormal");
    this->normalCheckBox = new wxCheckBox(this, wxID_ANY, "Normal");
    this->uv0CheckBox = new wxCheckBox(this, wxID_ANY, "UV 0");
    this->uv1CheckBox = new wxCheckBox(this, wxID_ANY, "UV 1");
    this->uv2CheckBox = new wxCheckBox(this, wxID_ANY, "UV 2");
    this->uv3CheckBox = new wxCheckBox(this, wxID_ANY, "UV 3");
    this->weightsCheckBox = new wxCheckBox(this, wxID_ANY, "Joint Weights");
    this->colorCheckBox = new wxCheckBox(this, wxID_ANY, "Color");

    // populate controls

    nLWExporterSettings* settings = nLWExporterSettings::Instance();
    n_assert(settings);
    if (settings)
    {
        int numExportNodeTypes = settings->GetNumExportNodeTypes();
        if (numExportNodeTypes)
        {
            for (int i = 0; i < numExportNodeTypes; i++)
            {
                this->exportNodeTypePopup->AppendString(settings->GetExportNodeType(i)->GetName().Get());
                // make a local copy of the export node type
                this->exportTypeArray.PushBack(n_new(nLWExportNodeType(*settings->GetExportNodeType(i))));
            }
            this->curExportNodeType = settings->GetExportNodeType(0);
            this->exportNodeTypePopup->Select(0);
        }

        int numSceneNodeTypes = settings->GetNumSceneNodeTypes();
        for (int i = 0; i < numSceneNodeTypes; i++)
        {
            this->sceneNodeTypePopup->AppendString(settings->GetSceneNodeType(i).Get());
        }

        this->UpdateControls();
    }

    // layout controls

    wxStaticBoxSizer* vertexBoxSizer = new wxStaticBoxSizer(vertexBox, wxVERTICAL);
    vertexBoxSizer->Add(this->coordCheckBox);
    vertexBoxSizer->Add(this->tangentCheckBox);
    vertexBoxSizer->Add(this->binormalCheckBox);
    vertexBoxSizer->Add(this->normalCheckBox);
    vertexBoxSizer->Add(this->uv0CheckBox);
    vertexBoxSizer->Add(this->uv1CheckBox);
    vertexBoxSizer->Add(this->uv2CheckBox);
    vertexBoxSizer->Add(this->uv3CheckBox);
    vertexBoxSizer->Add(this->weightsCheckBox);
    vertexBoxSizer->Add(this->colorCheckBox);

    wxStaticBoxSizer* exportBoxSizer = new wxStaticBoxSizer(exportBox, wxVERTICAL);
    exportBoxSizer->Add(this->texturesCheckBox);
    exportBoxSizer->Add(this->animationCheckBox);
    exportBoxSizer->AddSpacer(5);
    exportBoxSizer->Add(vertexBoxSizer, 0, wxEXPAND);

    wxBoxSizer* exportNodeTypeBtnSizer = new wxBoxSizer(wxHORIZONTAL);
    exportNodeTypeBtnSizer->Add(newExportNodeTypeBtn, 1);
    exportNodeTypeBtnSizer->Add(this->renameExportNodeTypeBtn, 1);
    exportNodeTypeBtnSizer->Add(this->deleteExportNodeTypeBtn, 1);

    wxBoxSizer* topSizer = new wxBoxSizer(wxVERTICAL);
    topSizer->Add(exportNodeTypeLabel, 0, wxLEFT|wxRIGHT, 10);
    topSizer->AddSpacer(5);
    topSizer->Add(this->exportNodeTypePopup, 0, wxEXPAND|wxLEFT|wxRIGHT, 10);
    topSizer->AddSpacer(5);
    topSizer->Add(exportNodeTypeBtnSizer, 0, wxEXPAND|wxLEFT|wxRIGHT, 10);
    topSizer->AddSpacer(5);
    topSizer->Add(staticLine, 0, wxEXPAND);
    topSizer->AddSpacer(5);
    topSizer->Add(sceneNodeTypeLabel, 0, wxLEFT|wxRIGHT, 10);
    topSizer->AddSpacer(5);
    topSizer->Add(this->sceneNodeTypePopup, 0, wxEXPAND|wxLEFT|wxRIGHT, 10);
    topSizer->AddSpacer(5);
    topSizer->Add(exportBoxSizer, 0, wxEXPAND|wxLEFT|wxRIGHT, 10);
    topSizer->Add(CreateButtonSizer(wxOK|wxCANCEL), 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 10);

    topSizer->Fit(this);
    this->SetSizer(topSizer);
}

//----------------------------------------------------------------------------
/**
*/
nLWExportNodeConfigPanel::~nLWExportNodeConfigPanel()
{
    for (int i = 0; i < this->exportTypeArray.Size(); i++)
    {
        n_delete(this->exportTypeArray[i]);
    }
    this->exportTypeArray.Clear();
}

//----------------------------------------------------------------------------
/**
*/
nLWExportNodeType*
nLWExportNodeConfigPanel::GetExportNodeType(const nString& typeName)
{
    for (int i = 0; i < this->exportTypeArray.Size(); i++)
    {
        if (typeName == this->exportTypeArray[i]->GetName())
            return this->exportTypeArray[i];
    }

    return 0;
}

//----------------------------------------------------------------------------
/**
*/
void
nLWExportNodeConfigPanel::StoreCurrentData()
{
    if (!this->curExportNodeType)
        return;

    nString curTypeName(this->exportNodeTypePopup->GetStringSelection().c_str());

    nLWExporterSettings* settings = nLWExporterSettings::Instance();
    n_assert(settings);
    if (!settings)
        return;

    nLWExportNodeType* changedType = this->curExportNodeType;
    changedType->SetSceneNodeType(this->sceneNodeTypePopup->GetStringSelection().c_str());
    changedType->SetExportTextures(this->texturesCheckBox->GetValue());
    changedType->SetExportAnimation(this->animationCheckBox->GetValue());
    int mask = 0;
    if (this->coordCheckBox->GetValue())
    {
        mask |= nMeshBuilder::Vertex::COORD;
    }
    if (this->tangentCheckBox->GetValue())
    {
        mask |= nMeshBuilder::Vertex::TANGENT;
    }
    if (this->binormalCheckBox->GetValue())
    {
        mask |= nMeshBuilder::Vertex::BINORMAL;
    }
    if (this->normalCheckBox->GetValue())
    {
        mask |= nMeshBuilder::Vertex::NORMAL;
    }
    if (this->uv0CheckBox->GetValue())
    {
        mask |= nMeshBuilder::Vertex::UV0;
    }
    if (this->uv1CheckBox->GetValue())
    {
        mask |= nMeshBuilder::Vertex::UV1;
    }
    if (this->uv2CheckBox->GetValue())
    {
        mask |= nMeshBuilder::Vertex::UV2;
    }
    if (this->uv3CheckBox->GetValue())
    {
        mask |= nMeshBuilder::Vertex::UV3;
    }
    if (this->weightsCheckBox->GetValue())
    {
        mask |= (nMeshBuilder::Vertex::WEIGHTS | nMeshBuilder::Vertex::JINDICES);
    }
    if (this->colorCheckBox->GetValue())
    {
        mask |= nMeshBuilder::Vertex::COLOR;
    }
    changedType->SetVertexComponentMask(mask);
}

//----------------------------------------------------------------------------
/**
*/
void
nLWExportNodeConfigPanel::UpdateControls()
{
    nLWExportNodeType* nodeType = this->curExportNodeType;
    if (nodeType)
    {
        this->renameExportNodeTypeBtn->Enable(true);
        this->deleteExportNodeTypeBtn->Enable(true);
        this->sceneNodeTypePopup->Enable(true);
        this->sceneNodeTypePopup->SetStringSelection(nodeType->GetSceneNodeType().Get());
        this->texturesCheckBox->Enable(true);
        this->texturesCheckBox->SetValue(nodeType->GetExportTextures());
        this->animationCheckBox->Enable(true);
        this->animationCheckBox->SetValue(nodeType->GetExportAnimation());
        int mask = nodeType->GetVertexComponentMask();
        this->coordCheckBox->Enable(true);
        this->coordCheckBox->SetValue((mask & nMeshBuilder::Vertex::COORD) != 0);
        this->tangentCheckBox->Enable(true);
        this->tangentCheckBox->SetValue((mask & nMeshBuilder::Vertex::TANGENT) != 0);
        this->binormalCheckBox->Enable(true);
        this->binormalCheckBox->SetValue((mask & nMeshBuilder::Vertex::BINORMAL) != 0);
        this->normalCheckBox->Enable(true);
        this->normalCheckBox->SetValue((mask & nMeshBuilder::Vertex::NORMAL) != 0);
        this->uv0CheckBox->Enable(true);
        this->uv0CheckBox->SetValue((mask & nMeshBuilder::Vertex::UV0) != 0);
        this->uv1CheckBox->Enable(true);
        this->uv1CheckBox->SetValue((mask & nMeshBuilder::Vertex::UV1) != 0);
        this->uv2CheckBox->Enable(true);
        this->uv2CheckBox->SetValue((mask & nMeshBuilder::Vertex::UV2) != 0);
        this->uv3CheckBox->Enable(true);
        this->uv3CheckBox->SetValue((mask & nMeshBuilder::Vertex::UV3) != 0);
        this->weightsCheckBox->Enable(true);
        this->weightsCheckBox->SetValue((mask & nMeshBuilder::Vertex::WEIGHTS) != 0);
        this->colorCheckBox->Enable(true);
        this->colorCheckBox->SetValue((mask & nMeshBuilder::Vertex::COLOR) != 0);
    }
    else
    {
        this->renameExportNodeTypeBtn->Enable(false);
        this->deleteExportNodeTypeBtn->Enable(false);
        this->sceneNodeTypePopup->Enable(false);
        this->texturesCheckBox->Enable(false);
        this->animationCheckBox->Enable(false);
        this->coordCheckBox->Enable(false);
        this->tangentCheckBox->Enable(false);
        this->binormalCheckBox->Enable(false);
        this->normalCheckBox->Enable(false);
        this->uv0CheckBox->Enable(false);
        this->uv1CheckBox->Enable(false);
        this->uv2CheckBox->Enable(false);
        this->uv3CheckBox->Enable(false);
        this->weightsCheckBox->Enable(false);
        this->colorCheckBox->Enable(false);
    }
}

//----------------------------------------------------------------------------
/**
*/
void
nLWExportNodeConfigPanel::SaveChanges()
{
    nLWExporterSettings* settings = nLWExporterSettings::Instance();
    n_assert(settings);
    if (!settings)
        return;

    settings->SetExportNodeTypes(this->exportTypeArray);
}

//----------------------------------------------------------------------------
/**
*/
void
nLWExportNodeConfigPanel::OnExportTypePopup(wxCommandEvent& event)
{
    nLWExporterSettings* settings = nLWExporterSettings::Instance();
    n_assert(settings);
    if (!settings)
        return;

    this->StoreCurrentData();

    nString oldSelection;
    if (this->curExportNodeType)
    {
        oldSelection = this->curExportNodeType->GetName();
    }
    nString newSelection(event.GetString().c_str());
    if (!oldSelection.IsEmpty() && (oldSelection != newSelection))
    {
        this->curExportNodeType = this->GetExportNodeType(newSelection);
        this->UpdateControls();
    }
}

//----------------------------------------------------------------------------
/**
*/
void
nLWExportNodeConfigPanel::OnNewExportTypeBtn(wxCommandEvent& WXUNUSED(event))
{
    wxTextEntryDialog dialog(this, "Name", "New Export Node Type");
    if (dialog.ShowModal() == wxID_OK)
    {
        nString typeName(dialog.GetValue().c_str());

        if (typeName.IsEmpty())
        {
            wxMessageBox("You must provide a name for the new export node type.",
                         "Error");
            return;
        }

        if (this->GetExportNodeType(typeName))
        {
            wxMessageBox("An export node type with this name already exists.",
                         "Error");
            return;
        }

        this->curExportNodeType = n_new(nLWExportNodeType());
        this->curExportNodeType->SetName(typeName);
        this->exportNodeTypePopup->AppendString(typeName.Get());
        this->exportNodeTypePopup->SetStringSelection(typeName.Get());
        this->exportTypeArray.PushBack(this->curExportNodeType);
        this->UpdateControls();
    }
}

//----------------------------------------------------------------------------
/**
*/
void
nLWExportNodeConfigPanel::OnRenExportTypeBtn(wxCommandEvent& WXUNUSED(event))
{
    if (!this->curExportNodeType)
        return;

    wxTextEntryDialog dialog(this, "New Name", "Rename Export Node Type",
                             this->exportNodeTypePopup->GetStringSelection());
    if (dialog.ShowModal() == wxID_OK)
    {
        nString newTypeName(dialog.GetValue().c_str());
        if (this->curExportNodeType->GetName() != newTypeName)
        {
            for (int i = 0; i < this->exportTypeArray.Size(); i++)
            {
                if (newTypeName == this->exportTypeArray[i]->GetName())
                {
                    wxMessageBox("An export node type with this name already exists.",
                                 "Error");
                    return;
                }
            }
            this->curExportNodeType->SetName(newTypeName);
            int curSelection = this->exportNodeTypePopup->GetSelection();
            this->exportNodeTypePopup->SetString(curSelection, newTypeName.Get());
            this->exportNodeTypePopup->SetSelection(curSelection);
        }
    }
}

//----------------------------------------------------------------------------
/**
*/
void
nLWExportNodeConfigPanel::OnDelExportTypeBtn(wxCommandEvent& WXUNUSED(event))
{
    if (!this->curExportNodeType)
        return;

    nString curTypeName(this->exportNodeTypePopup->GetStringSelection().c_str());

    for (int i = 0; i < this->exportTypeArray.Size(); i++)
    {
        if (curTypeName == this->exportTypeArray[i]->GetName())
        {
            n_delete(this->exportTypeArray[i]);
            this->exportTypeArray[i] = 0;
            this->exportTypeArray.Erase(i);
            break;
        }
    }

    this->exportNodeTypePopup->Delete(this->exportNodeTypePopup->GetSelection());

    if (this->exportNodeTypePopup->GetCount() == 0)
    {
        this->curExportNodeType = 0;
    }
    else
    {
        this->exportNodeTypePopup->SetSelection(0);
        nString curType(this->exportNodeTypePopup->GetStringSelection().c_str());
        this->curExportNodeType = this->GetExportNodeType(curType);
    }
    this->UpdateControls();
}

//----------------------------------------------------------------------------
/**
*/
void
nLWExportNodeConfigPanel::OnOKBtn(wxCommandEvent& WXUNUSED(event))
{
    this->StoreCurrentData();
    // propagate changes to the exporter settings singleton
    this->SaveChanges();
    this->EndModal(wxOK);
}

//----------------------------------------------------------------------------
/**
*/
void
nLWExportNodeConfigPanel::OnCancelBtn(wxCommandEvent& WXUNUSED(event))
{
    this->EndModal(wxCANCEL);
}

//----------------------------------------------------------------------------
// EOF
//----------------------------------------------------------------------------
