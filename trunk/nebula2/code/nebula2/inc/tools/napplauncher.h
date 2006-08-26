#ifndef N_APPLAUNCHER_H
#define N_APPLAUNCHER_H
//------------------------------------------------------------------------------
/**
    @class nAppLauncher
    @ingroup Tools

    Launch a command line utility and wait for its termination.

    This is an example of how to use the @ref nAppLauncher class.
    @code
    // create nAppLauncher object instance.
    nAppLauncher appLauncher;

    nString app = "nviewer.exe";
    nString cwd = "c:\nebula2\bin\win32";
    nString args = "-view home:export/gfxlib/examples/torus.n2";

    // specify the path of executable file and working directory.
    appLauncher.SetExecutable(app);
    appLauncher.SetWorkingDirectory(cwd);
    appLauncher.SetArguments(args);

    // execute the application.
    appLauncher.Launch();
    @endcode

    FIXME: operating system specific subclasses should be created.

    (C) 2003 RadonLabs GmbH
*/
#include "kernel/nkernelserver.h"

//------------------------------------------------------------------------------
class nAppLauncher
{
public:
    /// constructor
    nAppLauncher();
    /// destructor
    ~nAppLauncher();
    /// set the name of the executable, including path, if not in search path
    void SetExecutable(const nString& path);
    /// set optional URL path
    void SetUrl(const nString& url);
    /// set the working directory
    void SetWorkingDirectory(const nString& path);
    /// do not open a console window for the new process
    void SetNoConsoleWindow(bool b);
    /// set the command line args (just the arg string, without the executable)
    void SetArguments(const nString& args);
    /// launch app and wait
    bool LaunchWait() const;
    /// launch app and return
    bool Launch() const;

private:
    bool noConsoleWindow;
    nString exec;;
    nString dir;
    nString args;
    nString url;

#ifdef __LINUX__
    bool LaunchHelper(bool waitForChild) const;
#endif
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nAppLauncher::SetUrl(const nString& u)
{
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
nAppLauncher::SetExecutable(const nString& path)
{
    this->exec = path;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAppLauncher::SetWorkingDirectory(const nString& path)
{
    this->dir = path;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAppLauncher::SetArguments(const nString& path)
{
    this->args = path;
}

//------------------------------------------------------------------------------
#endif

