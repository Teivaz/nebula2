#ifndef N_JOYSTICKDEVICE_H
#define N_JOYSTICKDEVICE_H
//------------------------------------------------------------------------------
/**
    @class nJoystickDevice
    @ingroup Input

    @brief A Nebula joystick input device. Nebula joystick device have buttons, axes
    and button-axes. POVs are tranlated into normal axes. For each hardware
    joystick axes, 2 device axes and 2 axis buttons are created:

    x hardware axis:
       - -x          -> negative axis (0 .. +1.0)
       - +x          -> positive axis (0 .. +1.0)
       - -xbtn       -> negative axis button 
       - +xbtn       -> positive axis button

    Each pov translates to 2 "hardware axes", resulting in 4 Nebula axis and
    4 Nebula axis buttons.

    (C) 2002 RadonLabs GmbH
*/
#include "input/ninputdevice.h"
#include "input/naxisfilter.h"

//------------------------------------------------------------------------------
class nJoystickDevice : public nInputDevice
{
public:
    /// constructor
    nJoystickDevice(nKernelServer* ks, nInputServer* is, int devNum, int numAxs, int numBtns, int numPvs);
    /// destructor
    virtual ~nJoystickDevice();
    /// export the device into the Nebula input device database
    virtual void Export(nRoot* dir);
    /// prepare event emission
    virtual void BeginEmitEvents(double time);
    /// emit axis move events
    virtual void EmitAxisMovedEvents(int axisNum, int value);
    /// emit button down events
    virtual void EmitButtonEvents(int btnNum, bool pressed);
    /// emit POV event
    virtual void EmitPovMovedEvents(int povNum, int povVal);
    /// finish event emission
    virtual void EndEmitEvents();

protected:
    /// low level event emitting for analog axes
    void ProcessAnalogAxis(int axisNum, float val);

    enum 
    {
        NUMFILTEREDAXIS = 4,
    };
    bool axisMoved[NUMFILTEREDAXIS];
    nAxisFilter filterArray[NUMFILTEREDAXIS];
};  
//------------------------------------------------------------------------------
#endif

