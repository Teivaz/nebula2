#ifndef N_SIGNALBINDINGSET_H
#define N_SIGNALBINDINGSET_H
//------------------------------------------------------------------------------
/**
    @class nSignalBindingSet
    @ingroup NebulaSignals
    @brief nSignalBindingSet is a container of bindings.

    nSignalBindingSet is just a container of signal bindings, it has no logic
    about signal emission or binding invocation. It keeps a list of signal
    bindings sorted by priority.

    (C) 2004 Tragnarion Studios
*/
//------------------------------------------------------------------------------
#include "util/nlist.h"
#include "util/nhashnode.h"
#include "signals/nsignalbinding.h"

//------------------------------------------------------------------------------
class nSignal;

class nSignalBindingSet : public nHashNode
{
public:
    /// constructor
    nSignalBindingSet();
    /// destructor
    virtual ~nSignalBindingSet();

    /** @name Management
        Methods for managing the bindings in a signal binding set. */
    //@{
    /// add a binding to a binding set
    bool AddBinding(nSignalBinding * binding);
    /// remove a binding
    bool RemoveBinding(nSignalBinding * binding);
    /// remove all bindings to the object provided
    bool RemoveBinding(const nObject * objectPtr);
    /// remove all bindings for object and command
    bool RemoveBinding(const nObject * objectPtr, const nCmdProto * cmdPtr);
    /// remove all bindings for object and fourcc
    bool RemoveBinding(const nObject * objectPtr, nFourCC fcc);
    /// remove all bindings for object and command name
    bool RemoveBinding(const nObject * objectPtr, const char * name);
    /// remove all bindings
    void RemoveAllBindings();
    /// returns true if signal binding found in list
    bool HasBinding(const nSignalBinding * binding) const;
    //@}

    /** @name Iteration
        Methods for iterating over the bindings in a signal binding set. */
    //@{
    /// get first binding
    nSignalBinding * GetHead() const;
    /// get last binding
    nSignalBinding * GetTail() const;
    //@}

private:
    nList bindings;
};

//------------------------------------------------------------------------------
inline
nSignalBindingSet::nSignalBindingSet() :
    nHashNode(0)
{
    /// empty
}

//------------------------------------------------------------------------------
inline
nSignalBindingSet::~nSignalBindingSet()
{
    this->RemoveAllBindings();
}

//------------------------------------------------------------------------------
inline
bool
nSignalBindingSet::AddBinding(nSignalBinding * binding)
{
    n_assert(binding);

    int priority = binding->GetPriority();

    // find the proper insertion point (signals ordered by priority)
    nSignalBinding * node = static_cast<nSignalBinding *> (this->bindings.GetHead());
    while (node && node->GetPriority() < priority)
    {
        node = static_cast<nSignalBinding *> (node->GetSucc());
    }

    if (node)
    {
        binding->InsertBefore(node);
    }
    else
    {
        this->bindings.AddTail(static_cast<nNode *> (binding));
    }

    return true;
}

//------------------------------------------------------------------------------
inline
nSignalBinding *
nSignalBindingSet::GetHead() const
{
    return static_cast<nSignalBinding *> (this->bindings.GetHead());
}

//------------------------------------------------------------------------------
inline
nSignalBinding *
nSignalBindingSet::GetTail() const
{
    return static_cast<nSignalBinding *> (this->bindings.GetTail());
}

//------------------------------------------------------------------------------
#endif
