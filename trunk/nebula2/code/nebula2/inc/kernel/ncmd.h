#ifndef N_CMD_H
#define N_CMD_H
//------------------------------------------------------------------------------
/**
    Encapsulate a function call into a C++ object. 
    
    (C) 2002 RadonLabs GmbH
*/
#ifndef N_TYPES_H
#include "kernel/ntypes.h"
#endif

#ifndef N_ARG_H
#include "kernel/narg.h"
#endif

#ifndef N_CMDPROTO_H
#include "kernel/ncmdproto.h"
#endif

//------------------------------------------------------------------------------
class nCmdProto;
class nCmd 
{
public:
    /// constructor
    nCmd(nCmdProto* proto);
    /// copy constructor
    nCmd(const nCmd& rhs);
    /// get the cmd's prototype object
    nCmdProto* GetProto() const;
    /// get the cmd's fourcc code
    uint GetId() const;
    /// get number of input args
    int GetNumInArgs() const;
    /// get number of output args
    int GetNumOutArgs() const;
    /// get next input argument
    nArg* In();
    /// get next output argument
    nArg* Out();
    /// rewind internal next args
    void Rewind();

private:
    enum 
    {
        N_MAXNUM_ARGS = 16,
    };
    nCmdProto *cmdProto;           
    int outArgIndex;
    int inArgIndex;
    nArg args[N_MAXNUM_ARGS];   
};

//------------------------------------------------------------------------------
/**
*/
inline 
nCmd::nCmd(nCmdProto* proto)
{
    n_assert(proto);
    this->cmdProto    = proto;
    this->inArgIndex  = this->cmdProto->GetNumOutArgs();
    this->outArgIndex = 0;
}

//------------------------------------------------------------------------------
/**
*/
inline 
nCmd::nCmd(const nCmd& rhs)
{
    this->cmdProto    = rhs.cmdProto;
    this->inArgIndex  = this->cmdProto->GetNumOutArgs();
    this->outArgIndex = 0;
    int i;
    int numArgs = this->cmdProto->GetNumArgs();
    for (i = 0; i < numArgs; i++)
    {
        this->args[i].Set(rhs.args[i]);
    }
}

//------------------------------------------------------------------------------
/**
*/
inline 
int 
nCmd::GetNumInArgs() const
{
    return this->cmdProto->GetNumInArgs();
}

//------------------------------------------------------------------------------
/**
*/
inline 
int 
nCmd::GetNumOutArgs() const
{
    return this->cmdProto->GetNumOutArgs();
}

//------------------------------------------------------------------------------
/**
*/
inline 
nCmdProto*
nCmd::GetProto() const
{
    return this->cmdProto;
}

//------------------------------------------------------------------------------
/**
*/
inline 
uint 
nCmd::GetId() const
{ 
    return this->cmdProto->GetId();
}

//------------------------------------------------------------------------------
/**
*/
inline 
void 
nCmd::Rewind() 
{
    this->inArgIndex  = this->cmdProto->GetNumOutArgs();
    this->outArgIndex = 0;
}

//------------------------------------------------------------------------------
/**
*/
inline 
nArg*
nCmd::In()
{
    n_assert(this->inArgIndex < this->cmdProto->GetNumArgs());
    return &(this->args[this->inArgIndex++]);
}

//------------------------------------------------------------------------------
/**
*/
inline 
nArg*
nCmd::Out()
{
    n_assert(this->outArgIndex < this->GetNumOutArgs());
    return &(this->args[this->outArgIndex++]);
}

//------------------------------------------------------------------------------
#endif    
