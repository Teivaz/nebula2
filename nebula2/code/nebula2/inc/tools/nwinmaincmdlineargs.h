#ifndef N_WINMAINCMDLINEARGS_H
#define N_WINMAINCMDLINEARGS_H
//------------------------------------------------------------------------------
/**
    Specialized nCmdLine subclass which accepts a cmd line string
    as provided by the Win32 WinMain function.

    FIXME: currently has problems with args which have spaces in them
    (for instance path name with spaces).
    
    (C) 2003 RadonLabs GmbH
*/
#ifndef N_CMDLINEARGS_H
#include "tools/ncmdlineargs.h"
#endif

//------------------------------------------------------------------------------
class nWinMainCmdLineArgs : public nCmdLineArgs
{
public:
    /// constructor
    nWinMainCmdLineArgs(const char* cmdLine);

private:
    enum
    {
        MAXARGS = 64,
    };
    nString argString;
    int argc;
    const char* argv[MAXARGS];
};

//------------------------------------------------------------------------------
/**
*/
inline
nWinMainCmdLineArgs::nWinMainCmdLineArgs(const char* cmdLine)
{
    memset(this->argv, 0, sizeof(this->argv));

    // write a dummy executable args
    this->argc = 1;
    this->argv[0] = "invalid";
    
    // tokenize command line
    this->argString = cmdLine;
    char* ptr = (char*) argString.Get();    
    while ((this->argc < MAXARGS) && (argv[this->argc] = strtok(ptr, N_WHITESPACE ";")))
    {
        this->argc++;
        ptr = 0;
    }

    // invoke parent constructor
    this->Initialize(this->argc, this->argv);
}
//------------------------------------------------------------------------------
#endif


