#ifndef N_JOYMOUSEDEVICE_H
#define N_JOYMOUSEDEVICE_H
//------------------------------------------------------------------------------
/**
    @class nJoyMouseDevice
    @ingroup NebulaInputSystem

    A Nebula input device which takes as input mouse data and emulates
    a recentering joystick.

    (C) 2002 RadonLabs GmbH
*/
#ifndef N_INPUTDEVICE_H
#include "input/ninputdevice.h"
#endif

#undef N_DEFINES
#define N_DEFINES nJoyMouseDevice
#include "kernel/ndefdllclass.h"

//------------------------------------------------------------------------------
class N_PUBLIC nJoyMouseDevice : public nInputDevice
{
public:
    /// constructor
    nJoyMouseDevice(nKernelServer* ks, nInputServer* is, int devNum, int numAxs, int numBtns, int numPvs);
    /// destructor
    virtual ~nJoyMouseDevice();
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

private:
    /// recenter a moved axis
    void RecenterAxis(int axisNum);

    bool xAxisMoved;
    bool yAxisMoved;
};
//------------------------------------------------------------------------------
#endif
