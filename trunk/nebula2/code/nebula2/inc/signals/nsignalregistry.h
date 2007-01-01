#ifndef N_SIGNALREGISTRY_H
#define N_SIGNALREGISTRY_H
//------------------------------------------------------------------------------
/**
    @class nSignalRegistry
    @ingroup NebulaSignals
    @brief nSignalRegistry is the interface for adding signals to an nClass.

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

    /** @name Management
        Methods for managing signals on an nClass. */
    //@{
    /// start defining signals
    void BeginSignals(int numSignals);
    /// add a signal object already created
    bool AddSignal(nSignal* signal);
    /// add a signal object already created
    bool AddSignal(nSignal& signal);
    /// add a signal to the signal registry
    bool AddSignal(const char* proto_def, nFourCC id);
    /// finish defining signals
    void EndSignals();
    /// find a signal by name
    nSignal* FindSignalByName(const char* name);
    /// find a signal by fourcc code
    nSignal* FindSignalById(nFourCC id);
    /// Get list of signals
    nHashList* GetSignalList() const;
    //@}

    /// get number of signals
    int GetNumSignals() const;

private:
    nHashList* signalList;
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
/**
    @return the list of signals
*/
inline
nHashList*
nSignalRegistry::GetSignalList() const
{
    return this->signalList;
}

//------------------------------------------------------------------------------
#endif
