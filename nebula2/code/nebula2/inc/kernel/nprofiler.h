#ifndef N_PROFILER_H
#define N_PROFILER_H
//------------------------------------------------------------------------------
/**
    @class nProfiler
    @ingroup NebulaTimeManagement
    @brief nProfiler provides an easy way to measure time intervals.

    (C) 2002 RadonLabs GmbH
*/
#include "kernel/nkernelserver.h"
#include "kernel/nref.h"
#include "kernel/nenv.h"

//------------------------------------------------------------------------------
class nProfiler
{
public:
    /// constuctor
    nProfiler(const char* name);
    /// destructor
    ~nProfiler();
    /// start accumulated profiling 
    void Start();
    /// stop accumulated profiling, value is written to watcher variable
    void Stop();

private:
    nRef<nEnv> refEnv;
    nTime startTime;
    bool isStarted;
};

//------------------------------------------------------------------------------
/**
*/
inline 
nProfiler::nProfiler(const char* name)
{
    char buf[N_MAXPATH];
    sprintf(buf, "/sys/var/%s", name);
    this->refEnv = (nEnv*) nKernelServer::Instance()->Lookup(buf);
    if (!this->refEnv.isvalid())
    {
        this->refEnv = (nEnv *) nKernelServer::Instance()->New("nenv",buf);
    }
    this->startTime = 0.0;
    this->isStarted = false;
}

//------------------------------------------------------------------------------
/**
*/
inline 
nProfiler::~nProfiler()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline 
void 
nProfiler::Start() 
{
    n_assert(!this->isStarted);
    this->startTime = nTimeServer::Instance()->GetTime();
    this->isStarted = true;
}

//------------------------------------------------------------------------------
/**
*/
inline 
void 
nProfiler::Stop() 
{
    n_assert(this->isStarted);
    nTime stop = nTimeServer::Instance()->GetTime();
    nTime diff = stop - this->startTime;
    this->refEnv->SetF(float(diff));
    this->isStarted = false;
}

//------------------------------------------------------------------------------
#endif
