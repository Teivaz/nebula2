#ifndef N_VARIABLESERVER_H
#define N_VARIABLESERVER_H
//------------------------------------------------------------------------------
/**
    @class nVariableServer

    The variable server implements a registry for named, typed variables,
    and offers variable context object. The name registry guarantees a
    systemwide consistent mapping between variable names and handles.
    Variables are usually accessed by through their handle, as this is
    much faster.

    See also @ref N2ScriptInterface_nvariableserver

    (C) 2002 RadonLabs GmbH
*/
#ifndef N_ROOT_H
#include "kernel/nroot.h"
#endif

#ifndef N_ARRAY_H
#include "util/narray.h"
#endif

#ifndef N_VARIABLE_H
#include "variable/nvariable.h"
#endif

#ifndef N_VARIABLECONTEXT_H
#include "variable/nvariablecontext.h"
#endif

#ifndef N_VECTOR_H
#include "mathlib/vector.h"
#endif

//------------------------------------------------------------------------------
class nVariableContext;

class nVariableServer : public nRoot
{
public:
    /// constructor
    nVariableServer();
    /// destructor
    virtual ~nVariableServer();
    /// declare a variable
    nVariable::Handle DeclareVariable(const char* varName, nFourCC fourcc);
    /// get number of variable declarations
    int GetNumVariables() const;
    /// get variable declaration attributes at given index
    void GetVariableAt(int index, const char*& varName, nFourCC& varFourCC);
    /// get a variable handle by name, if variable undeclared, create it
    nVariable::Handle GetVariableHandleByName(const char* varName);
    /// get a variable handle by fourcc, if variable undeclared, create it
    nVariable::Handle GetVariableHandleByFourCC(nFourCC fourcc);
    /// find variable handle by name
    nVariable::Handle FindVariableHandleByName(const char* varName);
    /// find variable handle by fourcc code
    nVariable::Handle FindVariableHandleByFourCC(nFourCC fourcc);
    /// get the name of a variable from its handle
    const char* GetVariableName(nVariable::Handle varHandle);
    /// get the fourcc code of a variable from its handle
    nFourCC GetVariableFourCC(nVariable::Handle varHandle);

    /// get the global variable context
    const nVariableContext& GetGlobalVariableContext() const;
    /// set the float value of a global variable
    void SetFloatVariable(nVariable::Handle varHandle, float v);
    /// set the vector4 value of a global variable
    void SetVectorVariable(nVariable::Handle varHandle, const float4& v);
    /// set the integer value of a global variable
    void SetIntVariable(nVariable::Handle varHandle, int i);
    /// set the string value of a global variable
    void SetStringVariable(nVariable::Handle varHandle, const char* s);

    /// convert a string to a fourcc code
    static nFourCC StringToFourCC(const char* str);
    /// convert a fourcc code to a string
    static const char* FourCCToString(nFourCC, char* buf, int bufSize);

    static nKernelServer* kernelServer;

private:
    class VariableDeclaration
    {
    public:
        /// default constructor
        VariableDeclaration();
        /// constructor
        VariableDeclaration(const char* name, nFourCC fourcc);
        /// constructor with name only (fourcc will be invalid)
        VariableDeclaration(const char* name);
        /// constructor with fourcc only (name will be invalid)
        VariableDeclaration(nFourCC fourcc);
        /// get name
        const char* GetName() const;
        /// get fourcc code
        nFourCC GetFourCC() const;
        /// check if variable name is valid
        bool IsNameValid() const;
        /// check if variable fourcc is valid
        bool IsFourCCValid() const;

        nString name;
        nFourCC fourcc;
    };

    nVariableContext globalVariableContext;
    nArray<VariableDeclaration> registry;
};

//------------------------------------------------------------------------------
/*
*/
inline
nVariableServer::VariableDeclaration::VariableDeclaration() :
    fourcc(0)
{
    // empty
}

//------------------------------------------------------------------------------
/*
*/
inline
nVariableServer::VariableDeclaration::VariableDeclaration(const char* n) :
    name(n),
    fourcc(0)
{
    // empty
}

//------------------------------------------------------------------------------
/*
*/
inline
nVariableServer::VariableDeclaration::VariableDeclaration(nFourCC fcc) :
    fourcc(fcc)
{
    // empty
}

//------------------------------------------------------------------------------
/*
*/
inline
nVariableServer::VariableDeclaration::VariableDeclaration(const char* n, nFourCC fcc) :
    name(n),
    fourcc(fcc)
{
    // empty
}

//------------------------------------------------------------------------------
/*
*/
inline
const char*
nVariableServer::VariableDeclaration::GetName() const
{
    return this->name.IsEmpty() ? 0 : this->name.Get();
}

//------------------------------------------------------------------------------
/*
*/
inline
nFourCC
nVariableServer::VariableDeclaration::GetFourCC() const
{
    return this->fourcc;
}

//------------------------------------------------------------------------------
/*
*/
inline
bool
nVariableServer::VariableDeclaration::IsNameValid() const
{
    return !(this->name.IsEmpty());
}

//------------------------------------------------------------------------------
/*
*/
inline
bool
nVariableServer::VariableDeclaration::IsFourCCValid() const
{
    return (0 != this->fourcc);
}

//------------------------------------------------------------------------------
/*
    Converts a string to a fourcc code.
*/
inline
nFourCC
nVariableServer::StringToFourCC(const char* str)
{
    n_assert(str);

    // create a valid FourCC even if the string is not,
    // fill invalid stuff with spaces
    char c[4] = {' ',' ',' ',' '};
    int i;
    for (i = 0; i < 4; i++)
    {
        if (0 == str[i])
        {
            break;
        }
        else if (isalnum(str[i]))
        {
            c[i] = str[i];
        }
    }
    return MAKE_FOURCC(c[0], c[1], c[2], c[3]);
}

//------------------------------------------------------------------------------
/*
    Convertes a fourcc code to a string.
*/
inline
const char*
nVariableServer::FourCCToString(nFourCC fourcc, char* buf, int bufSize)
{
    n_assert(bufSize >= 5);
    buf[0] = (fourcc)     & 0xff;
    buf[1] = (fourcc>>8)  & 0xff;
    buf[2] = (fourcc>>16) & 0xff;
    buf[3] = (fourcc>>24) & 0xff;
    buf[4] = 0;
    return buf;
}

//------------------------------------------------------------------------------
#endif
