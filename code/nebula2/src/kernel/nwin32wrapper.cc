//------------------------------------------------------------------------------
//  nwin32wrapper.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "kernel/nwin32wrapper.h"

nWin32Wrapper* nWin32Wrapper::Singleton = 0;

//------------------------------------------------------------------------------
/**
*/
nWin32Wrapper::nWin32Wrapper() :
    shell32Inst(0),
    shfolderInst(0),
    shGetFolderPathFunc(0),
    windowsVersion(Unknown)
{
    n_assert(0 == Singleton);
    Singleton = this;

    // handle SHFolder stuff
    this->shell32Inst = LoadLibrary("shell32");
    if (0 != this->shell32Inst)
    {
        this->shGetFolderPathFunc = (PFNSHGETFOLDERPATHA) GetProcAddress(this->shell32Inst, "SHGetFolderPathA");
    }
    if (0 == this->shGetFolderPathFunc)
    {
        this->shfolderInst = LoadLibrary("shfolder");
        if (0 != this->shfolderInst)
        {
            this->shGetFolderPathFunc = (PFNSHGETFOLDERPATHA) GetProcAddress(this->shfolderInst, "SHGetFolderPathA");
        }
    }
    if (0 == this->shGetFolderPathFunc)
    {
        n_error("Could not SHGetFolderPath() function\nbecause of unsupported windows version!");
    }

    // detect windows version
    OSVERSIONINFOEX osvi = { sizeof(OSVERSIONINFOEX), 0 };
    BOOL bOsVersionInfoEx = GetVersionEx((OSVERSIONINFO*)&osvi);
    if (!bOsVersionInfoEx)
    {
        // If OSVERSIONINFOEX doesn't work, try OSVERSIONINFO.
        osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
        bOsVersionInfoEx = GetVersionEx((OSVERSIONINFO*)&osvi);
    }

    if (bOsVersionInfoEx)
    {
        switch (osvi.dwPlatformId)
        {
            case VER_PLATFORM_WIN32_NT:
                this->windowsVersion = Win32_NT;
                break;
            case VER_PLATFORM_WIN32_WINDOWS:
                this->windowsVersion = Win32_Windows;
                break;
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
nWin32Wrapper::~nWin32Wrapper()
{
    n_assert(Singleton);
    Singleton = 0;

    if (0 != this->shell32Inst)
    {
        FreeLibrary(this->shell32Inst);
        this->shell32Inst = 0;
    }
    if (0 != this->shfolderInst)
    {
        FreeLibrary(this->shfolderInst);
        this->shfolderInst = 0;
    }
    this->shGetFolderPathFunc = 0;
}

//------------------------------------------------------------------------------
/**
    Implements the Win32 SHGetFolderPath() which on Win98 is only
    available through client-installed shfolder.dll.
*/
HRESULT
nWin32Wrapper::SHGetFolderPath(HWND hwndOwner, int nFolder, HANDLE hToken, DWORD dwFlags, LPTSTR pszPath)
{
    n_assert(this->shGetFolderPathFunc);
    return this->shGetFolderPathFunc(hwndOwner, nFolder, hToken, dwFlags, pszPath);
}
