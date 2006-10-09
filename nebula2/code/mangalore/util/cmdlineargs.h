#ifndef UTIL_CMDLINEARGS_H
#define UTIL_CMDLINEARGS_H
//------------------------------------------------------------------------------
/**
    @class Util::CmdLineArgs

    An universal cmd line argument parser. The command line string
    must have the form

    cmd arg0[=]value0 arg1[=]value1 arg2[=]value2

    (C) 2005 Radon Labs GmbH
*/
#include "util/nstring.h"

//------------------------------------------------------------------------------
namespace Util
{
class CmdLineArgs
{
public:
    /// constructor
    CmdLineArgs(const nString& cmdLine);
    /// get the original command line
    const nString& GetCmdLine() const;
    /// get the command name
    const nString& GetCmdName() const;
    /// return true if arg exists
    bool HasArg(const nString& arg) const;
    /// get string value
    nString GetString(const nString& name) const;
    /// get int value
    int GetInt(const nString& name) const;
    /// get float value
    float GetFloat(const nString& name) const;
    /// get bool value
    bool GetBool(const nString& name) const;
    /// get vector3 value
    vector3 GetVector3(const nString& name) const;
    /// get vector4 value
    vector4 GetVector4(const nString& name) const;
    /// get matrix44 value
    matrix44 GetMatrix44(const nString& name) const;

private:
    /// parse the command line string
    void Parse();
    /// put out an error
    void Error(const nString& argName) const;
    /// find index of argument, fails hard with error msg if not found
    int FindArgIndex(const nString& argName) const;

    nString cmdLine;
    nString cmdName;
    nArray<nString> argNames;
    nArray<nString> argValues;
};

} // namespace Util
//------------------------------------------------------------------------------
#endif
