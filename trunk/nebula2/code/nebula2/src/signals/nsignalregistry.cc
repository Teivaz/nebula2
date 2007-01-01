//------------------------------------------------------------------------------
/**
    @file nsignalregistry.cc

    (c) 2004 Tragnarion Studios
*/
//------------------------------------------------------------------------------
#include "kernel/nclass.h"
#include "signals/nsignalregistry.h"
#include "signals/nsignal.h"

//------------------------------------------------------------------------------
/**
*/
nSignalRegistry::~nSignalRegistry()
{
    if (this->signalList)
    {
        nSignal* signal;
        while (0 != (signal = static_cast<nSignal*>(this->signalList->RemHead())))
        {
            // Native signals are statically allocated while
            // signals defined from scripting are dynamically allocated
            signal->Release();
        }
        n_delete(this->signalList);
        this->signalList = 0;
        this->numSignals = 0;
    }
}

//------------------------------------------------------------------------------
/**
    Begin the definition of signals
*/
void
nSignalRegistry::BeginSignals(int numSignals)
{
    n_assert(!this->signalList);
    this->signalList = n_new(nHashList(numSignals));
}

//------------------------------------------------------------------------------
/**
    AddSignal adds the signal provided as parameter <tt>signal</tt> to the
    internal list of signals supported by the class.

     - This will error when the signal already exists
     - This will error if the nFourCC isn't unique for that class
       and its ancestors.
*/
bool
nSignalRegistry::AddSignal(nSignal* signal)
{
    n_assert(signal);

    if (this->FindSignalByName(signal->GetName()))
    {
        return false;
    }

    if (this->FindSignalById(signal->GetId()))
    {
        return false;
    }

    this->signalList->AddTail(signal);
    this->numSignals++;

    return true;
}

//------------------------------------------------------------------------------
/**
    AddSignal creates a new signal with the parameters provided

    @param  proto_def   the signal prototype definition
    @param  id          the signal unique nFourCC code
    @returns true if the signal was added successfully, otherwise false

     - This will error when the signal already exists
     - This will error if the nFourCC isn't unique for that class and its
       ancestors.
     - This will error if the type signature is not valid.
*/
bool
nSignalRegistry::AddSignal(const char* proto_def, nFourCC id)
{
    n_assert(proto_def);
    n_assert(id);

    // create the signal list to allow addition of signals from scripting
    // even if no begin signals was issued
    if (!this->signalList)
    {
        if (numSignals > 0)
        {
            this->signalList = n_new(nHashList(numSignals));
        }
        else
        {
            // create the hash list with minimum space
            this->signalList = n_new(nHashList(5));
        }
    }

    // type signature is checked on creation of nSignal (assert on error)
    ProtoDefInfo info(proto_def);
    if (!info.valid)
    {
        return false;
    }

    // check the signal name does not already exist
    if (this->FindSignalByName(info.name))
    {
        return false;
    }

    // check the signal id does not already exist
    if (this->FindSignalById(id))
    {
        return false;
    }

    nSignal* signal = n_new(nSignal(info.name, proto_def, id));
    this->AddSignal(signal);

    return true;
}

//------------------------------------------------------------------------------
/**
    End definition of signals
*/
void
nSignalRegistry::EndSignals()
{
    /// empty
}

//------------------------------------------------------------------------------
/**
    Find signal by name
    @return the signal object when found, otherwise NULL
*/
nSignal*
nSignalRegistry::FindSignalByName(const char* name)
{
    n_assert(name);

    if (this->signalList)
    {
        nHashNode* node = this->signalList->GetHead();
        while (node)
        {
            if (!strcmp(node->GetName(), name))
            {
                return static_cast<nSignal*>(node);
            }
            node = node->GetSucc();
        }
    }

    /// this only works when this is nClass (which mixes-in nSignalRegistry)
    nClass* theClass = static_cast<nClass*>(this);
    nClass* superClass = theClass->GetSuperClass();
    if (superClass)
    {
        return superClass->FindSignalByName(name);
    }

    return NULL;
}

//------------------------------------------------------------------------------
/**
    Find signal by nFourCC identifier.

    @return the signal object when found, otherwise NULL
*/
nSignal*
nSignalRegistry::FindSignalById(nFourCC id)
{
    if (this->signalList)
    {
        nHashNode* node = this->signalList->GetHead();
        while (node)
        {
            if ((static_cast<nSignal*>(node))->GetId() == id)
            {
                return static_cast<nSignal*>(node);
            }
            node = node->GetSucc();
        }
    }

    /// this only works when this is nClass (which mixes-in nSignalRegistry)
    nClass* theClass = static_cast<nClass*>(this);
    nClass* superClass = theClass->GetSuperClass();
    if (superClass)
    {
        return superClass->FindSignalById(id);
    }

    return NULL;
}

//------------------------------------------------------------------------------
//  EOF
//------------------------------------------------------------------------------
