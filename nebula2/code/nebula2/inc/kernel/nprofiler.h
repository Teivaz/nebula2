#ifndef N_PROFILER_H
#define N_PROFILER_H
//------------------------------------------------------------------------------
/**
    @class nProfiler
    @ingroup NebulaTimeManagement

    A mini profiler class.

    (C) 2002 RadonLabs GmbH
*/
#ifndef N_STRNODE_H
#include "util/nstrnode.h"
#endif

#ifndef N_REF_H
#include "kernel/nref.h"
#endif

#ifndef N_ENV_H
#include "kernel/nenv.h"
#endif

//------------------------------------------------------------------------------
class nKernelServer;
class nProfiler : public nStrNode 
{
public:
    /// constuctor
    nProfiler(nKernelServer* ks, nRoot* owner, const char* name);
    /// destructor
    ~nProfiler();
    /// start accumulated profiling 
    void Start();
    /// stop accumulated profiling
    void Stop();
    /// rewind profiler
    void Rewind();

private:
    nRef<nEnv> refEnv;
    bool isStarted;

#ifdef __WIN32__
    LONGLONG start;
    LONGLONG accum;
#   else
    long long int start;
    long long int accum;
#   endif
};

//------------------------------------------------------------------------------
/**
*/
inline 
nProfiler::nProfiler(nKernelServer* ks, nRoot* owner, const char* name)
    : nStrNode(name)
{
    char buf[N_MAXPATH];
    sprintf(buf,"/sys/var/prof_%s",name);
    nRoot *o = ks->Lookup(buf);
    n_assert(NULL == o);
    this->refEnv = (nEnv *) ks->New("nenv",buf);
    this->start = 0;
    this->accum = 0;
    this->isStarted = false;
}

//------------------------------------------------------------------------------
/**
*/
inline 
nProfiler::~nProfiler()
{
    if (refEnv.isvalid()) 
    {
        refEnv->Release();
    }
}


#ifdef __WIN32__
//------------------------------------------------------------------------------
/**
*/
inline 
void 
nProfiler::Start() 
{
    n_assert(!this->isStarted);

    QueryPerformanceCounter((LARGE_INTEGER *) &(this->start));
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

    LONGLONG stop;
    QueryPerformanceCounter((LARGE_INTEGER *) &stop);
    this->accum += (stop - this->start);
    this->isStarted = false;
}

//------------------------------------------------------------------------------
/**
*/
inline 
void 
nProfiler::Rewind() 
{
    n_assert(!this->isStarted);
    LONGLONG freq;
    QueryPerformanceFrequency((LARGE_INTEGER *) &freq);
    double d_accum = (double) this->accum;
    double d_freq  = (double) freq;
    double d_time  = d_accum / d_freq;
    refEnv->SetF((float) d_time);
    this->accum = 0;
}

#elif __LINUX__
#define N_MICROSEC_INT    (1000000)
#define N_MICROSEC_FLOAT  (1000000.0)

//------------------------------------------------------------------------------
/**
*/
inline 
void 
nProfiler::Start() 
{
    n_assert(!this->isStarted);
    struct timeval tv;
    gettimeofday(&tv,NULL);
    this->start = tv.tv_sec * N_MICROSEC_INT + tv.tv_usec;
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
    struct timeval tv;
    long long int stop;
    gettimeofday(&tv,NULL);
    stop = tv.tv_sec * N_MICROSEC_INT + tv.tv_usec;
    this->accum += stop - this->start;
    this->isStarted = false;
}

//------------------------------------------------------------------------------
/**
*/
inline 
void 
nProfiler::Rewind() 
{
    n_assert(!this->isStarted);
    double d_accum = (double) this->accum;
    double d_time = (d_accum / N_MICROSEC_FLOAT);
    this->refEnv->SetF((float)d_time);
    this->accum = 0;
}

#else
#error "nProfiler class not supported"
#endif

//------------------------------------------------------------------------------
#endif
