#ifndef N_LW_OBJECT_EXPORT_SETTINGS_UI_H
#define N_LW_OBJECT_EXPORT_SETTINGS_UI_H
//----------------------------------------------------------------------------
#include "lwwrapper/nlwglobals.h"
#include "util/narray.h"
#include "lwexporter/nlwanimationstate.h"
#include "wx/wx.h"
#include "wx/listctrl.h"

class nLWObjectExportSettings;
class wxAnimListCtrl;

//----------------------------------------------------------------------------
/**
    @class nLWObjectExportSettingsPanel
    @brief Dialog that displays per object export-related settings.
*/
class nLWObjectExportSettingsPanel : public wxDialog
{
public:
    nLWObjectExportSettingsPanel(wxWindow*, nLWObjectExportSettings*);
    virtual ~nLWObjectExportSettingsPanel();

    void SetItemName(const nString&);

private:
    void OnScanMMBtn(wxCommandEvent&);
    void OnNewAnimStateBtn(wxCommandEvent&);
    void OnEditAnimStateBtn(wxCommandEvent&);
    void OnDeleteAnimStateBtn(wxCommandEvent&);
    void OnMoveAnimStateUpBtn(wxCommandEvent&);
    void OnMoveAnimStateDownBtn(wxCommandEvent&);
    void OnAnimListChanged(wxListEvent&);
    void OnOKBtn(wxCommandEvent&);
    void OnCancelBtn(wxCommandEvent&);
    void OnExportTypePopup(wxCommandEvent&);

    /// save settings
    void Save();
    /// load settings
    void Load();

    void GetAnimStatesFromListBox(nArray<nLWAnimationState>&);
    void FillListBoxWithAnimStates(const nArray<nLWAnimationState>&);
    bool NeedAnimControls() const;
    void RefreshAnimControls();

    wxStaticText* itemNameTextBox;
    wxChoice* nodeTypePopup;
    wxCheckBox* nodeHierarchyCheckBox;
    wxTextCtrl* animVarTextBox;
    wxButton* scanMotionMixerBtn;
    wxAnimListCtrl* animMultiListBox;
    wxButton* newAnimBtn;
    wxButton* editAnimBtn;
    wxButton* deleteAnimBtn;
    wxButton* moveAnimUpBtn;
    wxButton* moveAnimDownBtn;
    
    nLWObjectExportSettings* settings;
    nString itemName;

    DECLARE_EVENT_TABLE()
};

//----------------------------------------------------------------------------
#endif // N_LW_OBJECT_EXPORT_SETTINGS_UI_H
