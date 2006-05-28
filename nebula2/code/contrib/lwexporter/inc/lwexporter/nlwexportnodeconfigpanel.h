#ifndef N_LW_SCENE_NODE_CONFIG_PANEL_H
#define N_LW_SCENE_NODE_CONFIG_PANEL_H
//----------------------------------------------------------------------------
#include "lwwrapper/nlwglobals.h"
#include "wx/wx.h"
#include "util/nstring.h"
#include "util/narray.h"

class nLWExportNodeType;

//----------------------------------------------------------------------------
/**
    @class nLWExportNodeConfigPanel
    @brief Dialog for Adding/Editing Export Node Types.

    Anything the user does on the dialog, like adding, modifying or deleting
    export node types will only be saved when the OK button is pressed. The
    changes won't be saved to disk automatically, they'll simply be propagated
    to the exporter settings singleton.
*/
class nLWExportNodeConfigPanel : public wxDialog
{
public:
    nLWExportNodeConfigPanel(wxWindow*);
    virtual ~nLWExportNodeConfigPanel();

private:
    nLWExportNodeType* GetExportNodeType(const nString&);
    void StoreCurrentData();
    void UpdateControls();
    void SaveChanges();

    void OnExportTypePopup(wxCommandEvent&);
    void OnNewExportTypeBtn(wxCommandEvent&);
    void OnRenExportTypeBtn(wxCommandEvent&);
    void OnDelExportTypeBtn(wxCommandEvent&);
    void OnOKBtn(wxCommandEvent&);
    void OnCancelBtn(wxCommandEvent&);

    wxChoice* exportNodeTypePopup;
    wxButton* renameExportNodeTypeBtn;
    wxButton* deleteExportNodeTypeBtn;
    wxChoice* sceneNodeTypePopup;
    wxCheckBox* texturesCheckBox;
    wxCheckBox* animationCheckBox;
    wxCheckBox* coordCheckBox;
    wxCheckBox* tangentCheckBox;
    wxCheckBox* binormalCheckBox;
    wxCheckBox* normalCheckBox;
    wxCheckBox* uv0CheckBox;
    wxCheckBox* uv1CheckBox;
    wxCheckBox* uv2CheckBox;
    wxCheckBox* uv3CheckBox;
    wxCheckBox* weightsCheckBox;
    wxCheckBox* colorCheckBox;

    nLWExportNodeType* curExportNodeType;
    nArray<nLWExportNodeType*> exportTypeArray;

    DECLARE_EVENT_TABLE()
};

//----------------------------------------------------------------------------
#endif // N_LW_SCENE_NODE_CONFIG_PANEL_H
