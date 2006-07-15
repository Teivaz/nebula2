#ifndef N_WINMAINCMDLINEARGS_H
#define N_WINMAINCMDLINEARGS_H
//------------------------------------------------------------------------------
/**
    @class nWinMainCmdLineArgs
    @ingroup Tools

    Specialized nCmdLine subclass which accepts a cmd line string
    as provided by the Win32 WinMain function.
    
    (C) 2003 RadonLabs GmbH
*/
#include "tools/ncmdlineargs.h"
#include "util/nstring.h"

//------------------------------------------------------------------------------
class nWinMainCmdLineArgs : public nCmdLineArgs
{
public:
    /// constructor
    nWinMainCmdLineArgs(const nString& cmdLine);

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
nWinMainCmdLineArgs::nWinMainCmdLineArgs(const nString& cmdLine)
{
    memset(this->argv, 0, sizeof(this->argv));

    // write a dummy executable args
    this->argc = 1;
    this->argv[0] = "invalid";
    
    // tokenize command line
    this->argString = cmdLine;
    char* ptr = (char*) argString.Get();    
    char* end = ptr + strlen(ptr);

    while ((this->argc < MAXARGS) && (ptr < end))
    {
        char* c;
        while ((' ' == *ptr) || ('\t' == *ptr))
            ptr++;        
        if (('"' == *ptr) && (c = strchr(++ptr, '"')))
        {
            *c++ = 0;
            argv[this->argc++] = ptr;
            ptr = c;
        }
        else if ((c = strpbrk(ptr, N_WHITESPACE)))
        {
            *c++ = 0;
            argv[this->argc++] = ptr;
            ptr = c;
        }
        else
        {
            argv[this->argc++] = ptr;
            break;
        }
    }

    // invoke parent constructor
    this->Initialize(this->argc, this->argv);
}
//------------------------------------------------------------------------------
#endif


