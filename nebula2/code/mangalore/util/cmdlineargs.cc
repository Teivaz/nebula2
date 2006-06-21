//------------------------------------------------------------------------------
//  util/cmdlineargs.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "util/cmdlineargs.h"

namespace Util
{

//------------------------------------------------------------------------------
/**
*/
CmdLineArgs::CmdLineArgs(const nString& l) :
    cmdLine(l)
{
    this->Parse();
}

//------------------------------------------------------------------------------
/**
    This parses the cmd line string.
*/
void
CmdLineArgs::Parse()
{
    nArray<nString> tokens;
    this->cmdLine.Tokenize(" \t\n=", '"', tokens);
    
    // number of tokens must be odd (1 command name,  plus N name/value pairs)
    if (1 != (tokens.Size() & 1))
    {
        n_error("CmdLineArgs::Parse(): invalid command line %s\n(must be command name, followed by name/value pairs!", cmdLine.Get());
    }
    else
    {
        this->cmdName = tokens[0];
        int i;
        int num = tokens.Size() - 1;
        for (i = 1; i < num; i += 2)
        {
            this->argNames.Append(tokens[i]);
            this->argValues.Append(tokens[i + 1]);
        }
    }
}

//------------------------------------------------------------------------------
/**
    Put out a missing argument error.
*/
void
CmdLineArgs::Error(const nString& argName) const
{
    n_error("CmdLine: arg '%s' not found in line '%s'!", argName.Get(), this->cmdLine.Get());
}

//------------------------------------------------------------------------------
/**
    Returns the command name.
*/
const nString&
CmdLineArgs::GetCommandName() const
{
    return this->cmdName;
}

//------------------------------------------------------------------------------
/**
    Returns true if argument exists.
*/
bool
CmdLineArgs::HasArg(const nString& name) const
{
    return (-1 != this->argNames.FindIndex(name));
}

//------------------------------------------------------------------------------
/**
    Returns index of argument. Throws an error if argument not found.
*/
int
CmdLineArgs::FindArgIndex(const nString& name) const
{
    n_assert(name.IsValid());
    int i = this->argNames.FindIndex(name);
    if (-1 == i)
    {
        this->Error(name);
    }
    return i;
}

//------------------------------------------------------------------------------
/**
*/
nString
CmdLineArgs::GetString(const nString& name) const
{
    return this->argValues[this->FindArgIndex(name)];
}

//------------------------------------------------------------------------------
/**
*/
int
CmdLineArgs::GetInt(const nString& name) const
{
    return this->argValues[this->FindArgIndex(name)].AsInt();
}

//------------------------------------------------------------------------------
/**
*/
float
CmdLineArgs::GetFloat(const nString& name) const
{
    return this->argValues[this->FindArgIndex(name)].AsFloat();
}

//------------------------------------------------------------------------------
/**
*/
bool
CmdLineArgs::GetBool(const nString& name) const
{
    return this->argValues[this->FindArgIndex(name)].AsBool();
}

//------------------------------------------------------------------------------
/**
*/
vector3
CmdLineArgs::GetVector3(const nString& name) const
{
    return this->argValues[this->FindArgIndex(name)].AsVector3();
}

//------------------------------------------------------------------------------
/**
*/
vector4
CmdLineArgs::GetVector4(const nString& name) const
{
    return this->argValues[this->FindArgIndex(name)].AsVector4();
}

//------------------------------------------------------------------------------
/**
*/
matrix44
CmdLineArgs::GetMatrix44(const nString& name) const
{
    return this->argValues[this->FindArgIndex(name)].AsMatrix44();
}

} // namespace Util
