#ifndef N_ENV_H
#define N_ENV_H
//------------------------------------------------------------------------------
/**
    @class nEnv

    A typed value in a named object. Technically, a nArg object
    wrapped into a nRoot.

    See also @ref N2ScriptInterface_nenv

    (C) 2002 RadonLabs GmbH
*/
#include "kernel/ntypes.h"
#include "kernel/narg.h"
#include "kernel/nroot.h"

//------------------------------------------------------------------------------
class nEnv : public nRoot
{
public:
    /// object persistency
    virtual bool SaveCmds(nPersistServer *);
    /// Returns the type of this nEnv instance
    nArg::Type GetType();
    /// Sets the value of this object to the passed integer
    void SetI(int i);
    /// Sets the value of this object to the passes boolean value
    void SetB(bool b);
    /// Sets the value of this object to the passed float value
    void SetF(float f);
    /// Sets the value of this object to the passed string
    void SetS(const char *s);
    /// Sets the value of this objet to the passed object
    void SetO(nRoot *o);
    /// set float4 value
    void SetF4(const nFloat4& f4);
    /// set a vector4
    void SetV4(const vector4& v);
    /// set a vector3
    void SetV3(const vector3& v);
    /// Returns the integer value
    int GetI();
    /// Returns the boolean value
    bool GetB();
    /// Returns the float value
    float GetF();
    /// Returns the string
    const char *GetS();
    /// Returns the object
    nRoot *GetO();
    /// get float4 value
    const nFloat4& GetF4();
    /// get vector4 value
    const vector4& GetV4();
    /// get vector3 value
    const vector3& GetV3();

private:
    nArg arg;
};

//------------------------------------------------------------------------------
/**
*/
inline
nArg::Type
nEnv::GetType()
{
    return this->arg.GetType();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nEnv::SetI(int i)
{
    this->arg.SetI(i);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nEnv::SetB(bool b)
{
    this->arg.SetB(b);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nEnv::SetF(float f)
{
    this->arg.SetF(f);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nEnv::SetS(const char *s)
{
    this->arg.SetS(s);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nEnv::SetO(nRoot *o)
{
    this->arg.SetO(o);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nEnv::SetF4(const nFloat4& f4)
{
    this->arg.SetF4(f4);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nEnv::SetV4(const vector4& v4)
{
    this->arg.SetV4(v4);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nEnv::SetV3(const vector3& v3)
{
    this->arg.SetV3(v3);
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nEnv::GetI()
{
    return this->arg.GetI();
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nEnv::GetB()
{
    return this->arg.GetB();
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nEnv::GetF()
{
    return this->arg.GetF();
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nEnv::GetS()
{
    return this->arg.GetS();
}

//------------------------------------------------------------------------------
/**
*/
inline
nRoot*
nEnv::GetO()
{
    return (nRoot *) this->arg.GetO();
}

//------------------------------------------------------------------------------
/**
*/
inline
const nFloat4&
nEnv::GetF4()
{
    return this->arg.GetF4();
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector4&
nEnv::GetV4()
{
    return this->arg.GetV4();
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector3&
nEnv::GetV3()
{
    return this->arg.GetV3();
}

//------------------------------------------------------------------------------
#endif
