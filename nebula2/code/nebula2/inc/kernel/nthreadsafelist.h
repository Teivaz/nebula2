#ifndef N_THREADSAFELIST_H
#define N_THREADSAFELIST_H
//------------------------------------------------------------------------------
/**
    A thread safe doubly linked list.

    Offers method to manipulate lists in a thread safe way,
    so that the list can be used as a communication point between
    threads.

    (C) 2002 RadonLabs GmbH
*/
#ifndef N_LIST_H
#include "util/nlist.h"
#endif

#ifndef N_MUTEX_H
#include "kernel/nmutex.h"
#endif

#ifndef N_EVENT_H
#include "kernel/nevent.h"
#endif

//------------------------------------------------------------------------------
class nThreadSafeList : public nList 
{
public:
    /// signal event object
    void SignalEvent();
    /// wait for event to become signaled
    void WaitEvent();
    /// wait for event to become signaled with timeout
    void TimedWaitEvent(float sec);
    /// gain access to list
    void Lock();
    /// give up access to list
    void Unlock();

private:
    nMutex mutex;
    nEvent event;
};

//------------------------------------------------------------------------------
/**
*/
inline 
void 
nThreadSafeList::SignalEvent()
{
    this->event.Signal();
}

//------------------------------------------------------------------------------
/**
*/
inline 
void 
nThreadSafeList::WaitEvent()
{
    this->event.Wait();
}

//------------------------------------------------------------------------------
/**
*/
inline 
void 
nThreadSafeList::Lock()
{
    this->mutex.Lock();
}

//------------------------------------------------------------------------------
/**
*/
inline 
void 
nThreadSafeList::Unlock()
{
    this->mutex.Unlock();
}

//------------------------------------------------------------------------------
#endif
