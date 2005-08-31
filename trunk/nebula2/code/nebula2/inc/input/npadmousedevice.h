#ifndef N_PADMOUSEDEVICE_H
#define N_PADMOUSEDEVICE_H
//------------------------------------------------------------------------------
/**
    @class nPadMouseDevice
    @ingroup Input

    @brief A Nebula mouse device which emulates a non-centering joystick.

    (C) 2002 RadonLabs GmbH
*/
#include "input/ninputdevice.h"

//------------------------------------------------------------------------------
class nPadMouseDevice : public nInputDevice
{
public:
    /// constructor
    nPadMouseDevice(nKernelServer* ks, nInputServer* is, int devNum, int numAxs, int numBtns, int numPvs);
    /// destructor
    virtual ~nPadMouseDevice();
    /// export the device into the Nebula input device database
    virtual void Export(nRoot* dir);
    /// emit axis move events
    virtual void EmitAxisMovedEvents(int axisNum, int value);
    /// emit button down events
    virtual void EmitButtonEvents(int btnNum, bool pressed);
};
//------------------------------------------------------------------------------
#endif
