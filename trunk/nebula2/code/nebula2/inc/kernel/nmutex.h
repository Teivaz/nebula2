#ifndef N_MUTEX_H
#define N_MUTEX_H
//------------------------------------------------------------------------------
/**
    Implements a simple mutex object for thread synchronization.
    Win32: win32 mutex handles (CreateMutex())
    Linux: posix thread mutexes

    (C) 2002 RadonLabs GmbH
*/
#ifndef N_TYPES_H
#include "kernel/ntypes.h"
#endif

#ifndef __NEBULA_NO_THREADS__
#   if __XBxX__
#       include "xbox/nxbwrapper.h"
#   elif __WIN32__
#       ifndef _INC_WINDOWS
#       include <windows.h>
#       endif
#       ifndef _INC_PROCESS
#       include <process.h>
#       endif
#   else
#   include <pthread.h>
#   endif
#endif

//------------------------------------------------------------------------------
class nMutex 
{
public:
    /// constructor
    nMutex();
    /// destructor
    ~nMutex();
    /// lock mutex
    void Lock();
    /// unlock mutex
    void Unlock();

private:

#ifndef __NEBULA_NO_THREADS__
    #ifdef __WIN32__
        HANDLE wmutex;
    #else
        pthread_mutex_t pmutex;
    #endif
#endif
};

//------------------------------------------------------------------------------
/**
*/
inline 
nMutex::nMutex()
{
#ifndef __NEBULA_NO_THREADS__
    #ifdef __WIN32__
        this->wmutex = CreateMutex(NULL, FALSE, NULL);
    #else
        pthread_mutex_init(&(this->pmutex),NULL);
    #endif
#endif
}

//------------------------------------------------------------------------------
/**
*/
inline 
nMutex::~nMutex()
{
#ifndef __NEBULA_NO_THREADS__
    #ifdef __WIN32__
        CloseHandle(this->wmutex);
    #else
        pthread_mutex_destroy(&(this->pmutex));
    #endif
#endif
}

//------------------------------------------------------------------------------
/**
*/
inline 
void 
nMutex::Lock()
{
#ifndef __NEBULA_NO_THREADS__
    #ifdef __WIN32__
        WaitForSingleObject(this->wmutex, INFINITE);
    #else
        pthread_mutex_lock(&(this->pmutex));
    #endif
#endif
}

//------------------------------------------------------------------------------
/**
*/
inline 
void 
nMutex::Unlock()
{
#ifndef __NEBULA_NO_THREADS__
    #ifdef __WIN32__
        ReleaseMutex(this->wmutex);
    #else
        pthread_mutex_unlock(&(this->pmutex));
    #endif
#endif
}
//-------------------------------------------------------------------
#endif
