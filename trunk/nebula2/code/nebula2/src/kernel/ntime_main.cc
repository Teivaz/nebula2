//------------------------------------------------------------------------------
//  ntime_main.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "kernel/ntimeserver.h"
#include "kernel/nkernelserver.h"

#if defined(__LINUX__) || defined(__MACOSX__)
#define N_MICROSEC_INT    (1000000)
#define N_MICROSEC_FLOAT  (1000000.0)
#define tv2micro(x) (x.tv_sec * N_MICROSEC_INT + x.tv_usec);
#endif

nNebulaScriptClass(nTimeServer, "nroot");
nTimeServer* nTimeServer::Singleton = 0;

//------------------------------------------------------------------------------
/**
*/
nTimeServer::nTimeServer() :
    stopped(false),
    frame_enabled(false),
    frame_time(0.0),
    lock_delta_t(0.0),
    wait_delta_t(0.0),
    lock_time(0.0),
    time_stop(0)
{
    n_assert(0 == Singleton);
    Singleton = this;

#ifdef __WIN32__
    QueryPerformanceCounter((LARGE_INTEGER *) &(this->time_diff));
#elif defined(__LINUX__) || defined(__MACOSX__)
    struct timeval tv;
    gettimeofday(&tv,NULL);
    this->time_diff = tv2micro(tv);
#else
#error "Method not implemented!"
#endif
}

//------------------------------------------------------------------------------
/**
*/
nTimeServer::~nTimeServer()
{
    n_assert(Singleton);
    Singleton = 0;
}

//------------------------------------------------------------------------------
/**
*/
void
nTimeServer::ResetTime()
{
    this->lock_time = 0;

#ifdef __WIN32__
    QueryPerformanceCounter((LARGE_INTEGER *)&(this->time_diff));
#elif defined(__LINUX__) || defined(__MACOSX__)
    struct timeval tv;
    gettimeofday(&tv,NULL);
    this->time_diff = tv2micro(tv);
#else
#error "Method not implemented!"
#endif
}

//------------------------------------------------------------------------------
/**
*/
void
nTimeServer::SetTime(double t)
{
    this->lock_time = t;
    if (this->frame_enabled) this->frame_time = t;

#ifdef __WIN32__
    // t nach Ticks umrechnen
    LONGLONG freq;
    QueryPerformanceFrequency((LARGE_INTEGER *)&freq);
    LONGLONG td = (LONGLONG) (t * ((double)freq));
    QueryPerformanceCounter((LARGE_INTEGER *)&(this->time_diff));
    this->time_stop = this->time_diff;
    this->time_diff -= td;
#elif defined(__LINUX__) || defined(__MACOSX__)
    // t nach Microsecs umrechnen
    long long td = (long long int) (t * N_MICROSEC_FLOAT);
    struct timeval tv;
    gettimeofday(&tv,NULL);
    this->time_diff = tv2micro(tv);
    this->time_stop = this->time_diff;
    this->time_diff -= td;
#else
#error "Method not implemented!"
#endif
}

//------------------------------------------------------------------------------
/**
*/
void
nTimeServer::StopTime()
{
    if (this->stopped)
    {
        n_printf("Time already stopped!\n");
    }
    else
    {
        this->stopped = true;

#ifdef __WIN32__
        QueryPerformanceCounter((LARGE_INTEGER *)&(this->time_stop));
#elif defined(__LINUX__) || defined(__MACOSX__)
        struct timeval tv;
        gettimeofday(&tv,NULL);
        this->time_stop = tv2micro(tv);
#else
#error "Method not implemented!"
#endif
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nTimeServer::StartTime()
{
    if (!this->stopped)
    {
        n_printf("Time already started!\n");
    }
    else
    {
        this->stopped = false;

#   ifdef __WIN32__
        LONGLONG time,td;
        QueryPerformanceCounter((LARGE_INTEGER *)&time);
        td = time - this->time_stop;
        this->time_diff += td;
#   elif defined(__LINUX__) || (__MACOSX__)
        long long int time;
        long long int td;
        struct timeval tv;
        gettimeofday(&tv,NULL);
        time = tv2micro(tv);
        td = time - this->time_stop;
        this->time_diff += td;
#   else
#error "Method not implemented!"
#   endif
    }
}

//------------------------------------------------------------------------------
/**
*/
double
nTimeServer::GetTime()
{
    if (this->lock_delta_t > 0.0) return this->lock_time;
    else {
#       ifdef __WIN32__
        LONGLONG time,freq;
        QueryPerformanceCounter((LARGE_INTEGER *)&time);
        if (this->stopped) time = this->time_stop;
        QueryPerformanceFrequency((LARGE_INTEGER *)&freq);
        LONGLONG td = time - this->time_diff;
        double d_time = ((double)td) / ((double)freq);
        return d_time;
        return 0.0;
    }
#   elif defined(__LINUX__) || defined(__MACOSX__)
        long long int time;
        long long int td;
        double d_time;
        if (this->stopped) time = this->time_stop;
        else {
            struct timeval tv;
            gettimeofday(&tv,NULL);
            time = tv2micro(tv);
        }
        td = time - this->time_diff;
        d_time = ((double)td) / N_MICROSEC_FLOAT;
        return d_time;
    }
#   else
#error "Method not implemented!"
#   endif
}

//------------------------------------------------------------------------------
/**
*/
void
nTimeServer::LockDeltaT(double dt)
{
    n_assert(dt >= 0.0);
    if (dt == 0.0)
    {
        this->SetTime(this->lock_time);
    }
    else
    {
        this->lock_time = this->GetTime();
    }
    this->lock_delta_t = dt;
}

//------------------------------------------------------------------------------
/**
*/
void
nTimeServer::WaitDeltaT(double dt)
{
    n_assert(dt >= 0.0);
    this->wait_delta_t = dt;
}

//------------------------------------------------------------------------------
/**
*/
double
nTimeServer::GetLockDeltaT()
{
    return this->lock_delta_t;
}

//------------------------------------------------------------------------------
/**
*/
double
nTimeServer::GetWaitDeltaT()
{
    return this->wait_delta_t;
}

//------------------------------------------------------------------------------
/**
*/
void
nTimeServer::Trigger()
{
    if (this->lock_delta_t > 0.0)
    {
        if (!this->stopped)
        {
            this->lock_time += this->lock_delta_t;
        }
    }
    if (this->wait_delta_t > 0.0) n_sleep(this->wait_delta_t);
    if (this->frame_enabled) this->frame_time = this->GetTime();
}

//------------------------------------------------------------------------------
/**
*/
void nTimeServer::EnableFrameTime(void)
{
    this->frame_enabled = true;
    this->frame_time = this->GetTime();
}

//------------------------------------------------------------------------------
/**
*/
void nTimeServer::DisableFrameTime(void)
{
    this->frame_enabled = false;
}

//------------------------------------------------------------------------------
/**
*/
double nTimeServer::GetFrameTime(void)
{
    if (this->frame_enabled) return this->frame_time;
    else                     return this->GetTime();
}

//------------------------------------------------------------------------------
//  EOF
//------------------------------------------------------------------------------

