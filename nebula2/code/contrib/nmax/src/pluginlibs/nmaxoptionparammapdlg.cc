//------------------------------------------------------------------------------
//  nmaxoptionparammapdlg.cc
//
//  (c)2004 Kim, Hyoun Woo.
//------------------------------------------------------------------------------
#include "pluginlibs/nmaxoptionparammapdlg.h"
#include "../res/nmaxtoolbox.h"

#include "util/nstring.h"
#include "export2/nmaxoptions.h"

// items of the script server combobox.
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
    spinGeomScale(0),
    spinMaxJointPalette(0),
    spinWeightTrashold(0),
    spinSampleRate(0)
{

}

//------------------------------------------------------------------------------
/**
*/
nMaxOptionParamMapDlg::~nMaxOptionParamMapDlg()
{
    if (spinGeomScale)
        ReleaseISpinner(spinGeomScale);

    if (spinMaxJointPalette)
        ReleaseISpinner(spinMaxJointPalette);

    if (spinWeightTrashold)
        ReleaseISpinner(spinWeightTrashold);

    if (spinSampleRate)
        ReleaseISpinner(spinSampleRate);
}

//------------------------------------------------------------------------------
/**
    Initialize dialog controls.

    - 22-Mar-05 kims fixed specifying value of spinner.
                     The value, TRUE should be passed for the flag of SetValue()
                     to it correctly update its value.
*/
void nMaxOptionParamMapDlg::InitDialog(HWND hwnd)
{
    if (NULL == hwnd)
    {
        n_message("Failed to initialize option dialog: NULL window handle is passed.");
        return;
    }

    // mesh
    CheckDlgButton(hwnd, IDC_VERTEX_NORMAL, BST_CHECKED);
    CheckDlgButton(hwnd, IDC_VERTEX_COLOR, BST_CHECKED);
    CheckDlgButton(hwnd, IDC_VERTEX_UV, BST_CHECKED);
    CheckDlgButton(hwnd, IDC_VERTEX_TANGENT, BST_CHECKED);

    // geometry scale.
    float geomScale = nMaxOptions::Instance()->GetGeomScaleValue();

    spinGeomScale = GetISpinner(GetDlgItem(hwnd, IDC_SPIN_GEOMSCALE));
    spinGeomScale->SetScale(0.001f);
    spinGeomScale->SetLimits(0.001f, 1.0f);
    HWND editGeomScale = GetDlgItem(hwnd, IDC_EDIT_GEOMSCALE);
    spinGeomScale->LinkToEdit(editGeomScale, EDITTYPE_FLOAT);
    spinGeomScale->SetResetValue(0.01f);
    spinGeomScale->SetValue(geomScale, TRUE);

    // max joint palette.
    int maxJointPalett = nMaxOptions::Instance()->GetMaxJointPaletteSize();
    
    spinMaxJointPalette = GetISpinner(GetDlgItem(hwnd, IDC_SPIN_MAXJOINTPALETTE));
    spinMaxJointPalette->SetScale(1);
    // The minimum joint palette size is 4. under that value causes error when
    // the mesh is partitioned.
    spinMaxJointPalette->SetLimits(4, 24);
    HWND editJointPalette = GetDlgItem(hwnd, IDC_EDIT_MAXJOINTPALETTE);
    spinMaxJointPalette->LinkToEdit(editJointPalette, EDITTYPE_INT);
    spinMaxJointPalette->SetResetValue(24);
    spinMaxJointPalette->SetValue(maxJointPalett, TRUE);

    //FIXME: sending 'ES_RIGHT' message causes abnormal error on 3dsmax.
    //SendMessage(editJointPalette, ES_RIGHT, 0, 0L);

    // weight trash hold.
    float weightTrashold = nMaxOptions::Instance()->GetWeightTrashold();
    
    spinWeightTrashold = GetISpinner(GetDlgItem(hwnd, IDC_SPIN_WEIGHTRASHOLD));
    spinWeightTrashold->SetScale(0.00001f);
    spinWeightTrashold->SetLimits(0.00001f, 1.0f);
    HWND editWeigihtTrashold = GetDlgItem(hwnd, IDC_EDIT_WEIGHTRASHOLD);
    spinWeightTrashold->LinkToEdit(editWeigihtTrashold, EDITTYPE_FLOAT);
    spinWeightTrashold->SetResetValue(0.0001f);
    spinWeightTrashold->SetValue(weightTrashold, TRUE);

    //SendMessage(editWeigihtTrashold, ES_RIGHT, 0, 0L);

    // sample rate
    int sampleRate = nMaxOptions::Instance()->GetSampleRate();

    spinSampleRate = GetISpinner(GetDlgItem(hwnd, IDC_SPIN_SAMPLERATE));
    spinSampleRate->SetScale(1);
    spinSampleRate->SetLimits(1, 64);
    HWND editSampleRate = GetDlgItem(hwnd, IDC_EDIT_SAMPLERATE);
    spinSampleRate->LinkToEdit(editSampleRate, EDITTYPE_INT);
    spinSampleRate->SetResetValue(2);
    spinSampleRate->SetValue(sampleRate, TRUE);

    bool hasJointName = nMaxOptions::Instance()->HasJointName();
    int exportJointName = hasJointName ? 1 : 0;
    CheckDlgButton(hwnd, IDC_ADDJOINTNAME, exportJointName);

    // mesh type radio button
    const nString& meshFileType = nMaxOptions::Instance()->GetMeshFileType();
    if (".n3d2" == meshFileType)
    {
        CheckDlgButton(hwnd, IDC_N3D2, BST_CHECKED);
        CheckDlgButton(hwnd, IDC_NVX2, BST_UNCHECKED);
    }
    else // .nvx2
    {
        CheckDlgButton(hwnd, IDC_N3D2, BST_UNCHECKED);
        CheckDlgButton(hwnd, IDC_NVX2, BST_CHECKED);
    }

    // animation type radio button
    const nString& animFileType = nMaxOptions::Instance()->GetAnimFileType();
    if (".nanim2" == animFileType)
    {
        CheckDlgButton(hwnd, IDC_NANIM2, BST_CHECKED);
        CheckDlgButton(hwnd, IDC_NAX2, BST_UNCHECKED);
    }
    else // .nax2
    {
        CheckDlgButton(hwnd, IDC_NANIM2, BST_UNCHECKED);
        CheckDlgButton(hwnd, IDC_NAX2, BST_CHECKED);
    }
    
    bool tmpExportHiddenNode = nMaxOptions::Instance()->ExportHiddenNodes();
    int exportHiddenNode = tmpExportHiddenNode ? 1 : 0;
    CheckDlgButton(hwnd, IDC_HIDDEN_OBJ, exportHiddenNode);

    // script server combobox setting.
    HWND hwndSelScript = GetDlgItem(hwnd, IDC_SELECT_SCRIPT_SERVER);

    // reset content.
    SendMessage(hwndSelScript, CB_RESETCONTENT, 0L, 0L);
    // fill items.
    for (int i=0; i<NumScriptServers; i++)
    {
        SendMessage(hwndSelScript, CB_ADDSTRING, 0L, (LPARAM)script_server[i]);
    }

    // set the default item.
    const nString& name = nMaxOptions::Instance()->GetSaveScriptServer();
    int curSel = GetItemIndexFromScript(name.Get());
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

    case CC_SPINNER_CHANGE:
    case WM_CUSTEDIT_ENTER:
        {
            int id;
            if (msg == CC_SPINNER_CHANGE)
                id = LOWORD(wParam);
            else// WM_CUSTEDIT_ENTER
                id = wParam;

            if (spinGeomScale && id == IDC_SPIN_GEOMSCALE)
            {
                float geomScale = spinGeomScale->GetFVal();
                nMaxOptions::Instance()->SetGeomScaleValue(geomScale);
            }

            if (spinMaxJointPalette && id == IDC_SPIN_MAXJOINTPALETTE)
            {
                int jointPalette = spinMaxJointPalette->GetIVal();
                nMaxOptions::Instance()->SetMaxJointPaletteSize(jointPalette);
            }

            if (spinWeightTrashold && id == IDC_SPIN_WEIGHTRASHOLD)
            {
                float weightTrashold = spinWeightTrashold->GetFVal();
                nMaxOptions::Instance()->SetWeightTrashold(weightTrashold);
            }

            if (spinSampleRate && id == IDC_SPIN_SAMPLERATE)
            {
                int sampleRate = spinSampleRate->GetIVal();
                nMaxOptions::Instance()->SetSampleRate(sampleRate);
            }
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

    case IDC_ADDJOINTNAME:
        {
            if (IsChecked(hwnd, IDC_ADDJOINTNAME))
                nMaxOptions::Instance()->SetAddJointName(true);
            else
                nMaxOptions::Instance()->SetAddJointName(false);
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
    if (IsChecked(hwnd, IDC_HIDDEN_OBJ))
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

//------------------------------------------------------------------------------
/**
    Retrieves script server combobox item index with the given script server name.

    @param name script server name.

    @return zero based combobox item index. -1 if there is no match which is an error.
*/
int
nMaxOptionParamMapDlg::GetItemIndexFromScript(const char* name)
{
    nString serverName = name;
    serverName.ToLower();

    if (serverName == "ntclserver")
        return 0; //'Tcl"
    else
    if (serverName == "npythonserver")
        return 1; // "Python"
    else
    if (serverName == "nluaserver")
        return 2; // "Lua"
    else
    if (serverName == "nrubyserver")
        return 3;
    else
        return -1; // empty string.
}
