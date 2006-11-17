//-----------------------------------------------------------------------------
//  nMaxOptions.cc
//
//  (c)2004 Kim, Hyoun Woo
//-----------------------------------------------------------------------------
#include "base/nmaxlistener.h"
#include "export2/nmax.h"
#include "export2/nmaxoptions.h"
#include "pluginlibs/nmaxdlg.h"
#include "pluginlibs/nmaxlogdlg.h"

#include "kernel/nkernelserver.h"
#include "kernel/nfileserver2.h"
#include "kernel/nfile.h"
#include "misc/niniprefserver.h"

nMaxOptions* nMaxOptions::Singleton = 0;

//-----------------------------------------------------------------------------
/**
*/
nMaxOptions::nMaxOptions() : 
    exportNormal(true),
    exportVertColor(true),
    exportUvs(true),
    exportTangent(true),
    exportHiddenNodes(false),
    exportAnimations(true),
    groupMeshes(true),
    meshFileExtension(".n3d2"),
    animFileExtension(".nanim2"),
    geomScale(0.01f),
    maxJointPaletteSize(24),
    weightThreshold(0.0001f),
    sampleRate (2),
    addJointName(false),
    runViewer(true),
    saveScriptServer("ntclserver"),
    useIndivisualMesh(false),
    verboseLevel(2/*Low*/),
    overwriteExistTexture(true),
    previewMode(false),
    useDefaultViewer(true)
{
    n_assert(Singleton == 0);
    Singleton = this;
}

//-----------------------------------------------------------------------------
/**
*/
nMaxOptions::~nMaxOptions()
{
    Singleton = 0;
}

//-----------------------------------------------------------------------------
/**
    Read .ini file in '$3dsmax/plugcfg' directory and specifies assigns and 
    paths to file server.

    @return true if the function call is success.

    -19-Sep-05    kims    Added reading 'proj' assign from .ini file.
*/
bool nMaxOptions::Initialize()
{
    nString iniFilename;
    iniFilename += GetCOREInterface()->GetDir(APP_PLUGCFG_DIR);
    iniFilename += "\\";
    iniFilename += N_MAXEXPORT_INIFILE;

    nFileServer2* fileServer = nFileServer2::Instance();

    // check the .ini file exist in 3dsmax plugin directory.
    if (!fileServer->FileExists(iniFilename.Get()))
    {
        // .ini file does not exist in '/plugcfg' directory.
        n_listener("%s file does not exist in '$3dsmax/plugcfg' directory.", N_MAXEXPORT_INIFILE);
        return false;
    }

    nIniPrefServer* iniFile = (nIniPrefServer*)nKernelServer::Instance()->New("niniprefserver", "/iniprefsrv");
    iniFile->SetFileName(iniFilename);
    iniFile->SetSection("GeneralSettings");
    iniFile->SetDefault(".");
    
    homeDir = iniFile->ReadString("HomeDir");
    if (homeDir.IsEmpty()) 
        return false;
    
    projDir = iniFile->ReadString("ProjDir");
    if (projDir.IsEmpty()) 
        return false;
    
    binaryPath = iniFile->ReadString("BinaryPath") ;
    if (binaryPath.IsEmpty())    
        return false;

    iniFile->SetDefault(N_MAXEXPORT_ANIMS_ASSIGN);
    animsAssign = iniFile->ReadString("AnimsAssign");
    if (animsAssign.IsEmpty())
        return false;
    
    iniFile->SetDefault(N_MAXEXPORT_GFXLIB_ASSIGN);
    gfxlibAssign = iniFile->ReadString("GfxlibAssign");
    if (gfxlibAssign.IsEmpty())
        return false;
    
    iniFile->SetDefault(N_MAXEXPORT_GUI_ASSIGN);
    guiAssign = iniFile->ReadString("GuiAssign") ;
    if (guiAssign.IsEmpty())
        return false;
    
    iniFile->SetDefault(N_MAXEXPORT_LIGHTS_ASSIGN);
    lightsAssign = iniFile->ReadString("LightsAssign");
    if (lightsAssign.IsEmpty())
        return false;

    iniFile->SetDefault(N_MAXEXPORT_MESHES_ASSIGN);
    meshesAssign = iniFile->ReadString("MeshesAssign");
    if (meshesAssign.IsEmpty())
        return false;

    iniFile->SetDefault(N_MAXEXPORT_SHADERS_ASSIGN);
    shadersAssign = iniFile->ReadString("ShadersAssign");
    if (shadersAssign.IsEmpty())
        return false;

    iniFile->SetDefault(N_MAXEXPORT_TEXTURES_ASSIGN);
    texturesAssign = iniFile->ReadString("TexturesAssign");
    if (texturesAssign.IsEmpty())
        return false;

    iniFile->SetDefault(N_MAXEXPORT_ANIMS_PATH);
    animsPath = iniFile->ReadString("AnimsPath");
    if (animsPath.IsEmpty())
        return false;

    iniFile->SetDefault(N_MAXEXPORT_GFXLIB_PATH);
    gfxlibPath = iniFile->ReadString("GfxlibPath");
    if (gfxlibPath.IsEmpty())
        return false;

    iniFile->SetDefault(N_MAXEXPORT_GUI_PATH);
    guiPath = iniFile->ReadString("GuiPath");
    if (guiPath.IsEmpty())
        return false;

    iniFile->SetDefault(N_MAXEXPORT_LIGHTS_PATH);
    lightsPath = iniFile->ReadString("LightsPath");
    if (lightsPath.IsEmpty())
        return false;

    iniFile->SetDefault(N_MAXEXPORT_MESHES_PATH);
    meshesPath = iniFile->ReadString("MeshesPath");
    if (meshesPath.IsEmpty())
        return false;

    iniFile->SetDefault(N_MAXEXPORT_SHADERS_PATH);
    shadersPath = iniFile->ReadString("ShadersPath");
    if (shadersPath.IsEmpty())
        return false;

    iniFile->SetDefault(N_MAXEXPORT_TEXTURES_PATH);
    texturesPath = iniFile->ReadString("TexturesPath");
    if (texturesPath.IsEmpty())
        return false;

    iniFile->Release();

    if (!fileServer->DirectoryExists(this->homeDir.Get()))
    {
        n_listener("Home path '%s' does not exist.", this->homeDir.Get());
        return false;
    }
    else
    {
        nString tmp;

        fileServer->SetAssign("home", homeDir.Get());

        if (this->projDir == ".")
        {
            // the 'proj' dir was not specified, set 'home' dir for it.
            fileServer->SetAssign("proj", fileServer->GetAssign("home"));
        }
        else
        {
            fileServer->SetAssign("proj", projDir.Get());
        }

        if (!fileServer->DirectoryExists(binaryPath.Get()))
        {
            tmp += "\n  binary path: ";
            tmp += binaryPath;
        }

        if (!fileServer->DirectoryExists(animsAssign.Get()))
        {
            tmp += "\n  'anims' assign: ";
            tmp += animsAssign;
        }
        else
        {
            fileServer->SetAssign("anims", animsAssign.Get());
            if (!fileServer->DirectoryExists(animsPath.Get()))
            {
                tmp += "\n  'Animation' path: ";
                tmp += animsPath;
            }
        }

        if (!fileServer->DirectoryExists(gfxlibAssign.Get()))
        {
            tmp += "\n  'gfxlib' assign: ";
            tmp += gfxlibAssign;
        }
        else
        {
            fileServer->SetAssign("gfxlib", gfxlibAssign.Get());
            if (!fileServer->DirectoryExists(gfxlibPath.Get()))
            {
                tmp += "\n  'Gfx' path: ";
                tmp += gfxlibPath;
            }
        }

        if (!fileServer->DirectoryExists(guiAssign.Get()))
        {
            tmp += "\n  'gui' assign: ";
            tmp += guiAssign;
        }
        else
        {
            fileServer->SetAssign("gui", guiAssign.Get());
            if (!fileServer->DirectoryExists(guiPath.Get()))
            {
                tmp += "\n  'Gui' path: ";
                tmp += guiPath;
            }
        }

        if (!fileServer->DirectoryExists(lightsAssign.Get()))
        {
            tmp += "\n  'lights' assign: ";
            tmp += lightsAssign;
        }
        else
        {
            fileServer->SetAssign("lights", lightsAssign.Get());
            if (!fileServer->DirectoryExists(lightsPath.Get()))
            {
                tmp += "\n  'Lights' path: ";
                tmp += lightsPath;
            }
        }

        if (!fileServer->DirectoryExists(meshesAssign.Get()))
        {
            tmp += "\n  'meshes' assign: ";
            tmp += meshesAssign;
        }
        else
        {
            fileServer->SetAssign("meshes", meshesAssign.Get());
            if (!fileServer->DirectoryExists(meshesPath.Get()))
            {
                tmp += "\n  'Meshes' path: ";
                tmp += meshesPath;
            }
        }

        if (!fileServer->DirectoryExists(shadersAssign.Get()))
        {
            tmp += "\n  'shaders' assign: ";
            tmp += shadersAssign;
        }
        else
        {
            fileServer->SetAssign("shaders", shadersAssign.Get());
            if (!fileServer->DirectoryExists(shadersPath.Get()))
            {
                tmp += "\n  'Shaders' path: ";
                tmp += shadersPath;
            }
        }

        if (!fileServer->DirectoryExists(texturesAssign.Get()))
        {
            tmp += "\n  'textures' assign: ";
            tmp += texturesAssign;
        }
        else
        {
            fileServer->SetAssign("textures", texturesAssign.Get());
            if (!fileServer->DirectoryExists(texturesPath.Get()))
            {
                tmp += "\n  'Textures' path: ";
                tmp += texturesPath;
            }
        }

        if (tmp.Length() > 0)
        {
            nString alerts("ALERT: assignment of paths are wrong:");
            alerts += tmp;
            n_listener("%s", alerts.Get());
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
/**
    Load various utility options from nmaxtoolbox.ini file.

    @note
    If you add any optin in .ini file, don't forget to add code saves the option 
    in nMaxOptions::SaveUtilityOptions() function.
*/
bool nMaxOptions::LoadUtilityOptions()
{
    nString iniFilename;
    iniFilename += GetCOREInterface()->GetDir(APP_PLUGCFG_DIR);
    iniFilename += "\\";
    iniFilename += N_MAXEXPORT_INIFILE;

    nFileServer2* fileServer = nFileServer2::Instance();

    // check the .ini file exist in 3dsmax plugin directory.
    if (!fileServer->FileExists(iniFilename.Get()))
    {
        // .ini file does not exist in '/plugcfg' directory.
        n_listener("%s file does not exist in '$3dsmax/plugcfg' directory.", N_MAXEXPORT_INIFILE);
        return false;
    }

    nIniPrefServer* iniFile = (nIniPrefServer*)nKernelServer::Instance()->New("niniprefserver", "/iniprefsrv");
    iniFile->SetFileName(iniFilename);
    iniFile->SetSection("UtilityOptions");

    iniFile->SetDefault("0.01f");
    this->geomScale = iniFile->ReadFloat ("GeometryScale");

    iniFile->SetDefault("24");
    this->maxJointPaletteSize = iniFile->ReadInt ("MaxJointPalette");

    iniFile->SetDefault("0.0001f");
    this->weightThreshold = iniFile->ReadFloat ("WeightThreshold");

    iniFile->SetDefault(".n3d2");
    this->meshFileExtension = iniFile->ReadString("SaveMeshAs");

    iniFile->SetDefault(".nanim2");
    this->animFileExtension = iniFile->ReadString("SaveAnimAs");

    iniFile->SetDefault("2");
    this->sampleRate = iniFile->ReadInt ("SampleRate");

    //FIXME: change it to use ReadBool()
    int tmpExportHiddenNode;

    iniFile->SetDefault("false");
    tmpExportHiddenNode = iniFile->ReadBool("ExportHiddenNode");

    iniFile->SetDefault("ntclserver");
    this->saveScriptServer = iniFile->ReadString("ScriptServer");
    if (this->saveScriptServer.IsEmpty())
    {
        n_message("Faied to read script server option in nmaxtoolbox.ini. \
                  'ntclserver' is used as default.");
    }

    iniFile->SetDefault("4");
    this->verboseLevel= iniFile->ReadInt("Verbose");
    if (this->verboseLevel <0 || this->verboseLevel > 5)
    {
        n_message("Faied to read verbose option in %s or \
                   wrong verbose level in the ini file. \
                   Set to 'high' for the default value.", iniFilename.Get());
    }

    iniFile->Release();

    return true;
}

//-----------------------------------------------------------------------------
/**
    Save the settings to nmaxtoolbox.ini file. Called when the plugin is detached.
    
    @note 
    The settings of 'GeneralSettings' section in the nmaxtoolbox.ini are saved
    when the 'ok' buttion of the directory setting dialog is clicked.
        
*/
void nMaxOptions::SaveUtilityOptions()
{
    // write the values to .ini file.
    nString iniFilename;
    iniFilename += GetCOREInterface()->GetDir(APP_PLUGCFG_DIR);
    iniFilename += "\\";
    iniFilename += N_MAXEXPORT_INIFILE;

    nIniPrefServer* iniFile = (nIniPrefServer*)nKernelServer::Instance()->New("niniprefserver", "/iniprefsrv");
    iniFile->SetFileName(iniFilename);
    iniFile->SetSection("UtilityOptions");

    iniFile->WriteFloat ("GeometryScale",   this->geomScale);
    iniFile->WriteInt   ("MaxJointPalette", this->maxJointPaletteSize);
    iniFile->WriteFloat ("WeightThreshold", this->weightThreshold);
    iniFile->WriteString("SaveMeshAs",      this->meshFileExtension);
    iniFile->WriteString("SaveAnimAs",      this->animFileExtension);
    iniFile->WriteInt   ("SampleRate",      this->sampleRate);
    iniFile->WriteBool  ("ExportHiddenNode",this->exportHiddenNodes);
    iniFile->WriteString("ScriptServer",    this->saveScriptServer);
    iniFile->WriteInt   ("Verbose",         this->verboseLevel);

    iniFile->Release();
}

//-----------------------------------------------------------------------------
void nMaxOptions::SetExportNormals(bool status)
{
    this->exportNormal = status;
}
//-----------------------------------------------------------------------------
void nMaxOptions::SetExportColors(bool status)
{
    this->exportVertColor = status;
}
//-----------------------------------------------------------------------------
void nMaxOptions::SetExportUvs(bool status)
{
    this->exportUvs = status;
}
//-----------------------------------------------------------------------------
void nMaxOptions::SetExportTangents(bool status)
{
    this->exportTangent = status;
}
//-----------------------------------------------------------------------------
void nMaxOptions::SetExportHiddenNodes(bool status)
{
    this->exportHiddenNodes = status;
}
//-----------------------------------------------------------------------------
bool nMaxOptions::ExportNormals()
{
    return this->exportNormal;
}
//-----------------------------------------------------------------------------
bool nMaxOptions::ExportColors()
{
    return this->exportVertColor;
}
//-----------------------------------------------------------------------------
bool nMaxOptions::ExportUvs()
{
    return this->exportUvs;
}
//-----------------------------------------------------------------------------
bool nMaxOptions::ExportTangents()
{
    return this->exportTangent;
}
//-----------------------------------------------------------------------------
bool nMaxOptions::ExportHiddenNodes()
{
    return this->exportHiddenNodes;
}
//-----------------------------------------------------------------------------
void nMaxOptions::SetSampleRate(int val)
{
    this->sampleRate = val;
}
//-----------------------------------------------------------------------------
int nMaxOptions::GetSampleRate() const
{
    return this->sampleRate;
}
//-----------------------------------------------------------------------------
/**
*/
const char* nMaxOptions::GetViewerDir() const
{
    return this->binaryPath.Get();
}
//-----------------------------------------------------------------------------
/**
*/
bool nMaxOptions::RunViewer()
{
    return this->runViewer;
}
//-----------------------------------------------------------------------------
const nString& 
nMaxOptions::GetHomePath() const
{
    return this->homeDir;
}

//-----------------------------------------------------------------------------
const nString&
nMaxOptions::GetProjPath() const
{
    return this->projDir;
}
//-----------------------------------------------------------------------------
const nString& 
nMaxOptions::GetAnimAssign() const
{
    return this->animsAssign;
}
//-----------------------------------------------------------------------------
const nString&
nMaxOptions::GetAnimPath() const
{
    return this->animsPath;
}
//-----------------------------------------------------------------------------
const nString& 
nMaxOptions::GetGfxLibAssign() const
{
    return this->gfxlibAssign;
}
//-----------------------------------------------------------------------------
const nString& 
nMaxOptions::GetGfxLibPath() const
{
    return this->gfxlibPath;
}
//-----------------------------------------------------------------------------
const nString& 
nMaxOptions::GetMeshesAssign() const
{
    return this->meshesAssign;
}
//-----------------------------------------------------------------------------
const nString& 
nMaxOptions::GetMeshesPath() const
{
    return this->meshesPath;
}
//-----------------------------------------------------------------------------
const nString& 
nMaxOptions::GetTextureAssign() const
{
    return this->texturesAssign;
}
//-----------------------------------------------------------------------------
const nString& 
nMaxOptions::GetTexturePath() const
{
    return this->texturesPath;
}
//-----------------------------------------------------------------------------
bool nMaxOptions::OverwriteExistTexture() const
{
    return this->overwriteExistTexture;
}
