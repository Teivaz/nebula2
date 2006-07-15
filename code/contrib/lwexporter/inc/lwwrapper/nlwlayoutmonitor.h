#ifndef N_LW_LAYOUT_MONITOR_H
#define N_LW_LAYOUT_MONITOR_H
//----------------------------------------------------------------------------
extern "C" 
{
#include <lwsdk/lwmonitor.h>
}

#include "lwwrapper/nlwglobals.h"
#include "util/nstring.h"

//----------------------------------------------------------------------------
/**
    @class nLWLayoutMonitor
    @brief Encapsulates Lightwave's modal progress dialog.
*/
class nLWLayoutMonitor
{
public:
    /// constructor
    nLWLayoutMonitor(const nString& title, uint flags, 
                     const char* logFileName = 0);
    /// destructor
    ~nLWLayoutMonitor();

    void SetBounds(int x, int y, int width, int height);
    void Open(uint totalSteps, const nString& firstMsg);
    bool Step(uint increment, const nString& msg);
    void Close();
    bool UserAborted() const;

private:
    nLWGlobals::LMonFuncs monitorGlobal;
    nString title;
    nString logFileName;
    LWLMonID lwMonitor;
    nString curMsg;
    bool userAborted;
};

//----------------------------------------------------------------------------
#endif // N_LW_LAYOUT_MONITOR_H
