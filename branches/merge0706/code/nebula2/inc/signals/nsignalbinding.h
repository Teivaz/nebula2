#ifndef N_SIGNALBINDING_H
#define N_SIGNALBINDING_H
//------------------------------------------------------------------------------
/**
    @class nSignalBinding
    @ingroup NebulaSignals
    @brief Internal: nSignalBinding represents the callback to the signal
    receiver.

    An nSignalBinding is internal to the signal system.  A binding represents a
    callback that should be executed when a signal is emitted/triggered.
    These callbacks are conveniently (for Nebula) represented as nCmdProto
    instances.  This lets you take full advantage of the existing script
    interface infrastructure for calling into native code.  Also, for the script
    servers that support it (Lua and partially Python for now), you can have
    nCmdProto instances that will invoke code in a the scripting language.

    (C) 2004 Tragnarion Studios
*/
//------------------------------------------------------------------------------
#include "kernel/nobject.h"
#include "kernel/ntypes.h"
#include "util/nnode.h"

//------------------------------------------------------------------------------
class nSignalBindingSet;

class nSignalBinding : public nNode
{
public:
    /// constructor
    nSignalBinding(int priority);
    /// destructor
    virtual ~nSignalBinding();

    /** @name Iteration
        Methods for iterating over a signal binding set. */
    //@{
    /// get next signal binding in the binding set
    nSignalBinding * GetSucc () const;
    /// get previous signal binding in the binding set
    nSignalBinding * GetPred () const;
    //@}

    /** @name Invocation
        Methods for invoking a signal binding. */
    //@{
    /// Invoke the signal binding (cmd keeps the signal parameters values)
    virtual bool Invoke(nCmd * cmd) = 0;
    /// Invoke the signal with variable list arguments
    virtual bool Invoke(va_list args) = 0;
    //@}

    /// Return prototype string
    virtual const char * GetProtoDef() const = 0;

    /// Return true if binding bounds to the object provided
    virtual bool IsBoundWithObject(const nObject * objectPtr) const;
    /// Return true if binding bounds to a command with the cmdproto provided
    virtual bool IsBoundWithCmdProto(const nCmdProto * cmdProto) const;
    /// Return true if binding bounds to a command with the name provided
    virtual bool IsBoundWithCmdName(const char * name) const;
    /// Return true if binding bounds to a command with the fourcc provided
    virtual bool IsBoundWithCmdFourCC(nFourCC fourcc) const;
    /// Return true if the binding is valid
    virtual bool IsValid() const;

    /// Get priority of the binding
    int GetPriority() const;

protected:

    /**
        The priority of the binding in a binding set is stored in the binding.
        This is safe since a given binding is not referenced in more than once
        binding set.
    */
    int priority;
};

//------------------------------------------------------------------------------
inline
nSignalBinding::nSignalBinding(int priority) :
    priority(priority)
{
}

//------------------------------------------------------------------------------
inline
nSignalBinding::~nSignalBinding()
{
    if (this->IsLinked())
    {
        this->Remove();
    }
}

//------------------------------------------------------------------------------
inline
nSignalBinding *
nSignalBinding::GetSucc() const
{
    return static_cast<nSignalBinding *> (nNode::GetSucc());
}

//------------------------------------------------------------------------------
inline
nSignalBinding *
nSignalBinding::GetPred() const
{
    return static_cast<nSignalBinding *> (nNode::GetPred());
}

//------------------------------------------------------------------------------
inline
int
nSignalBinding::GetPriority() const
{
    return this->priority;
}

//------------------------------------------------------------------------------
/**
    @return true if binding bounds to the object provided
*/
inline
bool
nSignalBinding::IsBoundWithObject(const nObject * /*objectPtr*/) const
{
    return false;
}

//------------------------------------------------------------------------------
/**
    @return true if binding bounds to a command with the cmdproto provided
*/
inline
bool
nSignalBinding::IsBoundWithCmdProto(const nCmdProto * /*cmdProto*/) const
{
    return false;
}

//------------------------------------------------------------------------------
/**
    @return true if binding bounds to a command with the name provided
*/
inline
bool
nSignalBinding::IsBoundWithCmdName(const char * /*name*/) const
{
    return false;
}

//------------------------------------------------------------------------------
/**
    @return true if binding bounds to a command with the fourcc provided
*/
inline
bool
nSignalBinding::IsBoundWithCmdFourCC(nFourCC /*fourcc*/) const
{
    return false;
}

//------------------------------------------------------------------------------
inline
bool
nSignalBinding::IsValid() const
{
    return false;
}

//------------------------------------------------------------------------------
#endif
