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
    /// set a vector4 (must be initialized with F4)
    void SetV4(const vector4& v);
    /// Returns the integer value
    int GetI(void);
    /// Returns the boolean value
    bool GetB(void);
    /// Returns the float value
    float GetF(void);
    /// Returns the string
    const char *GetS(void);
    /// Returns the object
    nRoot *GetO(void);
    /// get float4 value
    const nFloat4& GetF4();

    static nKernelServer* kernelServer;

private:
    nArg arg;
};

//------------------------------------------------------------------------------
/**
*/
inline
nArg::Type 
nEnv::GetType(void)
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
    nFloat4 f4 = { v4.x, v4.y, v4.z, v4.w };
    this->arg.SetF4(f4);
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
#endif      
