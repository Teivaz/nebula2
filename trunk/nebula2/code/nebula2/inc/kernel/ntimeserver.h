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
#ifndef N_TYPES_H
#include "kernel/ntypes.h"
#endif

#ifdef __XBxX__
#   include "xbox/nxbwrapper.h"
#elif __WIN32__
#   ifndef _INC_WINDOWS
#   include <windows.h> 
#   endif
#else
#include <sys/time.h>
#include <unistd.h>
#endif

#ifndef N_PROFILER_H
#include "kernel/nprofiler.h"
#endif

#ifndef N_ROOT_H
#include "kernel/nroot.h"
#endif

#ifndef N_REF_H
#include "kernel/nref.h"
#endif

#ifndef N_ENV_H
#include "kernel/nenv.h"
#endif

#undef N_DEFINES
#define N_DEFINES nTimeServer
#include "kernel/ndefdllclass.h"

//------------------------------------------------------------------------------
class N_PUBLIC nTimeServer : public nRoot 
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

    static nKernelServer* kernelServer;

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
