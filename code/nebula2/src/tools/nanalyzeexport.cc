//------------------------------------------------------------------------------
//  nanalyzeexport.cc
//  (C) 2006 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "tools/nexportanalyzer.h"
#include "tools/nviewerapp.h"
#include "gfx2/ngfxserver2.h"

nNebulaUsePackage(nnebula);
nNebulaUsePackage(ndinput8);
nNebulaUsePackage(ndirect3d9);
nNebulaUsePackage(ndshow);
nNebulaUsePackage(ngui);
nNebulaUsePackage(nnetwork);
nNebulaUsePackage(ndsound);

//------------------------------------------------------------------------------
/*
    Win32 specific method which checks the registry for the Nebula2
    Toolkit's project path. If the reg keys are not found, the
    routine just returns 0.
*/
nString
ReadProjRegistryKey()
{
    // read the project directory from the registry
    HKEY hKey;
    LONG err;
    err = RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\RadonLabs\\Toolkit", 0, KEY_QUERY_VALUE, &hKey);
    if (ERROR_SUCCESS != err)
    {
        return 0;
    }
    DWORD keyType;
    static char projectDir[512];
    DWORD projectDirSize = sizeof(projectDir);
    err = RegQueryValueEx(hKey, "project", 0, &keyType, (LPBYTE) &projectDir, &projectDirSize);
    if (ERROR_SUCCESS != err)
    {
        return 0;
    }
    return nString(projectDir);
}

//------------------------------------------------------------------------------
/*
*/
int
main(int argc, const char** argv)
{
    // initialize Nebula runtime
    nKernelServer kernelServer;
    kernelServer.AddPackage(nnebula);
    kernelServer.AddPackage(ndinput8);
    kernelServer.AddPackage(ndirect3d9);
    kernelServer.AddPackage(ndshow);
    kernelServer.AddPackage(ngui);
    kernelServer.AddPackage(nnetwork);
    kernelServer.AddPackage(ndsound);

    // initialize a display mode object
    nDisplayMode2 displayMode;
    displayMode.SetWindowTitle("Nebula2 Export Analyzer");
    displayMode.SetIcon("NebulaIcon");
    displayMode.SetDialogBoxMode(true);
    displayMode.SetXPos(0);
    displayMode.SetYPos(0);
    displayMode.SetWidth(160);
    displayMode.SetHeight(20);
    displayMode.SetType(nDisplayMode2::Windowed);

    nString projDir = ReadProjRegistryKey();
    if (!projDir.IsValid())
    {
        projDir = "home:";
    }
    nViewerApp viewerApp;
    viewerApp.SetDisplayMode(displayMode);
    viewerApp.SetProjDir(projDir);
    viewerApp.SetOverlayEnabled(false);
    viewerApp.SetStartupScript("proj:data/scripts/startup.tcl");
    if (viewerApp.Open())
    {
        nExportAnalyzer analyzer;
        analyzer.Run();
        viewerApp.Close();
    }
    return 0;
}

