#ifndef N_SHELL32WRAPPER_H
#define N_SHELL32WRAPPER_H
//------------------------------------------------------------------------------
/**
    @class nShell32Wrapper
    @ingroup Kernel

    Wrap some Win32 shell32 functionality to fix some backward
    compatibility issues between Win95, Win98, Win2000 and XP.

    (C) 2004 RadonLabs GmbH
*/
#include "kernel/nsystem.h"
#include <shfolder.h>

#ifndef __WIN32__
#error "Win32 specific class!"
#endif

//------------------------------------------------------------------------------
class nShell32Wrapper
{
public:
    /// constructor
    nShell32Wrapper();
    /// destructor
    ~nShell32Wrapper();
    /// invoke the SHGetFolderPath() function
    HRESULT SHGetFolderPath(HWND hwndOwner, int nFolder, HANDLE hToken, DWORD dwFlags, LPTSTR pszPath);

private:
    HINSTANCE shell32Inst;
    HINSTANCE shfolderInst;
    PFNSHGETFOLDERPATH shGetFolderPathFunc;
};

//------------------------------------------------------------------------------
/**
*/
inline
nShell32Wrapper::nShell32Wrapper() :
    shell32Inst(0),
    shfolderInst(0),
    shGetFolderPathFunc(0)
{
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
        n_error("Could not find SHGetFolderPath() function\n"
                "because of unsupported Windows version!");
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
nShell32Wrapper::~nShell32Wrapper()
{
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
*/
inline
HRESULT
nShell32Wrapper::SHGetFolderPath(HWND hwndOwner, int nFolder, HANDLE hToken, DWORD dwFlags, LPTSTR pszPath)
{
    n_assert(this->shGetFolderPathFunc);
    return this->shGetFolderPathFunc(hwndOwner, nFolder, hToken, dwFlags, pszPath);
}

//------------------------------------------------------------------------------
#endif // N_SHELL32WRAPPER_H
