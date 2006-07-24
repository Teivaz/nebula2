//----------------------------------------------------------------------------
// (c) 2006    Vadim Macagon
//----------------------------------------------------------------------------
#include "lwexporter/nlwanimstatepanel.h"
#include "wx/gbsizer.h"

//----------------------------------------------------------------------------
enum
{
    ID_START_FRAME_BTN = wxID_HIGHEST + 1,
    ID_END_FRAME_BTN,
};

BEGIN_EVENT_TABLE(nLWAnimStatePanel, wxDialog)
    EVT_BUTTON(wxID_OK, nLWAnimStatePanel::OnOKBtn)
    EVT_BUTTON(wxID_CANCEL, nLWAnimStatePanel::OnCancelBtn)
    EVT_BUTTON(ID_START_FRAME_BTN, nLWAnimStatePanel::OnStartFrameBtn)
    EVT_BUTTON(ID_END_FRAME_BTN, nLWAnimStatePanel::OnEndFrameBtn)
END_EVENT_TABLE()

//----------------------------------------------------------------------------
/**
*/
nLWAnimStatePanel::nLWAnimStatePanel(wxWindow* parent, const char* title) :
    wxDialog(parent, wxID_ANY, title, wxDefaultPosition),
    stateNameBox(0),
    startFrameBox(0),
    endFrameBox(0),
    fadeInBox(0),
    loopAnimCheckBox(0)
{
    wxStaticText* stateNameLabel = new wxStaticText(this, wxID_ANY, "State Name");
    this->stateNameBox = new wxTextCtrl(this, wxID_ANY);
    wxStaticText* startFrameLabel = new wxStaticText(this, wxID_ANY, "Start Frame");
    this->startFrameBox = new wxTextCtrl(this, wxID_ANY);
    wxButton* startFrameBtn = new wxButton(this, ID_START_FRAME_BTN, "Set Current");
    wxStaticText* endFrameLabel = new wxStaticText(this, wxID_ANY, "End Frame");
    this->endFrameBox = new wxTextCtrl(this, wxID_ANY);
    wxButton* endFrameBtn = new wxButton(this, ID_END_FRAME_BTN, "Set Current");
    wxStaticText* fadeInLabel = new wxStaticText(this, wxID_ANY, "Fade-in Time");
    this->fadeInBox = new wxTextCtrl(this, wxID_ANY);
    this->loopAnimCheckBox = new wxCheckBox(this, wxID_ANY, "Loop");

    // layout controls
    wxGridBagSizer* topSizer = new wxGridBagSizer(5, 5);
    // row 0
    topSizer->Add(stateNameLabel, wxGBPosition(0, 0));
    topSizer->Add(this->stateNameBox, wxGBPosition(0, 1), wxGBSpan(1, 2));
    // row 1
    topSizer->Add(startFrameLabel, wxGBPosition(1, 0));
    topSizer->Add(this->startFrameBox, wxGBPosition(1, 1));
    topSizer->Add(startFrameBtn, wxGBPosition(1, 2));
    // row 2
    topSizer->Add(endFrameLabel, wxGBPosition(2, 0));
    topSizer->Add(this->endFrameBox, wxGBPosition(2, 1));
    topSizer->Add(endFrameBtn, wxGBPosition(2, 2));
    // row 3
    topSizer->Add(fadeInLabel, wxGBPosition(3, 0));
    topSizer->Add(this->fadeInBox, wxGBPosition(3, 1));
    // row 4
    topSizer->Add(this->loopAnimCheckBox, wxGBPosition(4, 1));
    // row 5
    topSizer->Add(CreateButtonSizer(wxOK|wxCANCEL), wxGBPosition(5, 0), wxGBSpan(1, 3), wxALIGN_CENTER_HORIZONTAL|wxALL, 10);

    topSizer->Fit(this);
    this->SetSizer(topSizer);
}

//----------------------------------------------------------------------------
/**
*/
nLWAnimStatePanel::~nLWAnimStatePanel()
{
    // empty
}

//----------------------------------------------------------------------------
/**
*/
void
nLWAnimStatePanel::SetAnimState(const nLWAnimationState& animState)
{
    this->animState = animState;
    this->CopyDataToControls();
}

//----------------------------------------------------------------------------
/**
*/
const nLWAnimationState&
nLWAnimStatePanel::GetAnimState() const
{
    return this->animState;
}

//----------------------------------------------------------------------------
/**
*/
void
nLWAnimStatePanel::CopyDataToControls()
{
    if (this->stateNameBox)
    {
        this->stateNameBox->SetValue(this->animState.name.Get());
    }
    nString tempStr;
    if (this->startFrameBox)
    {
        tempStr.SetInt(this->animState.startFrame);
        this->startFrameBox->SetValue(tempStr.Get());
    }
    if (this->endFrameBox)
    {
        tempStr.SetInt(this->animState.endFrame);
        this->endFrameBox->SetValue(tempStr.Get());
    }
    if (this->fadeInBox)
    {
        tempStr.SetFloat(this->animState.fadeInTime);
        this->fadeInBox->SetValue(tempStr.Get());
    }
    if (this->loopAnimCheckBox)
    {
        this->loopAnimCheckBox->SetValue(this->animState.repeat);
    }
}

//----------------------------------------------------------------------------
/**
*/
void
nLWAnimStatePanel::CopyDataFromControls()
{
    if (this->stateNameBox)
    {
        this->animState.name = this->stateNameBox->GetValue().c_str();
    }
    nString tempStr;
    if (this->startFrameBox)
    {
        tempStr = this->startFrameBox->GetValue().c_str();
        this->animState.startFrame = tempStr.AsInt();
    }
    if (this->endFrameBox)
    {
        tempStr = this->endFrameBox->GetValue().c_str();
        this->animState.endFrame = tempStr.AsInt();
    }
    if (this->fadeInBox)
    {
        tempStr = this->fadeInBox->GetValue().c_str();
        this->animState.fadeInTime = tempStr.AsFloat();
    }
    if (this->loopAnimCheckBox)
    {
        this->animState.repeat = this->loopAnimCheckBox->GetValue();
    }
}

//----------------------------------------------------------------------------
namespace
{
    //----------------------------------------------------------------------------
    /**
        @brief Get the current frame being displayed in Lightwave.
    */
    int
    GetCurrentFrame()
    {
        nLWGlobals::InterfaceInfo interfaceInfo;
        n_assert(interfaceInfo.IsValid());
        if (!interfaceInfo.IsValid())
            return 0;

        nLWGlobals::SceneInfo sceneInfo;
        n_assert(sceneInfo.IsValid());
        if (!sceneInfo.IsValid())
            return 0;

        double curTime = interfaceInfo.Get()->curTime;
        double fps = sceneInfo.Get()->framesPerSecond;
        return (int)(curTime * fps);
    }
}

//----------------------------------------------------------------------------
/**
*/
void
nLWAnimStatePanel::OnStartFrameBtn(wxCommandEvent& WXUNUSED(event))
{
    nString tempStr;
    tempStr.SetInt(GetCurrentFrame());
    this->startFrameBox->SetValue(tempStr.Get());
}

//----------------------------------------------------------------------------
/**
*/
void
nLWAnimStatePanel::OnEndFrameBtn(wxCommandEvent& WXUNUSED(event))
{
    nString tempStr;
    tempStr.SetInt(GetCurrentFrame());
    this->endFrameBox->SetValue(tempStr.Get());
}

//----------------------------------------------------------------------------
/**
*/
void
nLWAnimStatePanel::OnOKBtn(wxCommandEvent& WXUNUSED(event))
{
    // make sure that the user at least entered a name for the state
    if (this->stateNameBox->GetValue().IsEmpty())
    {
        wxMessageBox("The State Name cannot be blank.", "Animation State");
        return;
    }
    this->CopyDataFromControls();
    this->EndModal(wxID_OK);
}

//----------------------------------------------------------------------------
/**
*/
void
nLWAnimStatePanel::OnCancelBtn(wxCommandEvent& WXUNUSED(event))
{
    this->EndModal(wxID_CANCEL);
}

//----------------------------------------------------------------------------
// EOF
//----------------------------------------------------------------------------
