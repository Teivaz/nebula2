#ifndef N_KEYBOARDDEVICE_H
#define N_KEYBOARDDEVICE_H
//------------------------------------------------------------------------------
/**
    @class nKeyboardDevice
    @ingroup Input

    @brief A Nebula input device wrapping around a keyboard.

    (C) 2002 RadonLabs GmbH
*/
#include "input/ninputdevice.h"

//------------------------------------------------------------------------------
class nKeyboardDevice : public nInputDevice
{
public:
    /// constructor
    nKeyboardDevice(nKernelServer* ks, nInputServer* is, int devNum, int numAxs, int numBtns, int numPvs);
    /// destructor
    virtual ~nKeyboardDevice();
    /// export the device into the Nebula input device database
    virtual void Export(nRoot* dir);
    /// emit button down events
    virtual void EmitButtonEvents(int btnNum, bool pressed);
};
//------------------------------------------------------------------------------
#endif
