//------------------------------------------------------------------------------
//  napplauncher.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "tools/napplauncher.h"
#include "kernel/nfileserver2.h"
#ifdef __WIN32__
#include <windows.h>
#include <shellapi.h>
#elif defined(__LINUX__)
#include <limits.h>
#include <sys/wait.h>
#include <unistd.h>
#endif

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

#ifdef __WIN32__
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
#elif defined(__LINUX__)
    return this->LaunchHelper(true);
#else
    return false;
#endif

    return true;
}

//------------------------------------------------------------------------------
/**
    The path of executable file and directory are should be specified
    before launching the application by calling nAppLauncher::SetExecutable() and
    nAppLauncher::SetWorkingDirectory().  
    And aguments can be specified by calling nAppLauncher::SetArguments() if 
    those are needed.

    - 15-Oct-04 kims fixed a string invalidation bug on 'execmangled' and 
                   'dirMangled' variables.
*/
bool
nAppLauncher::Launch() const
{
    n_assert(!this->exec.IsEmpty());
    n_assert(this->kernelServer);

    nString execMangled;
    nString dirMangled;

    // mangle paths
    const char* filePath = 0;
    if (!this->url.IsEmpty())
    {
        filePath = this->url.Get();
    }
    else
    {
        execMangled = nFileServer2::Instance()->ManglePath(this->exec.Get());
        filePath = execMangled.Get();
    }
    const char* dirPtr = 0;
    if (!this->dir.IsEmpty())
    {
        dirMangled  = nFileServer2::Instance()->ManglePath(this->dir.Get());
        dirPtr = dirMangled.Get();
    }
#ifdef __WIN32__
    const char* argPtr = 0;
    if (!this->args.IsEmpty())
    {
        argPtr = this->args.Get();
    }
    HINSTANCE retval = ShellExecute(NULL,                   // hWnd
                                    "open",                 // lpOperation
                                    filePath,               // lpFile
                                    argPtr,                 // lpParameters
                                    dirPtr,                 // lpDirectory
                                    SW_SHOW);               // nShowCmd
    return ((int)retval > 32);
#elif defined(__LINUX__)
    return this->LaunchHelper(false);
#else
    return false;
#endif
}

#ifdef __LINUX__
bool
nAppLauncher::LaunchHelper(bool waitForChild) const
{
    // mangle paths
    nString execMangled = nFileServer2::Instance()->ManglePath(this->exec.Get());
    nString dirMangled  = nFileServer2::Instance()->ManglePath(this->dir.Get());

    nString appCmdLine;
    appCmdLine.Format("%s/%s", dirMangled.Get(), execMangled.Get());
    const char *execCmd = appCmdLine.Get();

    // fork this process, and replace it with intended target
    int pid = fork();
    if (pid == 0) {
        // +2 -> 1 for the command name, 1 for the NULL
        char  *argv[ARG_MAX + 2];
        argv[0] = (char*)execMangled.Get();
        int argc = 1;

        // tokenize command line
        // XXX: This cast is bad.
        char* ptr = (char*)this->args.Get();
        char* end = ptr + strlen(ptr);

        while ((argc < ARG_MAX) && (ptr < end))
        {
            char* c;
            while ((' ' == *ptr) || ('\t' == *ptr))
                ptr++;
            if (('"' == *ptr) && (c = strchr(++ptr, '"')))
            {
                *c++ = 0;
                argv[argc++] = ptr;
                ptr = c;
            }
            else if ((c = strpbrk(ptr, N_WHITESPACE)))
            {
                *c++ = 0;
                argv[argc++] = ptr;
                ptr = c;
            }
            else
            {
                argv[argc++] = ptr;
                break;
            }
        }
        argv[argc] = NULL;

        // we have the args, so now we can exec the new process
        execv(execCmd, argv);
        exit(-1);
    } else if (pid > 0) {
        // Wait for the child...
        int status = 0;
        if (waitForChild)
        {
            waitpid(pid, &status, 0);
        }
        else
        {
            waitpid(pid, &status, WNOHANG);
        }
    } else {
        // Failed to fork
        return false;
    }

    return true;
}
#endif

