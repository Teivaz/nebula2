//----------------------------------------------------------------------------
// (c) 2006    Vadim Macagon
//----------------------------------------------------------------------------
#include "lwexporter/nlwobjectexportsettings_ui.h"
#include "lwexporter/nlwobjectexportsettings.h"
#include "lwexporter/nlwexportersettings.h"
#include "lwexporter/nlwexportnodetype.h"
#include "lwexporter/nlwanimstatepanel.h"
#include "lwwrapper/nlwglobals.h"
#include "kernel/nfileserver2.h"
#include "lwwxui/wxanimlistctrl.h"
#include "wx/gbsizer.h"

//----------------------------------------------------------------------------
namespace
{
    //------------------------------------------------------------------------
    /**
        @brief Create animation states from MotionMixer motions in the scene.
    */
    void
    GetAnimStatesFromMotionMixer(nArray<nLWAnimationState>& array)
    {
        /*
        The MotionMixer data we want is stored in the scene file in this format:
        TrackMotionName <Motion Name>
        ...
        TrackMotionStartFrame <Frame #>
        TrackMotionEndFrame <Frame #>
        */

        nLWGlobals::SceneInfo sceneInfo;
        n_assert(sceneInfo.IsValid());
        if (!sceneInfo.IsValid())
            return;

        nFileServer2* fs = nFileServer2::Instance();
        if (!fs)
            return;

        nFile* sceneFile = fs->NewFileObject();
        if (!sceneFile->Open(sceneInfo.Get()->filename, "r"))
            return;

        char buffer[1024];
        nString line;
        nString firstToken;
        nString secondToken;
        const char* space = " ";
        nLWAnimationState curAnimState;
        while (sceneFile->GetS(buffer, sizeof(buffer)))
        {
            line = buffer;
            line = line.Trim(" \r\n");
            firstToken = line.GetFirstToken(space);
            if ("TrackMotionName" == firstToken)
            {
                secondToken = line.GetNextToken(space);
                // name is quoted in the scene file, we don't want the quotes
                secondToken = secondToken.Trim("\"");
                curAnimState.name = secondToken;
                curAnimState.fadeInTime = 0.0f;
                curAnimState.repeat = false;
            }
            else if ("TrackMotionStartFrame" == firstToken)
            {
                secondToken = line.GetNextToken(space);
                curAnimState.startFrame = secondToken.AsInt();
            }
            else if ("TrackMotionEndFrame" == firstToken)
            {
                secondToken = line.GetNextToken(space);
                curAnimState.endFrame = secondToken.AsInt();
                array.PushBack(curAnimState);
            }
        }

        sceneFile->Close();
    }

    //------------------------------------------------------------------------
    /**
        Column indices for the animation state multi-list box.
    */
    enum AnimListBoxColumns
    {
        ANIM_LIST_NAME_COL = 0,
        ANIM_LIST_START_COL,
        ANIM_LIST_END_COL,
        ANIM_LIST_FADEIN_COL,
        ANIM_LIST_LOOP_COL
    };

} // anonymous namespace

//----------------------------------------------------------------------------
enum
{
    ID_SCAN_MM_BTN = wxID_HIGHEST + 1,
    ID_NEW_ANIM_BTN,
    ID_EDIT_ANIM_BTN,
    ID_DEL_ANIM_BTN,
    ID_MOVE_ANIM_UP_BTN,
    ID_MOVE_ANIM_DOWN_BTN,
    ID_ANIM_LIST,
    ID_EXPORT_TYPE_POPUP,
};

BEGIN_EVENT_TABLE(nLWObjectExportSettingsPanel, wxDialog)
    EVT_BUTTON(wxID_OK, nLWObjectExportSettingsPanel::OnOKBtn)
    EVT_BUTTON(wxID_CANCEL, nLWObjectExportSettingsPanel::OnCancelBtn)
    EVT_BUTTON(ID_SCAN_MM_BTN, nLWObjectExportSettingsPanel::OnScanMMBtn)
    EVT_BUTTON(ID_NEW_ANIM_BTN, nLWObjectExportSettingsPanel::OnNewAnimStateBtn)
    EVT_BUTTON(ID_EDIT_ANIM_BTN, nLWObjectExportSettingsPanel::OnEditAnimStateBtn)
    EVT_BUTTON(ID_DEL_ANIM_BTN, nLWObjectExportSettingsPanel::OnDeleteAnimStateBtn)
    EVT_BUTTON(ID_MOVE_ANIM_UP_BTN, nLWObjectExportSettingsPanel::OnMoveAnimStateUpBtn)
    EVT_BUTTON(ID_MOVE_ANIM_DOWN_BTN, nLWObjectExportSettingsPanel::OnMoveAnimStateDownBtn)
    EVT_LIST_ITEM_SELECTED(ID_ANIM_LIST, nLWObjectExportSettingsPanel::OnAnimListChanged)
    EVT_CHOICE(ID_EXPORT_TYPE_POPUP, nLWObjectExportSettingsPanel::OnExportTypePopup)
END_EVENT_TABLE() 

//----------------------------------------------------------------------------
/**
*/
nLWObjectExportSettingsPanel::nLWObjectExportSettingsPanel(wxWindow* parent,
                                                           nLWObjectExportSettings* settings) :
    wxDialog(parent, wxID_ANY, "Object Export Settings", wxDefaultPosition),
    itemNameTextBox(0),
    nodeTypePopup(0),
    nodeHierarchyCheckBox(0),
    animVarTextBox(0),
    scanMotionMixerBtn(0),
    animMultiListBox(0),
    newAnimBtn(0),
    editAnimBtn(0),
    deleteAnimBtn(0),
    moveAnimUpBtn(0),
    moveAnimDownBtn(0),
    settings(settings)
{
    this->itemNameTextBox = new wxStaticText(this, wxID_ANY, this->itemName.Get());

    // export node group box controls

    wxStaticBox* nodeGroupBox = new wxStaticBox(this, wxID_ANY, "Export Node");

    wxStaticText* nodeTypeLabel = new wxStaticText(this, wxID_ANY, "Type");
    this->nodeTypePopup = new wxChoice(this, ID_EXPORT_TYPE_POPUP);
    this->nodeHierarchyCheckBox = new wxCheckBox(this, wxID_ANY, "Export Hierarchy");

    // animation box controls

    wxStaticBox* animGroupBox = new wxStaticBox(this, wxID_ANY, "Animation");

    wxStaticText* animVarLabel = new wxStaticText(this, wxID_ANY, "Animation State Variable");
    this->animVarTextBox = new wxTextCtrl(this, wxID_ANY);
    this->scanMotionMixerBtn = new wxButton(this, ID_SCAN_MM_BTN, "Scan MotionMixer");

    this->animMultiListBox = new wxAnimListCtrl(this, ID_ANIM_LIST, 
                                                wxDefaultPosition, wxSize(380, 300));

    this->newAnimBtn = new wxButton(this, ID_NEW_ANIM_BTN, "New");
    this->editAnimBtn = new wxButton(this, ID_EDIT_ANIM_BTN, "Edit");
    this->deleteAnimBtn = new wxButton(this, ID_DEL_ANIM_BTN, "Delete");
    this->moveAnimUpBtn = new wxButton(this, ID_MOVE_ANIM_UP_BTN, "Up");
    this->moveAnimDownBtn = new wxButton(this, ID_MOVE_ANIM_DOWN_BTN, "Down");

    // populate controls

    nLWExporterSettings* exporterSettings = nLWExporterSettings::Instance();
    n_assert(exporterSettings);
    if (exporterSettings)
    {
        int numExportNodeTypes = exporterSettings->GetNumExportNodeTypes();
        if (numExportNodeTypes)
        {
            for (int i = 0; i < numExportNodeTypes; i++)
            {
                this->nodeTypePopup->AppendString(exporterSettings->GetExportNodeType(i)->GetName().Get());
            }
        }
        else
        {
            this->nodeTypePopup->AppendString("None");
        }
    }

    this->Load();

    // layout controls
    
    wxBoxSizer* animBtnSizer = new wxBoxSizer(wxHORIZONTAL);
    animBtnSizer->Add(this->newAnimBtn);
    animBtnSizer->Add(this->editAnimBtn);
    animBtnSizer->Add(this->deleteAnimBtn);
    animBtnSizer->Add(this->moveAnimUpBtn);
    animBtnSizer->Add(this->moveAnimDownBtn);

    wxGridBagSizer* animGroupBoxSizer = new wxGridBagSizer(5, 5);
    animGroupBoxSizer->Add(animVarLabel, wxGBPosition(0, 0));
    animGroupBoxSizer->Add(this->animVarTextBox, wxGBPosition(0, 1));
    animGroupBoxSizer->Add(scanMotionMixerBtn, wxGBPosition(1, 0));
    animGroupBoxSizer->Add(this->animMultiListBox, wxGBPosition(2, 0), wxGBSpan(1, 2), wxEXPAND);
    animGroupBoxSizer->Add(animBtnSizer, wxGBPosition(3, 0), wxGBSpan(1, 2), wxALIGN_CENTER_HORIZONTAL);

    wxStaticBoxSizer* animGroupBoxBorderSizer = new wxStaticBoxSizer(animGroupBox, wxVERTICAL);
    animGroupBoxBorderSizer->Add(animGroupBoxSizer);

    wxGridBagSizer* nodeGroupBoxSizer = new wxGridBagSizer(5, 5);
    nodeGroupBoxSizer->Add(nodeTypeLabel, wxGBPosition(0, 0), wxDefaultSpan, wxALIGN_RIGHT);
    nodeGroupBoxSizer->Add(this->nodeTypePopup, wxGBPosition(0, 1));
    nodeGroupBoxSizer->Add(this->nodeHierarchyCheckBox, wxGBPosition(1, 1));
    wxStaticBoxSizer* nodeGroupBoxBorderSizer = new wxStaticBoxSizer(nodeGroupBox, wxVERTICAL);
    nodeGroupBoxBorderSizer->Add(nodeGroupBoxSizer);

    wxBoxSizer* topSizer = new wxBoxSizer(wxVERTICAL);
    topSizer->Add(this->itemNameTextBox);
    topSizer->Add(nodeGroupBoxBorderSizer, 0, wxEXPAND);
    topSizer->Add(animGroupBoxBorderSizer, 0, wxEXPAND);
    topSizer->Add(CreateButtonSizer(wxOK|wxCANCEL), 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 10);
    
    topSizer->Fit(this);
    this->SetSizer(topSizer);
}

//----------------------------------------------------------------------------
/**
*/
nLWObjectExportSettingsPanel::~nLWObjectExportSettingsPanel()
{
    // empty
}

//------------------------------------------------------------------------
/**
    @brief Create animation states from the animation states multi-list box.
*/
void
nLWObjectExportSettingsPanel::GetAnimStatesFromListBox(nArray<nLWAnimationState>& array)
{
    int numRows = this->animMultiListBox->GetItemCount();
    nLWAnimationState animState;
    for (int row = 0; row < numRows; row++)
    {
        this->animMultiListBox->GetAnim(row, animState);
        array.PushBack(animState);
    }
}

//----------------------------------------------------------------------------
/**
    @brief Populate the animation state multi-list box from the given array.
*/
void 
nLWObjectExportSettingsPanel::FillListBoxWithAnimStates(const nArray<nLWAnimationState>& array)
{
    this->animMultiListBox->ClearAnims();
    for (int i = 0; i < array.Size(); i++)
    {
        this->animMultiListBox->AddAnim(array[i]);
    }
}

//----------------------------------------------------------------------------
/**
*/
void 
nLWObjectExportSettingsPanel::SetItemName(const nString& itemName)
{
    this->itemName = itemName;
    if (this->itemNameTextBox)
        this->itemNameTextBox->SetLabel(itemName.Get());
}

//----------------------------------------------------------------------------
/**
*/
void 
nLWObjectExportSettingsPanel::Save()
{
    n_assert(this->settings);
    if (!this->settings)
        return;

    nLWObjectExportSettings* s = this->settings;
    
    // scene node group box
    s->SetExportNodeType(this->nodeTypePopup->GetStringSelection().c_str());
    s->SetExportSceneNodeHierarchy(this->nodeHierarchyCheckBox->GetValue());

    // animation stuff
    s->SetAnimationVarName(this->animVarTextBox->GetValue().c_str());
    nArray<nLWAnimationState> animStateArray;
    this->GetAnimStatesFromListBox(animStateArray);
    s->SetAnimationStates(animStateArray);
}

//----------------------------------------------------------------------------
/**
*/
void 
nLWObjectExportSettingsPanel::Load()
{
    n_assert(this->settings);
    if (!this->settings)
        return;

    nLWObjectExportSettings* s = this->settings;

    // scene node group box
    this->nodeTypePopup->SetStringSelection(s->GetExportNodeType().Get());
    this->nodeHierarchyCheckBox->SetValue(s->GetExportSceneNodeHierarchy());

    // animation stuff
    this->animVarTextBox->SetValue(s->GetAnimationVarName().Get());
    this->FillListBoxWithAnimStates(s->GetAnimationStates());

    this->RefreshAnimControls();
}

//----------------------------------------------------------------------------
/**
*/
bool
nLWObjectExportSettingsPanel::NeedAnimControls() const
{
    nString expTypeName(this->nodeTypePopup->GetStringSelection().c_str());
    if (!expTypeName.IsEmpty())
    {
        nLWExporterSettings* exps = nLWExporterSettings::Instance();
        nLWExportNodeType* expType = exps->GetExportNodeType(expTypeName);
        if (expType)
        {
            return expType->GetExportAnimation();
        }
    }
    return false;
}

//----------------------------------------------------------------------------
/**
*/
void 
nLWObjectExportSettingsPanel::RefreshAnimControls()
{
    if (this->NeedAnimControls())
    {
        this->animVarTextBox->Enable(true);
        this->scanMotionMixerBtn->Enable(true);
        this->animMultiListBox->Enable(true);
        this->newAnimBtn->Enable(true);

        if (this->animMultiListBox->GetSelectedItemCount() > 0)
        {
            this->editAnimBtn->Enable(true);
            this->deleteAnimBtn->Enable(true);
            this->moveAnimUpBtn->Enable(true);
            this->moveAnimDownBtn->Enable(true);
        }
        else
        {
            // disable the all buttons on the bottom row except the 'new' button
            this->editAnimBtn->Enable(false);
            this->deleteAnimBtn->Enable(false);
            this->moveAnimUpBtn->Enable(false);
            this->moveAnimDownBtn->Enable(false);
        }
    }
    else
    {
        this->animVarTextBox->Enable(false);
        this->scanMotionMixerBtn->Enable(false);
        this->animMultiListBox->Enable(false);
        this->newAnimBtn->Enable(false);
        this->editAnimBtn->Enable(false);
        this->deleteAnimBtn->Enable(false);
        this->moveAnimUpBtn->Enable(false);
        this->moveAnimDownBtn->Enable(false);
    }
}

//----------------------------------------------------------------------------
/**
*/
void
nLWObjectExportSettingsPanel::OnScanMMBtn(wxCommandEvent& WXUNUSED(event))
{
    nArray<nLWAnimationState> mixerAnimStateArray;
    GetAnimStatesFromMotionMixer(mixerAnimStateArray);

    nArray<nLWAnimationState> boxAnimStateArray;
    this->GetAnimStatesFromListBox(boxAnimStateArray);

    for (int mixerIdx = 0; mixerIdx < mixerAnimStateArray.Size(); mixerIdx++)
    {
        bool animInListBox = false;
        
        for (int boxIdx = 0; boxIdx < boxAnimStateArray.Size(); boxIdx++)
        {
            if (mixerAnimStateArray[mixerIdx].name == boxAnimStateArray[boxIdx].name)
            {
                // anim already in the list-box, so just update the start/end frames
                const nLWAnimationState& curAnimState = mixerAnimStateArray[mixerIdx];
                this->animMultiListBox->SetAnim(boxIdx, curAnimState);
                animInListBox = true;
                break;
            }
        }
        
        if (!animInListBox)
        {
            this->animMultiListBox->AddAnim(mixerAnimStateArray[mixerIdx]);
        }
    }

    this->RefreshAnimControls();
}

//----------------------------------------------------------------------------
/**
*/
void
nLWObjectExportSettingsPanel::OnNewAnimStateBtn(wxCommandEvent& WXUNUSED(event))
{
    nLWAnimStatePanel animStatePanel(this, "Add Animation State");
    nLWAnimationState blankAnimState;
    animStatePanel.SetAnimState(blankAnimState);
    if (animStatePanel.ShowModal() == wxID_OK)
    {
        if (blankAnimState != animStatePanel.GetAnimState())
        {
            this->animMultiListBox->AddAnim(animStatePanel.GetAnimState());
            this->RefreshAnimControls();
        }
    }
}

//----------------------------------------------------------------------------
/**
*/
void
nLWObjectExportSettingsPanel::OnEditAnimStateBtn(wxCommandEvent& WXUNUSED(event))
{
    if (this->animMultiListBox->GetSelectedItemCount())
    {
        nLWAnimStatePanel animStatePanel(this, "Edit Animation State");
        nLWAnimationState animState;
        int selectionIdx = this->animMultiListBox->GetSelectedAnimIdx();
        this->animMultiListBox->GetAnim(selectionIdx, animState);
        animStatePanel.SetAnimState(animState);
        if (animStatePanel.ShowModal() == wxID_OK)
        {
            this->animMultiListBox->SetAnim(selectionIdx, animStatePanel.GetAnimState());
        }
    }
}

//----------------------------------------------------------------------------
/**
*/
void
nLWObjectExportSettingsPanel::OnDeleteAnimStateBtn(wxCommandEvent& WXUNUSED(event))
{
    if (this->animMultiListBox->GetSelectedItemCount())
    {
        this->animMultiListBox->RemoveAnim(this->animMultiListBox->GetSelectedAnimIdx());
        this->RefreshAnimControls();
    }
}

//----------------------------------------------------------------------------
/**
*/
void
nLWObjectExportSettingsPanel::OnMoveAnimStateUpBtn(wxCommandEvent& WXUNUSED(event))
{
    if (this->animMultiListBox->GetSelectedItemCount())
    {
        int rowToMove = this->animMultiListBox->GetSelectedAnimIdx();
        this->animMultiListBox->MoveAnim(rowToMove, -1);
        this->animMultiListBox->SelectAnim(rowToMove - 1);
    }
}

//----------------------------------------------------------------------------
/**
*/
void
nLWObjectExportSettingsPanel::OnMoveAnimStateDownBtn(wxCommandEvent& WXUNUSED(event))
{
    if (this->animMultiListBox->GetSelectedItemCount())
    {
        int rowToMove = this->animMultiListBox->GetSelectedAnimIdx();
        this->animMultiListBox->MoveAnim(rowToMove, +1);
        this->animMultiListBox->SelectAnim(rowToMove + 1);
    }
}

//----------------------------------------------------------------------------
/**
*/
void
nLWObjectExportSettingsPanel::OnAnimListChanged(wxListEvent& event)
{
    this->RefreshAnimControls();
    event.Skip();
}

//----------------------------------------------------------------------------
/**
*/
void
nLWObjectExportSettingsPanel::OnOKBtn(wxCommandEvent& WXUNUSED(event))
{
    this->Save();
    this->EndModal(wxID_OK);
}

//----------------------------------------------------------------------------
/**
*/
void
nLWObjectExportSettingsPanel::OnCancelBtn(wxCommandEvent& WXUNUSED(event))
{
    this->EndModal(wxID_CANCEL);
}

//----------------------------------------------------------------------------
/**
*/
void 
nLWObjectExportSettingsPanel::OnExportTypePopup(wxCommandEvent& WXUNUSED(event))
{
    this->RefreshAnimControls();
}

//----------------------------------------------------------------------------
// EOF
//----------------------------------------------------------------------------
