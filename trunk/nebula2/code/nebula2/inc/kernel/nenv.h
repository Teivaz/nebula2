#ifndef N_ENV_H
#define N_ENV_H
//------------------------------------------------------------------------------
/**
    @class nEnv

    A typed value in a named object. Technically, a nArg object
    wrapped into a nRoot.
    
    (C) 2002 RadonLabs GmbH
*/
#ifndef N_TYPES_H
#include "kernel/ntypes.h"
#endif

#ifndef N_ARG_H
#include "kernel/narg.h"
#endif

#ifndef N_ROOT_H
#include "kernel/nroot.h"
#endif

#undef N_DEFINES
#define N_DEFINES nEnv
#include "kernel/ndefdllclass.h"

//------------------------------------------------------------------------------
class N_PUBLIC nEnv : public nRoot 
{
public:
	/// object persistency
    virtual bool SaveCmds(nPersistServer *);
	/// Returns the type of this nEnv instance
    nArg::ArgType GetType(void);
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

    static nKernelServer* kernelServer;

private:
    nArg arg;
};

//------------------------------------------------------------------------------
/**
*/
inline
nArg::ArgType 
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
int 
nEnv::GetI(void)
{
    return this->arg.GetI();
}

//------------------------------------------------------------------------------
/**
*/
inline
bool 
nEnv::GetB(void)
{
    return this->arg.GetB();
}

//------------------------------------------------------------------------------
/**
*/
inline
float 
nEnv::GetF(void)
{
    return this->arg.GetF();
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nEnv::GetS(void)
{
    return this->arg.GetS();
}

//------------------------------------------------------------------------------
/**
*/
inline
nRoot*
nEnv::GetO(void)
{
    return (nRoot *) this->arg.GetO();
}

//------------------------------------------------------------------------------
#endif      
