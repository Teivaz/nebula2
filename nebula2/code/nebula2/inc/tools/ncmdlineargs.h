#ifndef N_CMDLINEARGS_H
#define N_CMDLINEARGS_H
//------------------------------------------------------------------------------
/**
    @class nCmdLineArgs
    @ingroup Tools

    Helper class to extract arguments from a ANSI-C command line.

    (C) 2003 RadonLabs GmbH
*/
#include <stdlib.h>
#include <math.h>
#include <string.h>

//------------------------------------------------------------------------------
class nCmdLineArgs
{
public:
    /// default constructor
    nCmdLineArgs();
    /// constructor
    nCmdLineArgs(int argc, const char** argv);
    /// initialize (use if default constructor was used
    void Initialize(int argc, const char** argv);
    /// get a float arg
    float GetFloatArg(const char* option, float defaultValue);
    /// get an integer arg
    int GetIntArg(const char* option, int defaultValue);
    /// get a bool arg
    bool GetBoolArg(const char* option);
    /// get a string arg
    const char* GetStringArg(const char* option, const char* defaultValue);

private:
    /// find an argument index
    int FindArg(const char* option);

    int argCount;
    const char** argVector;
};

//------------------------------------------------------------------------------
/**
*/
inline
nCmdLineArgs::nCmdLineArgs() :
    argCount(0),
    argVector(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Call the destructor with the arguments of the main() function.
*/
inline
nCmdLineArgs::nCmdLineArgs(int argc, const char** argv)
{
    this->argCount = argc;
    this->argVector = argv;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nCmdLineArgs::Initialize(int argc, const char** argv)
{
    this->argCount = argc;
    this->argVector = argv;
}

//------------------------------------------------------------------------------
/**
    Find argument index for an option string. Returns 0 if option not
    found.
*/
inline
int
nCmdLineArgs::FindArg(const char* option)
{
    int i;
    for (i = 0; i < this->argCount; i++)
    {
        if (strcmp(this->argVector[i], option) == 0)
        {
            return i;
        }
    }
    // fallthrough: error
    return 0;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nCmdLineArgs::GetFloatArg(const char* option, float defaultValue)
{
    int i = this->FindArg(option);
    if (i == 0)
    {
        return defaultValue;
    }
    else if (++i < this->argCount)
    {
        return (float) atof(this->argVector[i]);
    }
    else
    {
        // unexpected end of command line, return default value
        return defaultValue;
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nCmdLineArgs::GetIntArg(const char* option, int defaultValue)
{
    int i = this->FindArg(option);
    if (i == 0)
    {
        return defaultValue;
    }
    else if (++i < this->argCount)
    {
        return atoi(this->argVector[i]);
    }
    else
    {
        // unexpected end of command line, return default value
        return defaultValue;
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nCmdLineArgs::GetBoolArg(const char* option)
{
    if (this->FindArg(option) > 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
const char* 
nCmdLineArgs::GetStringArg(const char* option, const char* defaultValue)
{
    int i = this->FindArg(option);
    if (i == 0)
    {
        return defaultValue;
    }
    else if (++i < this->argCount)
    {
        return this->argVector[i];
    }
    else
    {
        // unexpected end of command line, return default value
        return defaultValue;
    }
}

//------------------------------------------------------------------------------
#endif

