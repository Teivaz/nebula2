//------------------------------------------------------------------------------
/**
    @file nsignalbindingcmdproto.cc

    (c) 2004 Tragnarion Studios
*/
//------------------------------------------------------------------------------
#include "signals/nsignalbindingcmdproto.h"
#include "kernel/ncmdproto.h"

//------------------------------------------------------------------------------
/**
    @return the command prototype rebinding if needed
*/
nCmdProto *
nSignalBindingCmdProto::GetCmdProto() const
{
    switch (this->rebind)
    {
    case DontRebind:
        return this->cmdProto;
        break;
    case RebindByFourCC:
        return this->refObject->GetClass()->FindCmdById(this->fourcc);
        break;
    case RebindByName:
        return this->refObject->GetClass()->FindCmdByName(this->cmdname->Get());
        break;
    default:
        n_assert(0);
        break;
    }

    return 0;
}

//------------------------------------------------------------------------------
/**
    More appropiate for calls from scripting or even for asynchronous calls.
*/
bool
nSignalBindingCmdProto::Invoke(nCmd * cmdArg)
{
    // rebind the command if required
    nCmdProto * cmdProto = this->GetCmdProto();
    if (!cmdProto)
    {
        // if cmdProto cannot be resolved, ignore the binding, but do not delete it
        return false;
    }

    nCmd * cmd = cmdProto->NewCmd();
    n_assert(cmd);
    cmd->CopyInArgsFrom(cmdArg);
    cmd->Rewind();
    // execute the bound command with the arguments provided
    cmdProto->Dispatch(this->refObject.get(), cmd);
    cmdProto->RelCmd(cmd);

    return false;
}

//------------------------------------------------------------------------------
bool
nSignalBindingCmdProto::Invoke(va_list args)
{
    nCmdProto * cmdProto = this->GetCmdProto();
    nCmd * cmd = cmdProto->NewCmd();
    n_assert(cmd);

    // set input command arguments
    cmd->CopyInArgsFrom(args);

    // command execution, forget result and release
    cmdProto->Dispatch(this->refObject.get(), cmd);
    cmdProto->RelCmd(cmd);
    /// XXX: the result is not returned when called with va_list

    return true;
}

//------------------------------------------------------------------------------
const char *
nSignalBindingCmdProto::GetProtoDef() const
{
    const nCmdProto * cmdProto = this->GetCmdProto();
    if (cmdProto)
    {
        return cmdProto->GetProtoDef();
    }

    return 0;
}

//------------------------------------------------------------------------------
bool
nSignalBindingCmdProto::IsBoundWithObject(const nObject * objectPtr) const
{
    n_assert(objectPtr);
    return (this->refObject.get() == objectPtr);
}

//------------------------------------------------------------------------------
bool
nSignalBindingCmdProto::IsBoundWithCmdProto(const nCmdProto * cmdp) const
{
    n_assert(cmdp);
    const nCmdProto * cmdProto = this->GetCmdProto();
    return (cmdProto == cmdp);
}

//------------------------------------------------------------------------------
bool
nSignalBindingCmdProto::IsBoundWithCmdName(const char * name) const
{
    n_assert(name);
    const nCmdProto * cmdProto = this->GetCmdProto();
    return (!strcmp(cmdProto->GetName(), name));
}

//------------------------------------------------------------------------------
bool
nSignalBindingCmdProto::IsBoundWithCmdFourCC(nFourCC fourcc) const
{
    const nCmdProto * cmdProto = this->GetCmdProto();
    return (cmdProto->GetId() == fourcc);
}

//------------------------------------------------------------------------------
