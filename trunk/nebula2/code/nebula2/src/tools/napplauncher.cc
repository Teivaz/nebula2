//------------------------------------------------------------------------------
//  napplauncher.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "tools/napplauncher.h"
#include "kernel/nfileserver2.h"
#include <windows.h>
#include <shellapi.h>

//------------------------------------------------------------------------------
/**
*/
nAppLauncher::nAppLauncher(nKernelServer* ks) :
    kernelServer(ks),
    noConsoleWindow(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nAppLauncher::~nAppLauncher()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
bool
nAppLauncher::LaunchWait() const
{
    n_assert(!this->exec.IsEmpty());
    n_assert(this->kernelServer);

    STARTUPINFO startupInfo = { sizeof(STARTUPINFO), 0 };
    PROCESS_INFORMATION processInfo = { 0 };

    // mangle paths
    nString execMangled = nFileServer2::Instance()->ManglePath(this->exec.Get());
    nString dirMangled  = nFileServer2::Instance()->ManglePath(this->dir.Get());

    // build a command line
    nString cmdLine = execMangled;
    cmdLine.Append(" ");
    cmdLine.Append(this->args.Get());

    DWORD creationFlags = 0;
    if (this->noConsoleWindow)
    {
        creationFlags |= CREATE_NO_WINDOW;
    }
    if (!CreateProcess(NULL,                    // lpApplicationName
                       (LPSTR) cmdLine.Get(),   // lpCommandLine
                       NULL,                // lpProcessAttributes
                       NULL,                // lpThreadAttributes
                       FALSE,               // bInheritsHandle
                       creationFlags,       // dwCreationFlags
                       NULL,                // lpEnvironment
                       dirMangled.Get(),    // lpCurrentDirectory
                       &startupInfo,        // lpStartupInfo
                       &processInfo))       // lpProcessInformation
    {
        return false;
    }

    // wait until process exits
    WaitForSingleObject(processInfo.hProcess, INFINITE);
    
    // cleanup
    CloseHandle(processInfo.hProcess);
    CloseHandle(processInfo.hThread);

    return true;
}

//------------------------------------------------------------------------------
/**
*/
bool
nAppLauncher::Launch() const
{
    n_assert(!this->exec.IsEmpty());
    n_assert(this->kernelServer);

    // mangle paths
    nString execMangled = nFileServer2::Instance()->ManglePath(this->exec.Get());
    nString dirMangled  = nFileServer2::Instance()->ManglePath(this->dir.Get());

    HINSTANCE retval = ShellExecute(NULL,                   // hWnd
                                    "open",                 // lpOperation
                                    execMangled.Get(),      // lpFile
                                    this->args.Get(),       // lpParameters
                                    dirMangled.Get(),       // lpDirectory
                                    SW_SHOW);               // nShowCmd
    return ((int)retval > 32);
}


