#ifndef N_APPLAUNCHER_H
#define N_APPLAUNCHER_H
//------------------------------------------------------------------------------
/**
    @class nAppLauncher
    @ingroup Tools
    
    Launch a command line utility and wait for its termination.
    FIXME: operating system specific subclasses should be created.
    
    (C) 2003 RadonLabs GmbH
*/
#include "kernel/nkernelserver.h"

//------------------------------------------------------------------------------
class nAppLauncher
{
public:
    /// constructor
    nAppLauncher(nKernelServer* kernelServer);
    /// destructor
    ~nAppLauncher();
    /// set the name of the executable, including path, if not in search path
    void SetExecutable(const char* path);
    /// set optional URL path
    void SetUrl(const char* url);
    /// set the working directory
    void SetWorkingDirectory(const char* path);
    /// do not open a console window for the new process
    void SetNoConsoleWindow(bool b);
    /// set the command line args (just the arg string, without the executable)
    void SetArguments(const char* args);
    /// launch app and wait 
    bool LaunchWait() const;
    /// launch app and return
    bool Launch() const;

private:
    nKernelServer* kernelServer;
    bool noConsoleWindow;
    nString exec;
    nString dir;
    nString args;
    nString url;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nAppLauncher::SetUrl(const char* u)
{
    n_assert(u);
    this->url = u;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAppLauncher::SetNoConsoleWindow(bool b)
{
    this->noConsoleWindow = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAppLauncher::SetExecutable(const char* path)
{
    n_assert(path);
    this->exec = path;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAppLauncher::SetWorkingDirectory(const char* path)
{
    n_assert(path);
    this->dir = path;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAppLauncher::SetArguments(const char* path)
{
    n_assert(path);
    this->args = path;
}

//------------------------------------------------------------------------------
#endif

