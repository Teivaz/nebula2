#ifndef N_PROFILER_H
#define N_PROFILER_H
//------------------------------------------------------------------------------
/**
    @class nProfiler
    @ingroup Time
    @brief nProfiler provides an easy way to measure time intervals.

    (C) 2002 RadonLabs GmbH
*/
#include "kernel/nkernelserver.h"
#include "kernel/nref.h"
#include "kernel/nenv.h"
#include "kernel/ntimeserver.h"

//------------------------------------------------------------------------------
class nProfiler
{
public:
    /// default constructor
    nProfiler();
    /// constuctor
    nProfiler(const char* name);
    /// destructor
    ~nProfiler();
    /// initialize the profiler (if default constructor used)
    void Initialize(const char* name);
    /// return true if profiler has been initialized
    bool IsValid() const;
    /// start one-shot profiling 
    void Start();
    /// return true if profiler has been started
    bool IsStarted() const;
    /// stop one-shot profiling, value is written to watcher variable
    void Stop();
    /// reset the accumulator
    void ResetAccum();
    /// start accumulated profiling
    void StartAccum();
    /// stop accumulated profiling
    void StopAccum();
    /// get the measured time
    float GetTime();

private:
    nRef<nEnv> refEnv;
    nTime startTime;
    bool isStarted;
    nTime accumTime;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nProfiler::Initialize(const char* name)
{
    n_assert(name);
    char buf[N_MAXPATH];
    snprintf(buf, sizeof(buf), "/sys/var/%s", name);
    this->refEnv = (nEnv*) nKernelServer::Instance()->Lookup(buf);
    if (!this->refEnv.isvalid())
    {
        this->refEnv = (nEnv *) nKernelServer::Instance()->New("nenv",buf);
    }
    this->startTime = 0.0;
    this->isStarted = false;
    this->accumTime = 0.0;
}

//------------------------------------------------------------------------------
/**
*/
inline
nProfiler::nProfiler() :
    startTime(0.0),
    isStarted(false),
    accumTime(0.0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline 
nProfiler::nProfiler(const char* name)
{
    this->Initialize(name);
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
bool
nProfiler::IsValid() const
{
    return this->refEnv.isvalid();
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nProfiler::IsStarted() const
{
    return this->isStarted;
}

//------------------------------------------------------------------------------
/**
*/
inline 
void 
nProfiler::Start() 
{
    if (this->isStarted)
    {
        this->Stop();
    }
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
    if (this->isStarted)
    {
        nTime stop = nTimeServer::Instance()->GetTime();
        nTime diff = stop - this->startTime;
        this->refEnv->SetF(float(diff) * 1000.0f);
        this->isStarted = false;
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nProfiler::ResetAccum()
{
    this->accumTime = 0.0;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nProfiler::StartAccum()
{
    this->Start();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nProfiler::StopAccum()
{
    n_assert(this->isStarted);
    nTime stop = nTimeServer::Instance()->GetTime();
    nTime diff = stop - this->startTime;
    this->accumTime += diff;
    this->refEnv->SetF(float(this->accumTime) * 1000.0f);
    this->isStarted = false;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nProfiler::GetTime()
{
    return this->refEnv->GetF();
}

//------------------------------------------------------------------------------
#endif
