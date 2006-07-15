#ifndef N_LW_ANIM_STATE_PANEL_H
#define N_LW_ANIM_STATE_PANEL_H
//----------------------------------------------------------------------------
#include "lwwrapper/nlwglobals.h"
#include "lwexporter/nlwanimationstate.h"
#include "wx/wx.h"

//----------------------------------------------------------------------------
/**
    @class nLWAnimStatePanel
    @brief Dialog for Adding/Editing animation state info.
*/
class nLWAnimStatePanel : public wxDialog
{
public:
    nLWAnimStatePanel(wxWindow* parent, const char* title);
    virtual ~nLWAnimStatePanel();

    void SetAnimState(const nLWAnimationState&);
    const nLWAnimationState& GetAnimState() const;

private:
    void CopyDataToControls();
    void CopyDataFromControls();

    void OnStartFrameBtn(wxCommandEvent&);
    void OnEndFrameBtn(wxCommandEvent&);
    void OnOKBtn(wxCommandEvent&);
    void OnCancelBtn(wxCommandEvent&);

    nLWAnimationState animState;

    wxTextCtrl* stateNameBox;
    wxTextCtrl* startFrameBox; // FIXME: should only take ints
    wxTextCtrl* endFrameBox; // FIXME: should only take ints
    wxTextCtrl* fadeInBox; // FIXME: should only take floats
    wxCheckBox* loopAnimCheckBox;

    DECLARE_EVENT_TABLE()
};

//----------------------------------------------------------------------------
#endif // N_LW_ANIM_STATE_PANEL_H
