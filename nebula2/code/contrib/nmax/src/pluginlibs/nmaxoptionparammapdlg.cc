//------------------------------------------------------------------------------
//  nmaxoptionparammapdlg.cc
//
//  (c)2004 Kim, Hyoun Woo.
//------------------------------------------------------------------------------
#include "pluginlibs/nmaxoptionparammapdlg.h"
#include "../res/nmaxtoolbox.h"

#include "util/nstring.h"
#include "export2/nmaxoptions.h"

const char* script_server[nMaxOptionParamMapDlg::NumScriptServers] = {
    "Tcl",
    "Python",
    "Lua",
    "Ruby"
};

//------------------------------------------------------------------------------
/**
*/
nMaxOptionParamMapDlg::nMaxOptionParamMapDlg() :
    spinMaxJointPalette(0),
    spinWeightTrashold(0)
{

}

//------------------------------------------------------------------------------
/**
*/
nMaxOptionParamMapDlg::~nMaxOptionParamMapDlg()
{
    if (spinMaxJointPalette)
        ReleaseISpinner(spinMaxJointPalette);

    if (spinWeightTrashold)
        ReleaseISpinner(spinWeightTrashold);
}

//------------------------------------------------------------------------------
/**
    Initialize dialog controls.
*/
void nMaxOptionParamMapDlg::InitDialog(HWND hwnd)
{
    // mesh
    CheckDlgButton(hwnd, IDC_VERTEX_NORMAL, BST_CHECKED);
    CheckDlgButton(hwnd, IDC_VERTEX_COLOR, BST_CHECKED);
    CheckDlgButton(hwnd, IDC_VERTEX_UV, BST_CHECKED);
    CheckDlgButton(hwnd, IDC_VERTEX_TANGENT, BST_CHECKED);

    // max joint palette.
    int maxJointPalett = nMaxOptions::Instance()->GetMaxJointPaletteSize();
    
    nString strJointPalette;
    strJointPalette.AppendInt(maxJointPalett);

    spinMaxJointPalette = GetISpinner(GetDlgItem(hwnd, IDC_SPIN_MAXJOINTPALETTE));
    spinMaxJointPalette->SetScale(1.0f);
    spinMaxJointPalette->SetLimits(1, 24);
    HWND editJointPalette = GetDlgItem(hwnd, IDC_EDIT_MAXJOINTPALETTE);
    spinMaxJointPalette->LinkToEdit(editJointPalette, EDITTYPE_INT);
    spinMaxJointPalette->SetResetValue(24);
    spinMaxJointPalette->SetValue(maxJointPalett, FALSE);

    //FIXME: sending 'ES_RIGHT' message causes abnormal error on 3dsmax.
    //SendMessage(editJointPalette, ES_RIGHT, 0, 0L);

    // weight trash hold.
    float weightTrashold = nMaxOptions::Instance()->GetWeightTrashold();
    
    nString strWeightTrashold;
    strWeightTrashold.AppendFloat(weightTrashold);

    spinWeightTrashold = GetISpinner(GetDlgItem(hwnd, IDC_SPIN_WEIGHTRASHOLD));
    spinWeightTrashold->SetScale(0.00001f);
    spinWeightTrashold->SetLimits(0.00001f, 1.0f);
    HWND editWeigihtTrashold = GetDlgItem(hwnd, IDC_EDIT_WEIGHTRASHOLD);
    spinWeightTrashold->LinkToEdit(editWeigihtTrashold, EDITTYPE_FLOAT);
    spinWeightTrashold->SetResetValue(0.0001f);
    spinWeightTrashold->SetValue(weightTrashold, FALSE);

    //SendMessage(editWeigihtTrashold, ES_RIGHT, 0, 0L);

    // mesh type radio button
    CheckDlgButton(hwnd, IDC_N3D2, BST_CHECKED);
    CheckDlgButton(hwnd, IDC_NVX2, BST_UNCHECKED);

    // animation type radio button
    CheckDlgButton(hwnd, IDC_NANIM2, BST_CHECKED);
    CheckDlgButton(hwnd, IDC_NAX2, BST_UNCHECKED);

    CheckDlgButton(hwnd, IDC_HIDDEN_OBJ, 0);

    // script server
    HWND hwndSelScript = GetDlgItem(hwnd, IDC_SELECT_SCRIPT_SERVER);

    SendMessage(hwndSelScript, CB_RESETCONTENT, 0L, 0L);
    for (int i=0; i<NumScriptServers; i++)
    {
        SendMessage(hwndSelScript, CB_ADDSTRING, 0L, (LPARAM)script_server[i]);
    }

    int curSel = Tcl;
    SendMessage(hwndSelScript, CB_SETCURSEL, (WPARAM)curSel, 0);

}

//------------------------------------------------------------------------------
/**
    window procedure of the dialog.
*/
BOOL nMaxOptionParamMapDlg::DlgProc(TimeValue t, IParamMap *map, HWND hWnd, 
                                    UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
    case WM_INITDIALOG:
        InitDialog(hWnd);
        break;
    case WM_COMMAND:
        {
            // process command for the dialog controls.
            WORD highParam = HIWORD(wParam);
            WORD lowParam  = LOWORD(wParam);
            this->OnCommand(hWnd, highParam, lowParam);
        }
        break;

    default:
        return FALSE;
    }
    return FALSE;
}

//------------------------------------------------------------------------------
/**
   process command for the dialog controls.
*/
void nMaxOptionParamMapDlg::OnCommand(HWND hwnd, WORD highParam, WORD lowParam)
{
    switch(lowParam)
    {
    case IDC_VERTEX_NORMAL:
    case IDC_VERTEX_COLOR:
    case IDC_VERTEX_UV:
    case IDC_VERTEX_TANGENT:
        OnVertexComponent(hwnd);
        break;

    case CC_SPINNER_CHANGE:
    case WM_CUSTEDIT_ENTER:
        {
            int jointPalette = spinMaxJointPalette->GetIVal();
            nMaxOptions::Instance()->SetMaxJointPaletteSize(jointPalette);

            float weightTrashold = spinWeightTrashold->GetFVal();
            nMaxOptions::Instance()->SetWeightTrashold(weightTrashold);
        }
        break;

    case IDC_N3D2:
    case IDC_NVX2:
        OnMeshFileType(hwnd);
        break;

    case IDC_NANIM2:
    case IDC_NAX2:
        OnAnimFileType(hwnd);
        break;

    case IDC_HIDDEN_OBJ:
        OnHiddenObject(hwnd);
        break;

    case IDC_SELECT_SCRIPT_SERVER:
        if (highParam == CBN_SELCHANGE)
        {
            OnSelectedScriptServer(hwnd);
        }
        break;
    }
}

//------------------------------------------------------------------------------
/**
    Check the button status of the given resource id.

    @param hwnd windows handle.
    @param id button resource id.
*/
bool nMaxOptionParamMapDlg::IsChecked(HWND hwnd, int id)
{
    int status = IsDlgButtonChecked(hwnd, id);

    switch (status)
    {
    case BST_CHECKED:
        return true; 
    case BST_INDETERMINATE:
        return false;
    case BST_UNCHECKED:
        return false;
    }

    return false;
}

//------------------------------------------------------------------------------
/**
*/
void nMaxOptionParamMapDlg::OnVertexComponent(HWND hwnd)
{
    nMaxOptions* option = nMaxOptions::Instance();

    // normal
    if (IsChecked(hwnd, IDC_VERTEX_NORMAL))
        option->SetExportNormals(true);
    else
        option->SetExportNormals(false);

    // color
    if (IsChecked(hwnd, IDC_VERTEX_COLOR))
        option->SetExportColors(true);
    else
        option->SetExportColors(false);
    
    // uvs
    if (IsChecked(hwnd, IDC_VERTEX_UV))
        option->SetExportUvs(true);
    else
        option->SetExportUvs(false);
    
    // tangent
    if (IsChecked(hwnd, IDC_VERTEX_TANGENT))
    {
        CheckDlgButton(hwnd, IDC_VERTEX_NORMAL, BST_CHECKED);
        CheckDlgButton(hwnd, IDC_VERTEX_UV, BST_CHECKED);

        option->SetExportNormals(true);
        option->SetExportUvs(true);
        option->SetExportTangents(true);
    }
    else
    {
        option->SetExportTangents(false);
    }
}

//------------------------------------------------------------------------------
/**
*/
void nMaxOptionParamMapDlg::OnMeshFileType(HWND hwnd)
{
    if (IsChecked(hwnd, IDC_N3D2))
    {
        nMaxOptions::Instance()->SetMeshFileType(".n3d2");
    }
    else
    if (IsChecked(hwnd, IDC_NVX2))
    {
        nMaxOptions::Instance()->SetMeshFileType(".nvx2");
    }
}

//------------------------------------------------------------------------------
/**
*/
void nMaxOptionParamMapDlg::OnAnimFileType(HWND hwnd)
{
    if (IsChecked(hwnd, IDC_NANIM2))
    {
        nMaxOptions::Instance()->SetAnimFileType(".nanim2");
    }
    else
    if (IsChecked(hwnd, IDC_NAX2))
    {
        nMaxOptions::Instance()->SetAnimFileType(".nax2");
    }
}

//------------------------------------------------------------------------------
/**
*/
void nMaxOptionParamMapDlg::OnHiddenObject(HWND hwnd)
{
    if (IsChecked(hwnd, IDC_NANIM2))
    {
        nMaxOptions::Instance()->SetExportHiddenNodes(true);
    }
    else
    {
        nMaxOptions::Instance()->SetExportHiddenNodes(false);
    }
}

//------------------------------------------------------------------------------
/**
    Retrieves script server name and specifies it to option.
*/
void nMaxOptionParamMapDlg::OnSelectedScriptServer(HWND hwnd)
{
    int selectedItem = SendDlgItemMessage(hwnd, IDC_SELECT_SCRIPT_SERVER, 
                                          CB_GETCURSEL, 0, 0);

    if (selectedItem != CB_ERR)
    {
        // retrieves selected item name.
        char itemName[N_MAXPATH];
        SendDlgItemMessage(hwnd, 
                           IDC_SELECT_SCRIPT_SERVER,
                           CB_GETLBTEXT, 
                           (WPARAM) selectedItem, 
                           (LPARAM)(LPCSTR)&itemName[0]);

        nString scriptServerName = itemName;

        // specifies option.
        nMaxOptions::Instance()->SetSaveScriptServer(GetScriptServer(scriptServerName.Get()));
    }
}

//------------------------------------------------------------------------------
/**
    Retrieves proper script server name based on the selected item.
*/
const char* 
nMaxOptionParamMapDlg::GetScriptServer(const char* item)
{
    nString serverName = item;
    serverName.ToLower();

    if (serverName == "tcl")
        return "ntclserver";
    else
    if (serverName == "python")
        return "npythonserver";
    else
    if (serverName == "lua")
        return "nluaserver";
    else
    if (serverName == "ruby")
        return "nrubyserver";
    else
        return ""; // empty string.
}
