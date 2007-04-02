//------------------------------------------------------------------------------
//  nkeyboarddevice.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "input/nkeyboarddevice.h"

//------------------------------------------------------------------------------
/**
*/
nKeyboardDevice::nKeyboardDevice(nKernelServer* ks, nInputServer* is, int devNum, int numAxs, int numBtns, int numPvs) :
    nInputDevice(ks, is, devNum, numAxs, numBtns,numPvs)
{
    n_assert(0 == this->numAxes);
    n_assert(0 == this->numPovs);
}

//------------------------------------------------------------------------------
/**
*/
nKeyboardDevice::~nKeyboardDevice()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nKeyboardDevice::Export(nRoot* dir)
{
    n_assert(dir);

    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nKeyboardDevice::EmitButtonEvents(int btnNum, bool /*pressed*/)
{
    n_assert((btnNum >= 0) && (btnNum < 256));

    // empty
}

//------------------------------------------------------------------------------


