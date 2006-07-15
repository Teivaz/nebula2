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
#include "util/nstring.h"

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
    float GetFloatArg(const nString& option, float defaultValue = 0.0f) const;
    /// get an integer arg
    int GetIntArg(const nString& option, int defaultValue = 0) const;
    /// get a bool arg
    bool GetBoolArg(const nString& option) const;
    /// get a string arg
    nString GetStringArg(const nString& option, const nString& defaultValue = 0) const;
    /// return true if argument exists
    bool HasArg(const nString& option) const;

private:
    /// find an argument index
    int FindArg(const nString& option) const;

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
nCmdLineArgs::FindArg(const nString& option) const
{
    int i;
    for (i = 0; i < this->argCount; i++)
    {
        if (strcmp(this->argVector[i], option.Get()) == 0)
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
bool
nCmdLineArgs::HasArg(const nString& option) const
{
    return this->FindArg(option) != 0;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nCmdLineArgs::GetFloatArg(const nString& option, float defaultValue) const
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
nCmdLineArgs::GetIntArg(const nString& option, int defaultValue) const
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
nCmdLineArgs::GetBoolArg(const nString& option) const
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
nString
nCmdLineArgs::GetStringArg(const nString& option, const nString& defaultValue) const
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

