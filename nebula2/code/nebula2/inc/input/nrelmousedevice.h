#ifndef N_RELMOUSEDEVICE_H
#define N_RELMOUSEDEVICE_H
//------------------------------------------------------------------------------
/**
    @class nRelMouseDevice
    @ingroup NebulaInputSystem

    Directly emit mouse movement events.

    (C) 2002 RadonLabs GmbH
*/
#ifndef N_INPUTDEVICE_H
#include "input/ninputdevice.h"
#endif

#ifndef N_AXISFILTER_H
#include "input/naxisfilter.h"
#endif

#undef N_DEFINES
#define N_DEFINES nRelMouseDevice
#include "kernel/ndefdllclass.h"

//------------------------------------------------------------------------------
class N_PUBLIC nRelMouseDevice : public nInputDevice
{
public:
    /// constructor
    nRelMouseDevice(nKernelServer* ks, nInputServer* is, int devNum, int numAxs, int numBtns, int numPvs);
    /// destructor
    virtual ~nRelMouseDevice();
    /// export the device into the Nebula input device database
    virtual void Export(nRoot* dir);
    /// prepare event emission
    virtual void BeginEmitEvents(double time);
    /// emit axis move events
    virtual void EmitAxisMovedEvents(int axisNum, int value);
    /// emit button down events
    virtual void EmitButtonEvents(int btnNum, bool pressed);
    /// finish event emission
    virtual void EndEmitEvents();

protected:
    /// reset mouse axis if not moved
    void ResetAxis(int axisNum);

    bool xAxisMoved;
    bool yAxisMoved;

    enum
    {
        NUMFILTEREDAXIS = 2,
    };
    nAxisFilter filterArray[NUMFILTEREDAXIS];
};
//------------------------------------------------------------------------------
#endif

