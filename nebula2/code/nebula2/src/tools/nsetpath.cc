//------------------------------------------------------------------------------
//  nsetpath.cc
//
//  Command line tool for changing the Nebula2 toolkit registry keys.
//
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include <stdio.h>
#include "tools/ncmdlineargs.h"
#include <windows.h>

//------------------------------------------------------------------------------
/**
*/
int main(int argc, const char** argv)
{
    nCmdLineArgs args(argc, argv);

    const char* projDirArg    = args.GetStringArg("-projdir", 0);
    const char* toolkitDirArg = args.GetStringArg("-toolkitdir", 0);
    bool helpArg = args.GetBoolArg("-help");
    if (helpArg)
    {
        printf("nsetpath [-projdir] [-toolkitdir] [-help]\n"
               "(C) 2004 RadonLabs GmbH\n"
               "Set Nebula2 Toolkit path registry keys\n\n"
               "-projdir        - path to current project\n"
               "-toolkitdir     - path to toolkit directory\n"
               "-help           - show this help\n");
        return 5;
    }

    // open RadonLabs/Toolkit key
    HKEY hKey;
    LONG err;
    err = RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\RadonLabs\\Toolkit", 0, KEY_SET_VALUE, &hKey);
    if (ERROR_SUCCESS != err)
    {
        printf("Failed to open the RadonLabs Toolkit registry key ('HKEY_LOCAL_MACHINE\\SOFTWARE\\RadonLabs\\Toolkit)!\n");
        return 10;
    }
    if (projDirArg)
    {
        err = RegSetValueEx(hKey, "project", 0, REG_SZ, (const BYTE*) projDirArg, strlen(projDirArg) + 1);
        if (err != ERROR_SUCCESS)
        {
            RegCloseKey(hKey);
            printf("Failed to write to registry key 'HKEY_LOCAL_MACHINE\\SOFTWARE\\RadonLabs\\Toolkit\\project'!\n");
            return 10;
        }
    }
    if (toolkitDirArg)
    {
        err = RegSetValueEx(hKey, "path", 0, REG_SZ, (const BYTE*) toolkitDirArg, strlen(toolkitDirArg) + 1);
        if (err != ERROR_SUCCESS)
        {
            RegCloseKey(hKey);
            printf("Failed to write to registry key 'HKEY_LOCAL_MACHINE\\SOFTWARE\\RadonLabs\\Toolkit\\project'!\n");
            return 10;
        }
    }
    RegCloseKey(hKey);
    return 0;
}
