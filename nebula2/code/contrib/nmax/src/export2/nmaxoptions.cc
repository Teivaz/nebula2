//-----------------------------------------------------------------------------
//  nMaxOptions.cc
//
//  (c)2004 Kim, Hyoun Woo
//-----------------------------------------------------------------------------
#include "export2/nmax.h"
#include "export2/nmaxoptions.h"
#include "pluginlibs/ninifile.h"
#include "pluginlibs/nmaxdlg.h"
#include "pluginlibs/nmaxlogdlg.h"

#include "kernel/nfileserver2.h"
#include "kernel/nfile.h"

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
    maxJointPaletteSize(24),
    weightTrashHold(0.0001f),
    sampleRate (2),
    addJointName(false),
    runViewer(true),
    saveScriptServer("ntclserver"),
    useIndivisualMesh(false),
    verboseLevel(2/*high*/),
    overwriteExistTexture(true)
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
    pathes to file server.

    @return true if the function call is success.
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
        n_maxlog(Error, "%s file does not exist in '$3dsmax/plugcfg' directory.", N_MAXEXPORT_INIFILE);
        return false;
    }

    nIniFile iniFile(iniFilename);
    if (!iniFile.ReadString("HomeDir", homeDir, ".", "GeneralSettings")) return false;
    if (!iniFile.ReadString("BinaryPath", binaryPath, ".", "GeneralSettings")) return false;

    if (!iniFile.ReadString("AnimsAssign",    animsAssign,    N_MAXEXPORT_ANIMS_ASSIGN,    "GeneralSettings")) return false;
    if (!iniFile.ReadString("GfxlibAssign",   gfxlibAssign,   N_MAXEXPORT_GFXLIB_ASSIGN,   "GeneralSettings")) return false;
    if (!iniFile.ReadString("GuiAssign",      guiAssign,      N_MAXEXPORT_GUI_ASSIGN,      "GeneralSettings")) return false;
    if (!iniFile.ReadString("LightsAssign",   lightsAssign,   N_MAXEXPORT_LIGHTS_ASSIGN,   "GeneralSettings")) return false;
    if (!iniFile.ReadString("MeshesAssign",   meshesAssign,   N_MAXEXPORT_MESHES_ASSIGN,   "GeneralSettings")) return false;
    if (!iniFile.ReadString("ShadersAssign",  shadersAssign,  N_MAXEXPORT_SHADERS_ASSIGN,  "GeneralSettings")) return false;
    if (!iniFile.ReadString("TexturesAssign", texturesAssign, N_MAXEXPORT_TEXTURES_ASSIGN, "GeneralSettings")) return false;

    if (!iniFile.ReadString("AnimsPath",    animsPath,    N_MAXEXPORT_ANIMS_PATH,    "GeneralSettings")) return false;
    if (!iniFile.ReadString("GfxlibPath",   gfxlibPath,   N_MAXEXPORT_GFXLIB_PATH,   "GeneralSettings")) return false;
    if (!iniFile.ReadString("GuiPath",      guiPath,      N_MAXEXPORT_GUI_PATH,      "GeneralSettings")) return false;
    if (!iniFile.ReadString("LightsPath",   lightsPath,   N_MAXEXPORT_LIGHTS_PATH,   "GeneralSettings")) return false;
    if (!iniFile.ReadString("MeshesPath",   meshesPath,   N_MAXEXPORT_MESHES_PATH,   "GeneralSettings")) return false;
    if (!iniFile.ReadString("ShadersPath",  shadersPath,  N_MAXEXPORT_SHADERS_PATH,  "GeneralSettings")) return false;
    if (!iniFile.ReadString("TexturesPath", texturesPath, N_MAXEXPORT_TEXTURES_PATH, "GeneralSettings")) return false;

    if (!fileServer->DirectoryExists(this->homeDir.Get()))
    {
        n_maxlog(Error, "Home path '%s' does not exist.", this->homeDir.Get());
        return false;
    }
    else
    {
        nString tmp;

        fileServer->SetAssign("home", homeDir.Get());

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
            nString alerts("ALERT: assignment of pathes are wrong:");
            alerts += tmp;
            n_maxlog(Error, "%s", alerts.Get());
            return false;
        }
    }

    return true;
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
nMaxOptions::GetAnimAssgin() const
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
