#ifndef N_TIMESERVER_H
#define N_TIMESERVER_H
//------------------------------------------------------------------------------
/**
    @class nTimeServer
    @ingroup NebulaTimeManagement
    @brief nTimeServer provides a high resolution time source.

    See also @ref N2ScriptInterface_ntimeserver

    (C) 2002 RadonLabs GmbH
*/
#include "kernel/ntypes.h"

#ifdef __XBxX__
#   include "xbox/nxbwrapper.h"
#elif __WIN32__
#   ifndef _INC_WINDOWS
#   define WIN32_LEAN_AND_MEAN
#   include <windows.h> 
#   endif
#else
#include <sys/time.h>
#include <unistd.h>
#endif

#include "kernel/nprofiler.h"
#include "kernel/nroot.h"
#include "kernel/nref.h"
#include "kernel/nenv.h"

//------------------------------------------------------------------------------
class nTimeServer : public nRoot 
{
public:
    /// constructor
    nTimeServer();
    /// destructor
    virtual ~nTimeServer();
    /// trigger time server
    void Trigger();
    /// reset time to 0
    void ResetTime();
    /// stop time
    void StopTime();
    /// start time
    void StartTime();
    /// get current time
    double GetTime();
    /// set current time
    void SetTime(double);
    /// create a profiler object
    nProfiler *NewProfiler(const char *);
    /// release a profiler object
    void ReleaseProfiler(nProfiler*);
    /// get profiler list
    nStrList* GetProfilers();
    /// lock frame time
    void LockDeltaT(double);
    /// set a wait period
    void WaitDeltaT(double);
    /// get locked frame time
    double GetLockDeltaT();
    /// get frame wait period
    double GetWaitDeltaT();
    /// enable frame based timing
    void EnableFrameTime();
    /// get current frame time
    double GetFrameTime();
    /// disable frame based timing
    void DisableFrameTime();

private:
    bool stopped;
    bool frame_enabled;
    double frame_time;
    double lock_delta_t;
    double wait_delta_t;
    double lock_time;
    /// list of living nProfiler objects
    nStrList prof_list;             

#   ifdef __WIN32__
    LONGLONG time_diff;
    LONGLONG time_stop;
#   else
    long long int time_diff;
    long long int time_stop; 
#   endif
};
//--------------------------------------------------------------------
#endif
