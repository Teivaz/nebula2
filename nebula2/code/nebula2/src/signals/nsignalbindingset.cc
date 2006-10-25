//------------------------------------------------------------------------------
/**
    @file nsignalbindingset.cc

    (c) 2004 Tragnarion Studios
*/
//------------------------------------------------------------------------------
#include "signals/nsignalbindingset.h"

//------------------------------------------------------------------------------
bool
nSignalBindingSet::RemoveBinding(nSignalBinding * binding)
{
    if (!this->HasBinding(binding))
    {
        return false;
    }

    n_delete(binding);
    return true;
}

//------------------------------------------------------------------------------
bool
nSignalBindingSet::RemoveBinding(const nObject * objectPtr)
{
    bool found = false;
    nSignalBinding * binding = this->GetHead();
    while (binding != 0)
    {
        nSignalBinding * tmp = binding->GetSucc();
        if (binding->IsBoundWithObject(objectPtr))
        {
            n_delete(binding);
            found = true;
        }
        binding = tmp;
    }

    return found;
}

//------------------------------------------------------------------------------
bool
nSignalBindingSet::RemoveBinding(const nObject * objectPtr, const nCmdProto * cmdPtr)
{
    bool found = false;

    nSignalBinding * binding = this->GetHead();
    while (binding != 0)
    {
        nSignalBinding * tmp = binding->GetSucc();
        if (binding->IsBoundWithObject(objectPtr) && binding->IsBoundWithCmdProto(cmdPtr))
        {
            n_delete(binding);
            found = true;
        }
        binding = tmp;
    }

    return found;
}

//------------------------------------------------------------------------------
bool
nSignalBindingSet::RemoveBinding(const nObject * objectPtr, nFourCC fcc)
{
    bool found = false;

    nSignalBinding * binding = this->GetHead();
    while (binding != 0)
    {
        nSignalBinding * tmp = binding->GetSucc();
        if (binding->IsBoundWithObject(objectPtr) && binding->IsBoundWithCmdFourCC(fcc))
        {
            n_delete(binding);
            found = true;
        }
        binding = tmp;
    }

    return found;
}

//------------------------------------------------------------------------------
bool
nSignalBindingSet::RemoveBinding(const nObject * objectPtr, const char * name)
{
    bool found = false;

    nSignalBinding * binding = this->GetHead();
    while (binding != 0)
    {
        nSignalBinding * tmp = binding->GetSucc();
        if (binding->IsBoundWithObject(objectPtr) && binding->IsBoundWithCmdName(name))
        {
            n_delete(binding);
            found = true;
        }
        binding = tmp;
    }

    return found;
}

//------------------------------------------------------------------------------
void
nSignalBindingSet::RemoveAllBindings()
{
    nSignalBinding * binding;
    while (0 != (binding = static_cast<nSignalBinding *> (this->bindings.RemHead())))
    {
        n_delete(binding);
    }
}

//------------------------------------------------------------------------------
bool
nSignalBindingSet::HasBinding(const nSignalBinding * binding) const
{
    nSignalBinding * item = this->GetHead();
    while (0 != item)
    {
        if (item == binding)
        {
            return true;
        }
        item = binding->GetSucc();
    }

    return false;
}

//------------------------------------------------------------------------------
//  EOF
//------------------------------------------------------------------------------
