#ifndef N_CMD_H
#define N_CMD_H
//------------------------------------------------------------------------------
/**
    @class nCmd
    @ingroup NebulaObjectSystem
    @ingroup NebulaScriptServices

    Encapsulates a function call into a C++ object.

    (C) 2002 RadonLabs GmbH
*/
#include "kernel/ntypes.h"
#include "kernel/narg.h"
#include "kernel/ncmdproto.h"

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
    nFourCC GetId() const;
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

    /// Set input arguments in nCmd from a C variable argument list
    void CopyInArgsFrom(va_list marker);
    /// Copy input arguments from another nCmd
    void CopyInArgsFrom(nCmd * cmd);
    /// Retrieves index of the in-arg.
    int GetInArgIndex() const;

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
        this->args[i] = rhs.args[i];
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
nFourCC
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
inline
void
nCmd::CopyInArgsFrom(va_list marker)
{
    va_list markerCopy;
    va_copy(markerCopy, marker);
    for (int i = 0; i < this->GetNumInArgs(); i++)
    {
        nArg * arg = this->In();
        arg->Copy(&markerCopy);
    }
}

//------------------------------------------------------------------------------
inline
void
nCmd::CopyInArgsFrom(nCmd * cmd)
{
    n_assert(cmd);
    n_assert(cmd->GetNumInArgs() == this->cmdProto->GetNumInArgs());

    // rewind both commands
    cmd->Rewind();
    this->Rewind();

    // copy the arguments from the cmd provided
    for (int i = 0; i < cmd->GetNumInArgs(); i++)
    {
        nArg * argSrc = cmd->In();
        nArg * argDst = this->In();
        n_assert(argSrc->GetType() == argDst->GetType());
        argDst->Copy(*argSrc);
    }
}

//------------------------------------------------------------------------------
/**
    Retrieves index of the in-arg.

    -23-Dec-04 added by kims needs for nmaxscript module when it generate
                             custom attribute of a function in-arg.
*/
inline
int
nCmd::GetInArgIndex() const
{
    return this->inArgIndex;
}
//------------------------------------------------------------------------------
#endif
