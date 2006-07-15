#ifndef N_WIN32WRAPPER_H
#define N_WIN32WRAPPER_H
//------------------------------------------------------------------------------
/**
    @class nWin32Wrapper
    @ingroup Kernel

    A wrapper class for specific Win32 functions. An instance of the
    nWin32Wrapper class is created globally by the nKernelServer.

    (C) 2004 RadonLabs GmbH
*/
#include "kernel/nsystem.h"
#include <windows.h>
#include <shfolder.h>
#include "kernel/ntypes.h"

#ifndef __WIN32__
#error "Win32 specific class!"
#endif

//------------------------------------------------------------------------------
class nWin32Wrapper
{
public:
    /// windows versions
    enum WindowsVersion
    {
        Unknown,
        Win32_NT,           // NT, 2000, XP
        Win32_Windows,      // 95, 98, ME
    };

    /// constructor
    nWin32Wrapper();
    /// destructor
    ~nWin32Wrapper();
    /// get instance pointer
    static nWin32Wrapper* Instance();
    /// invoke the SHGetFolderPath() function
    HRESULT SHGetFolderPath(HWND hwndOwner, int nFolder, HANDLE hToken, DWORD dwFlags, LPTSTR pszPath);
    /// get Win32 version
    WindowsVersion GetWindowsVersion() const;

private:
    static nWin32Wrapper* Singleton;
    HINSTANCE shell32Inst;
    HINSTANCE shfolderInst;
    PFNSHGETFOLDERPATH shGetFolderPathFunc;
    WindowsVersion windowsVersion;
};

//------------------------------------------------------------------------------
/**
*/
inline
nWin32Wrapper*
nWin32Wrapper::Instance()
{
    n_assert(Singleton);
    return Singleton;
}

//------------------------------------------------------------------------------
/**
*/
inline
nWin32Wrapper::WindowsVersion
nWin32Wrapper::GetWindowsVersion() const
{
    return this->windowsVersion;
}

//------------------------------------------------------------------------------
#endif
