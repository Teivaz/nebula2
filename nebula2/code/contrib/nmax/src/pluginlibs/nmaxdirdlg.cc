//-----------------------------------------------------------------------------
//  nmaxdirdlg.cc
//
//  (C)2004 Kim, Hyoun Woo
//-----------------------------------------------------------------------------
#define _WIN32_DCOM
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <shlobj.h>
#include <objbase.h>

#include "export2/nmax.h"
#include "export2/nmaxexport2.h"

#include "../res/nmaxtoolbox.h"

#include "pluginlibs/nmaxdlg.h"
#include "pluginlibs/nmaxdirdlg.h"
#include "util/nstring.h"
#include "pluginlibs/ninifile.h"
#include "kernel/nfileserver2.h"
#include "kernel/nfile.h"

//-----------------------------------------------------------------------------
/**
*/
nMaxDirDlg::nMaxDirDlg(WORD resID, HINSTANCE hInst, HWND hwndParent) : 
    nMaxDlg(resID, hInst, hwndParent)
{
}

//-----------------------------------------------------------------------------
/**
*/
nMaxDirDlg::~nMaxDirDlg()
{
}

//-----------------------------------------------------------------------------
/**
    called when the dialog get the message WM_INITDIALOG.
*/
void nMaxDirDlg::OnInitDialog()
{
    nString homeDir;
    nString binaryPath;

    nString animsAssign;
    nString gfxlibAssign;
    nString guiAssign;
    nString lightsAssign;
    nString meshesAssign;
    nString shadersAssign;
    nString texturesAssign;

    nString animsPath;
    nString gfxlibPath;
    nString guiPath;
    nString lightsPath;
    nString meshesPath;
    nString shadersPath;
    nString texturesPath;

    nFileServer2* fileServer = nFileServer2::Instance();

    nString iniFilename;
    iniFilename += GetCOREInterface()->GetDir(APP_PLUGCFG_DIR);
    iniFilename += "\\";
    iniFilename += N_MAXEXPORT_INIFILE;

    // check the .ini file exist in 3dsmax plugin directory.
    if (!fileServer->FileExists(iniFilename.Get()))
    {
        // the .ini file does not exist, so make new one.
        //this->CreateDefaultIniFile(iniFilename.Get());
        nFile* file = fileServer->NewFileObject();
        file->Open(iniFilename.Get(), "w");
        file->Close();
        file->Release();
    }

    // read values from .ini file and specify those to dialog controls.
    nIniFile iniFile(iniFilename);
    iniFile.ReadString("HomeDir", homeDir, ".", "GeneralSettings");
    iniFile.ReadString("BinaryPath", binaryPath, ".", "GeneralSettings");

    iniFile.ReadString("AnimsAssign",    animsAssign,    N_MAXEXPORT_ANIMS_ASSIGN,    "GeneralSettings");
    iniFile.ReadString("GfxlibAssign",   gfxlibAssign,   N_MAXEXPORT_GFXLIB_ASSIGN,   "GeneralSettings");
    iniFile.ReadString("GuiAssign",      guiAssign,      N_MAXEXPORT_GUI_ASSIGN,      "GeneralSettings");
    iniFile.ReadString("LightsAssign",   lightsAssign,   N_MAXEXPORT_LIGHTS_ASSIGN,   "GeneralSettings");
    iniFile.ReadString("MeshesAssign",   meshesAssign,   N_MAXEXPORT_MESHES_ASSIGN,   "GeneralSettings");
    iniFile.ReadString("ShadersAssign",  shadersAssign,  N_MAXEXPORT_SHADERS_ASSIGN,  "GeneralSettings");
    iniFile.ReadString("TexturesAssign", texturesAssign, N_MAXEXPORT_TEXTURES_ASSIGN, "GeneralSettings");

    iniFile.ReadString("AnimsPath",    animsPath,    N_MAXEXPORT_ANIMS_PATH,    "GeneralSettings");
    iniFile.ReadString("GfxlibPath",   gfxlibPath,   N_MAXEXPORT_GFXLIB_PATH,   "GeneralSettings");
    iniFile.ReadString("GuiPath",      guiPath,      N_MAXEXPORT_GUI_PATH,      "GeneralSettings");
    iniFile.ReadString("LightsPath",   lightsPath,   N_MAXEXPORT_LIGHTS_PATH,   "GeneralSettings");
    iniFile.ReadString("MeshesPath",   meshesPath,   N_MAXEXPORT_MESHES_PATH,   "GeneralSettings");
    iniFile.ReadString("ShadersPath",  shadersPath,  N_MAXEXPORT_SHADERS_PATH,  "GeneralSettings");
    iniFile.ReadString("TexturesPath", texturesPath, N_MAXEXPORT_TEXTURES_PATH, "GeneralSettings");

    SetDlgItemText(hWnd, IDC_EXT_DIRNAME,     homeDir.Get());    // home dir
    SetDlgItemText(hWnd, IDC_EXT_BINARY_PATH, binaryPath.Get()); // binary path

    // assigns
    SetDlgItemText(hWnd, IDC_EXT_ASSIGN_ANIM,     animsAssign.Get());    // anim
    SetDlgItemText(hWnd, IDC_EXT_ASSIGN_GFXLIB,   gfxlibAssign.Get());   // gfx
    SetDlgItemText(hWnd, IDC_EXT_ASSIGN_GUI,      guiAssign.Get());      // gui
    SetDlgItemText(hWnd, IDC_EXT_ASSIGN_LIGHTS,   lightsAssign.Get());   // lights
    SetDlgItemText(hWnd, IDC_EXT_ASSIGN_MESHES,   meshesAssign.Get());   // meshes
    SetDlgItemText(hWnd, IDC_EXT_ASSIGN_SHADERS,  shadersAssign.Get());  // shaders
    SetDlgItemText(hWnd, IDC_EXT_ASSIGN_TEXTURES, texturesAssign.Get()); // textures

    // pathes
    SetDlgItemText(hWnd, IDC_EXT_PATH_ANIM,     animsPath.Get());
    SetDlgItemText(hWnd, IDC_EXT_PATH_GFXLIB,   gfxlibPath.Get());
    SetDlgItemText(hWnd, IDC_EXT_PATH_GUI,      guiPath.Get());
    SetDlgItemText(hWnd, IDC_EXT_PATH_LIGHTS,   lightsPath.Get());
    SetDlgItemText(hWnd, IDC_EXT_PATH_MESHES,   meshesPath.Get());
    SetDlgItemText(hWnd, IDC_EXT_PATH_SHADERS,  shadersPath.Get());
    SetDlgItemText(hWnd, IDC_EXT_PATH_TEXTURES, texturesPath.Get());
}

//-----------------------------------------------------------------------------
/**
    called when the dialog get the message WM_COMMAND.
*/
BOOL nMaxDirDlg::OnCommand(int wParamLow, int wParamHigh, long lParam)
{
    switch(wParamLow)
    {
    case IDOK:
        if (OnOK())
        {
            EndDialog(this->hWnd, IDOK);
        }
        break;

    case IDCANCEL:
        EndDialog(this->hWnd, IDCANCEL);
        break;

    case IDC_EXP_FILESEL:
        OnSelHomeDir();
        break;

    // 'Set Default' button in 'Assigns'
    case IDC_SET_DEFAULT_ASSIGNS:
        OnSetDefaultAssigns();
        break;

    // 'Set Default' button in 'Paths'
    case IDC_SET_DEFAULT_PATHS:
        OnSetDefaultPathes();
        break;

    default:
        break;
    }

    return TRUE;
}

//-----------------------------------------------------------------------------
/**
*/
bool nMaxDirDlg::OnOK()
{
    nString homeDir;
    nString binaryPath;

    nString animsAssign;
    nString gfxlibAssign;
    nString guiAssign;
    nString lightsAssign;
    nString meshesAssign;
    nString shadersAssign;
    nString texturesAssign;

    nString animsPath;
    nString gfxlibPath;
    nString guiPath;
    nString lightsPath;
    nString meshesPath;
    nString shadersPath;
    nString texturesPath;

    const int BUFSIZE = 512;
    char str[BUFSIZE];

    // retrieves value from dialog controls.
    GetDlgItemText(hWnd, IDC_EXT_DIRNAME,         str, BUFSIZE); homeDir = str;
    GetDlgItemText(hWnd, IDC_EXT_BINARY_PATH,     str, BUFSIZE); binaryPath = str;

    GetDlgItemText(hWnd, IDC_EXT_ASSIGN_ANIM,     str, BUFSIZE); animsAssign = str;
    GetDlgItemText(hWnd, IDC_EXT_ASSIGN_GFXLIB,   str, BUFSIZE); gfxlibAssign = str;
    GetDlgItemText(hWnd, IDC_EXT_ASSIGN_GUI,      str, BUFSIZE); guiAssign = str;
    GetDlgItemText(hWnd, IDC_EXT_ASSIGN_LIGHTS,   str, BUFSIZE); lightsAssign = str;
    GetDlgItemText(hWnd, IDC_EXT_ASSIGN_MESHES,   str, BUFSIZE); meshesAssign = str;
    GetDlgItemText(hWnd, IDC_EXT_ASSIGN_SHADERS,  str, BUFSIZE); shadersAssign = str;
    GetDlgItemText(hWnd, IDC_EXT_ASSIGN_TEXTURES, str, BUFSIZE); texturesAssign = str;

    GetDlgItemText(hWnd, IDC_EXT_PATH_ANIM,     str, BUFSIZE); animsPath = str;
    GetDlgItemText(hWnd, IDC_EXT_PATH_GFXLIB,   str, BUFSIZE); gfxlibPath = str;
    GetDlgItemText(hWnd, IDC_EXT_PATH_GUI,      str, BUFSIZE); guiPath = str;
    GetDlgItemText(hWnd, IDC_EXT_PATH_LIGHTS,   str, BUFSIZE); lightsPath = str;
    GetDlgItemText(hWnd, IDC_EXT_PATH_MESHES,   str, BUFSIZE); meshesPath = str;
    GetDlgItemText(hWnd, IDC_EXT_PATH_SHADERS,  str, BUFSIZE); shadersPath = str;
    GetDlgItemText(hWnd, IDC_EXT_PATH_TEXTURES, str, BUFSIZE); texturesPath = str;

    // check the specified assigns and pathes are valid.
    if (!nFileServer2::Instance()->DirectoryExists(homeDir.Get()))
    {
        // specified home directory does not exist.
        MessageBox(this->hWnd, "The specified 'Home' assign directory does not exist.", "Error", MB_OK);
        return false;
    }
    else
    {
        // write the values to .ini file.
        nString iniFilename;
        iniFilename += GetCOREInterface()->GetDir(APP_PLUGCFG_DIR);
        iniFilename += "\\";
        iniFilename += N_MAXEXPORT_INIFILE;
        
        nIniFile iniFile (iniFilename);
        iniFile.WriteString("HomeDir", homeDir,  "GeneralSettings");
        iniFile.WriteString("BinaryPath", binaryPath, "GeneralSettings");

        iniFile.WriteString("AnimsAssign",    animsAssign,    "GeneralSettings");
        iniFile.WriteString("GfxlibAssign",   gfxlibAssign,   "GeneralSettings");
        iniFile.WriteString("GuiAssign",      guiAssign,      "GeneralSettings");
        iniFile.WriteString("LightsAssign",   lightsAssign,   "GeneralSettings");
        iniFile.WriteString("MeshesAssign",   meshesAssign,   "GeneralSettings");
        iniFile.WriteString("ShadersAssign",  shadersAssign,  "GeneralSettings");
        iniFile.WriteString("TexturesAssign", texturesAssign, "GeneralSettings");

        iniFile.WriteString("AnimsPath",    animsPath,    "GeneralSettings");
        iniFile.WriteString("GfxlibPath",   gfxlibPath,   "GeneralSettings");
        iniFile.WriteString("GuiPath",      guiPath,      "GeneralSettings");
        iniFile.WriteString("LightsPath",   lightsPath,   "GeneralSettings");
        iniFile.WriteString("MeshesPath",   meshesPath,   "GeneralSettings");
        iniFile.WriteString("ShadersPath",  shadersPath,  "GeneralSettings");
        iniFile.WriteString("TexturesPath", texturesPath, "GeneralSettings");
    }

    return true;
}

//-----------------------------------------------------------------------------
/**
*/
void nMaxDirDlg::OnSelHomeDir()
{
    // Retrieve the task memory allocator.
    LPMALLOC pIMalloc;
    if (!SUCCEEDED(::SHGetMalloc(&pIMalloc)))
    {
        return;
    }

    char szBuf[MAX_PATH];

    // Initialize a BROWSEINFO structure,
    BROWSEINFO brInfo;
    ::ZeroMemory(&brInfo, sizeof(brInfo));

    brInfo.hwndOwner      = this->hWnd;
    brInfo.pidlRoot       = NULL;
    brInfo.pszDisplayName = szBuf;
    brInfo.lpszTitle      = "Select Nebula2 installed directory";

    // only want folders (no printers, etc.)
    brInfo.ulFlags = BIF_RETURNONLYFSDIRS;

    // Display the browser.
    ITEMIDLIST* browseList = NULL;
    browseList = ::SHBrowseForFolder(&brInfo);

    // if the user selected a folder . . .
    if (browseList)
    {
        // Convert the item ID to a pathname, 
        //char szBuf[MAX_PATH];
        if(::SHGetPathFromIDList(browseList, szBuf))
        {
            //TRACE1("You chose==>%s\n",szBuf);
            nString homeDir;
            homeDir.Format("%s", szBuf);
            SetDlgItemText(hWnd, IDC_EXT_DIRNAME, homeDir.Get());
        }

        // Free the PIDL
        pIMalloc->Free(browseList);
    }
    else
    {
        *szBuf=_T('\0');
        //AfxMessageBox("Canceled browse.");
    }

    // Decrement ref count on the allocator
    pIMalloc->Release();
}

//-----------------------------------------------------------------------------
/**
*/
void nMaxDirDlg::OnSetDefaultAssigns()
{
    SetDlgItemText(hWnd, IDC_EXT_ASSIGN_ANIM,     N_MAXEXPORT_ANIMS_ASSIGN);
    SetDlgItemText(hWnd, IDC_EXT_ASSIGN_GFXLIB,   N_MAXEXPORT_GFXLIB_ASSIGN);
    SetDlgItemText(hWnd, IDC_EXT_ASSIGN_GUI,      N_MAXEXPORT_GUI_ASSIGN);
    SetDlgItemText(hWnd, IDC_EXT_ASSIGN_LIGHTS,   N_MAXEXPORT_LIGHTS_ASSIGN);
    SetDlgItemText(hWnd, IDC_EXT_ASSIGN_MESHES,   N_MAXEXPORT_MESHES_ASSIGN);
    SetDlgItemText(hWnd, IDC_EXT_ASSIGN_SHADERS,  N_MAXEXPORT_SHADERS_ASSIGN);
    SetDlgItemText(hWnd, IDC_EXT_ASSIGN_TEXTURES, N_MAXEXPORT_TEXTURES_ASSIGN);
}

//-----------------------------------------------------------------------------
/**
*/
void nMaxDirDlg::OnSetDefaultPathes()
{
    SetDlgItemText(hWnd, IDC_EXT_PATH_ANIM,     N_MAXEXPORT_ANIMS_PATH);
    SetDlgItemText(hWnd, IDC_EXT_PATH_GFXLIB,   N_MAXEXPORT_GFXLIB_PATH);
    SetDlgItemText(hWnd, IDC_EXT_PATH_GUI,      N_MAXEXPORT_GUI_PATH);
    SetDlgItemText(hWnd, IDC_EXT_PATH_LIGHTS,   N_MAXEXPORT_LIGHTS_PATH);
    SetDlgItemText(hWnd, IDC_EXT_PATH_MESHES,   N_MAXEXPORT_MESHES_PATH);
    SetDlgItemText(hWnd, IDC_EXT_PATH_SHADERS,  N_MAXEXPORT_SHADERS_PATH);
    SetDlgItemText(hWnd, IDC_EXT_PATH_TEXTURES, N_MAXEXPORT_TEXTURES_PATH);
}

//-----------------------------------------------------------------------------
/**
    Defined for scripting interface. See nmaxdlgscript.cc
*/
void ShowDirSettingDlg()
{
    // get instance hanlde of dll
    HINSTANCE hInstance = maxExportInterfaceClassDesc2.HInstance();

    // get window handle of 3dsmax
    Interface* intf = GetCOREInterface();
    HWND parentHWnd = intf->GetMAXHWnd();

    // show directory setting dialog up.
    nMaxDirDlg dirDlg(IDD_DIR_SETTING, hInstance, parentHWnd);
    int ret = dirDlg.DoModal();
}
