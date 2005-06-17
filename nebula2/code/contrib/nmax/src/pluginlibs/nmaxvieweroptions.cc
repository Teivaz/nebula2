//------------------------------------------------------------------------------
//  nmaxvieweroptions.cc
//
//  (C)2005 Kim, Hyoun Woo
//------------------------------------------------------------------------------
#include "export2/nmax.h"
#include "export2/nmaxoptions.h"
#include "pluginlibs/nmaxdlg.h"
#include "pluginlibs/nmaxlogdlg.h"
#include "pluginlibs/nmaxvieweroptions.h"
#include "pluginlibs/ninifile.h"

#include "kernel/nfileserver2.h"
#include "kernel/nfile.h"

//------------------------------------------------------------------------------
/**
*/
nMaxViewerOptions::nMaxViewerOptions() :
    viewerType(Default),
    usePreview(false),
    executable("nviewer.exe")
{
}

//------------------------------------------------------------------------------
/**
*/
nMaxViewerOptions::~nMaxViewerOptions()
{
}

//------------------------------------------------------------------------------
/**
*/
bool nMaxViewerOptions::Read()
{
    nString iniFilename;
    iniFilename += GetCOREInterface()->GetDir(APP_PLUGCFG_DIR);
    iniFilename += "\\";
    iniFilename += N_MAXEXPORT_INIFILE;

    nFileServer2* fileServer = nFileServer2::Instance(); 

    // check the .ini file exist in 3dsmax plugin directory.
    if (!fileServer->FileExists(iniFilename.Get()))
    {
        // the .ini file does not exist, so make new one.
        nFile* file = fileServer->NewFileObject();
        file->Open(iniFilename.Get(), "w");
        file->Close();
        file->Release();
    }

    // read viewer options.
    bool result = false;
    switch(viewerType)
    {
    case Default:
        result = ReadDefaultViewerOptions(iniFilename, "DefaultViewerSetting");
        break;
    case Custom:
        result = ReadCustomViewerOptions(iniFilename, "CustomViewerSetting");
        break;
    }

    if (!result)
    {
        n_maxlog(Warning, "Error on reading viewer options.");
        return false;
    }

    // specifies scene file name.
    if (!this->sceneFilename.IsEmpty())
    {
        this->arguments += "-view ";
        this->arguments += nMaxOptions::Instance()->GetGfxLibAssign();
        this->arguments += this->sceneFilename;
        this->arguments += " ";
    }
    else
    {
        n_maxlog(Warning, "No scene filename was specified.");
        return false;
    }

    return true;
}

//------------------------------------------------------------------------------
/**
    Read viewer options for default viewer.

    @param iniFilename .ini filename which contains default viewer options.
    @param sectionName section name in .ini file. "DefaultViewerSetting" is used 
                       for a custom viewer.
*/
bool nMaxViewerOptions::ReadDefaultViewerOptions(const nString &iniFilename, 
                                                 const nString &sectionName)
{
    nString scriptServer;
    nString x,y,w,h;
    uint fullscreen, alwaysontop;
    nString projDir;
    nString featureset;
    nString gfxServer;
    nString sceneServer;
    nString stage;

    nIniFile iniFile(iniFilename, sectionName);

    iniFile.ReadString("-scriptserver", scriptServer, "ntclserver");
    if (!scriptServer.IsEmpty())
    {
        this->arguments += "-scriptserver ";
        this->arguments += scriptServer;
        this->arguments += " ";
    }

    iniFile.ReadUINT("-fullscreen", fullscreen, 0);
    if (fullscreen > 0)
        this->arguments += "-fullscreen ";

    iniFile.ReadUINT("-alwaysontop", alwaysontop, 0);
    if (alwaysontop > 0)
        this->arguments += "-alwaysontop ";

    iniFile.ReadString("-x", x, "0");
    this->arguments += "-x";
    this->arguments += " ";
    this->arguments += x;
    this->arguments += " ";

    iniFile.ReadString("-y", y, "0");
    this->arguments += "-y";
    this->arguments += " ";
    this->arguments += y;
    this->arguments += " ";

    iniFile.ReadString("-w", w, "640");
    this->arguments += "-w ";
    this->arguments += w;
    this->arguments += " ";

    iniFile.ReadString("-h", h, "480");
    this->arguments += "-h ";
    this->arguments += h;
    this->arguments += " ";

    // projdir arg.
    iniFile.ReadString("-projdir", projDir, "");
    this->arguments += "-projdir ";
    if (!projDir.IsEmpty())
    {
        this->arguments += projDir; 
    }
    else
    {
        this->arguments += nMaxOptions::Instance()->GetHomePath();
    }
    this->arguments += " ";

    iniFile.ReadString("-sceneserver", sceneServer, "nmrtsceneserver");
    this->arguments += "-sceneserver ";
    this->arguments += sceneServer;
    this->arguments += " ";

    iniFile.ReadString("-stage", stage, "home:export/gfxlib/stdlight.n2");
    this->arguments += "-stage ";
    this->arguments += stage;
    this->arguments += " ";

    // gfxserver arg.
    iniFile.ReadString("-gfxserver", gfxServer, "");
    if (!gfxServer.IsEmpty())
    {
        this->arguments += "-gfxserver ";
        this->arguments += gfxServer;
        this->arguments += " ";
    }

    // featureset arg.
    iniFile.ReadString("-featureset", featureset, "");
    if (!featureset.IsEmpty())
    {
        this->arguments += "-featureset ";
        this->arguments += featureset;
        this->arguments += " ";
    }

    // specifies to use ram file in shared memory.
    if (nMaxOptions::Instance()->UsePreviewMode())
        this->arguments += "-useram "; //use ram file.

    return true;
}

//------------------------------------------------------------------------------
/**
    Read viewer options for custom viewer.

    @param iniFilename .ini filename which contains custom viewer options.
    @param sectionName section name in .ini file. "CustomViewerSetting" is used 
                       for a custom viewer.
*/
bool nMaxViewerOptions::ReadCustomViewerOptions(const nString &iniFilename, 
                                                const nString &sectionName)
{
    nIniFile iniFile(iniFilename, sectionName);

    nString args;

    if (!iniFile.ReadString("args", args, ""))
    {
        n_maxlog(Warning, "Failed to read '-featureset' value from [%s] in ini file\n", sectionName.Get());
        return false;
    }
    this->arguments += args;

    return false;
}

