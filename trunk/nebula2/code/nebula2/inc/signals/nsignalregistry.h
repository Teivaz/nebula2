#ifndef N_SIGNALREGISTRY_H
#define N_SIGNALREGISTRY_H
//------------------------------------------------------------------------------
/**
    @file nsignalregistry.h
    @class nSignalRegistry
    @ingroup NebulaSignals

    nSignalRegistry is the interface needed to add to nClass to handle signals.
    Not yet sure about how to merge it with it, rather moving this code into
    nClass or just use multiple inheritance.

    Much as nClass currently tracks the commands that a class supports, it will
    also come to track the signals that it may emit.  The signal setup could
    probably happen within the existing implementations of n_initcmds functions
    Perhaps we should consider renaming this function to n_initclass?

    (C) 2004 Tragnarion Studios
*/
//------------------------------------------------------------------------------
#include "util/nhashlist.h"
#include "kernel/ntypes.h"

//------------------------------------------------------------------------------
class nSignal;

//------------------------------------------------------------------------------
class nSignalRegistry
{
public:
    /// constructor
    nSignalRegistry();
    /// destructor
    ~nSignalRegistry();

    /// start defining signals
    void BeginSignals(int numSignals);
    /// add a signal object already created
    bool AddSignal(nSignal * signal);
    /// add a signal object already created
    bool AddSignal(nSignal & signal);
    /// add a signal to the signal registry
    bool AddSignal(const char * proto_def, nFourCC id);
    /// finish defining signals
    void EndSignals();
    /// find a signal by name
    nSignal * FindSignalByName(const char * name);
    /// find a signal by fourcc code
    nSignal * FindSignalById(nFourCC id);

    /// get number of signals
    int GetNumSignals() const;

private:
    nHashList * signalList;
    int numSignals;
};

//------------------------------------------------------------------------------
/**
*/
inline
nSignalRegistry::nSignalRegistry() :
    signalList(NULL),
    numSignals(0)
{
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nSignalRegistry::GetNumSignals() const
{
    return this->numSignals;
}

//------------------------------------------------------------------------------
/**
    Added mainly for native signals
*/
inline
bool
nSignalRegistry::AddSignal(nSignal & signal)
{
    return this->AddSignal(&signal);
}

//------------------------------------------------------------------------------
#endif
