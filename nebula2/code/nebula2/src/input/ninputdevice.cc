//------------------------------------------------------------------------------
//  ninputdevice.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "input/ninputdevice.h"
#include "input/ninputserver.h"
#include "kernel/nenv.h"

//------------------------------------------------------------------------------
/**
*/
nInputDevice::nInputDevice(nKernelServer* ks, nInputServer* is, int devNum, int numAxs, int numBtns, int numPvs) :
    kernelServer(ks),
    inputServer(is),
    frameTime(0.0),
    sourceDevice(0),
    deviceNum(devNum),
    axisMinRange(0),
    axisMaxRange(10),
    numAxes(numAxs),
    numButtons(numBtns),
    numPovs(numPvs),
    numValues(0),
    valueStore(0)
{
    n_assert(this->kernelServer);
    n_assert(this->inputServer);
}

//------------------------------------------------------------------------------
/**
*/
nInputDevice::~nInputDevice()
{
    if (this->valueStore)
    {
        n_delete_array(this->valueStore);
        this->valueStore = 0;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nInputDevice::SetSourceDevice(void* src)
{
    this->sourceDevice = src;
}

//------------------------------------------------------------------------------
/**
*/
void*
nInputDevice::GetSourceDevice()
{
    return this->sourceDevice;
}

//------------------------------------------------------------------------------
/**
*/
void
nInputDevice::SetAxisRange(int minR, int maxR)
{
    this->axisMinRange = minR;
    this->axisMaxRange = maxR;
}

//------------------------------------------------------------------------------
/**
*/
int
nInputDevice::GetMinAxisRange()
{
    return this->axisMinRange;
}

//------------------------------------------------------------------------------
/**
*/
int
nInputDevice::GetMaxAxisRange()
{
    return this->axisMaxRange;
}

//------------------------------------------------------------------------------
/**
*/
int
nInputDevice::GetDeviceNum()
{
    return this->deviceNum;
}

//------------------------------------------------------------------------------
/**
*/
int
nInputDevice::GetNumAxes()
{
    return this->numAxes;
}

//------------------------------------------------------------------------------
/**
*/
int
nInputDevice::GetNumButtons()
{
    return this->numButtons;
}

//------------------------------------------------------------------------------
/**
*/
int
nInputDevice::GetNumPovs()
{
    return this->numPovs;
}

//------------------------------------------------------------------------------
/**
*/
void
nInputDevice::ExportAxis(int devId, const char* chnName, int axis)
{
    n_assert(chnName);

    char buf[N_MAXNAMELEN];
    sprintf(buf, "devid=%d type=%d axis=%d", devId, (int) N_INPUT_AXIS_MOVE, axis);
    nEnv* env = (nEnv*) kernelServer->New("nenv", chnName);
    env->SetS(buf);
}

//------------------------------------------------------------------------------
/**
*/
void
nInputDevice::ExportButton(int devId, const char* chnName, int button)
{
    n_assert(chnName);

    char buf[N_MAXNAMELEN];
    sprintf(buf, "devid=%d type=%d btn=%d", devId, (int) N_INPUT_BUTTON_DOWN, button);
    nEnv* env = (nEnv*) kernelServer->New("nenv", chnName);
    env->SetS(buf);
}

//------------------------------------------------------------------------------
/**
    Create and link a lowlevel axis move input event.

    @param  devId       the Nebula device id
    @param  axisIndex   the axis index
    @param  axisVal     the axis value (0.0 .. 1.0)
*/
void
nInputDevice::PutAxisEvent(int devId, int axisIndex, float axisVal)
{
    nInputEvent* ie = this->inputServer->NewEvent();
    n_assert(ie);
    ie->SetType(N_INPUT_AXIS_MOVE);
    ie->SetDeviceId(devId);
    ie->SetAxis(axisIndex);
    ie->SetAxisValue(axisVal);
    this->inputServer->LinkEvent(ie);
}

//------------------------------------------------------------------------------
/**
    Create and link a button down event.

    @param  devId           the Nebula device id
    @param  buttonIndex     the button index
*/
void
nInputDevice::PutButtonDownEvent(int devId, int buttonIndex)
{
    nInputEvent* ie = this->inputServer->NewEvent();
    n_assert(ie);
    ie->SetType(N_INPUT_BUTTON_DOWN);
    ie->SetDeviceId(devId);
    ie->SetButton(buttonIndex);
    this->inputServer->LinkEvent(ie);
}

//------------------------------------------------------------------------------
/**
    Create and link a button up event.

    @param  devId           the Nebula device id
    @param  buttonIndex     the button index
*/
void
nInputDevice::PutButtonUpEvent(int devId, int buttonIndex)
{
    nInputEvent* ie = this->inputServer->NewEvent();
    n_assert(ie);
    ie->SetType(N_INPUT_BUTTON_UP);
    ie->SetDeviceId(devId);
    ie->SetButton(buttonIndex);
    this->inputServer->LinkEvent(ie);
}

//------------------------------------------------------------------------------
/**
    Generate the additional button events that are associated with an axis.
    The generated button events depend on the previous value of the axis,
    and a new value.
*/
void
nInputDevice::PutAxisButtonEvents(int devId, int buttonIndex, float oldVal, float newVal)
{
    if (newVal > 0.0f)
    {
        // generate button down event if previous axis state was 0.0
        if (oldVal == 0.0f)
        {
            this->PutButtonDownEvent(devId, buttonIndex);
        }
    }
    else
    {
        // generate button up event if previous state was greater 0.0
        if (oldVal > 0.0f)
        {
            this->PutButtonUpEvent(devId, buttonIndex);
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nInputDevice::Export(nRoot* /*dir*/)
{
    // empty
}

//------------------------------------------------------------------------------
/**
    This method is called before the Emit*() methods. Can be used by subclasses
    to prepare the device object for the event emission.
*/
void
nInputDevice::BeginEmitEvents(double time)
{
    this->frameTime = time;
}

//------------------------------------------------------------------------------
/**
*/
void 
nInputDevice::EmitAxisMovedEvents(int /*axisNum*/, int /*value*/)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nInputDevice::EmitButtonEvents(int /*btnNum*/, bool /*pressed*/)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nInputDevice::EmitPovMovedEvents(int /*povNum*/, int /*povVal*/)
{
    // empty
}

//------------------------------------------------------------------------------
/**
    This method is called after the Emit*() methods. Can be used by subclasses
    for any post-emission stuff.
*/
void
nInputDevice::EndEmitEvents()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nInputDevice::SetStoreSize(int s)
{
    n_assert(s > 0);
    n_assert(0 == this->valueStore);
    this->valueStore = n_new_array(float,s);
    this->numValues = s;
    int i;
    for (i = 0; i < s; i++)
    {
        this->valueStore[i] = 0.0f;
    }
}

//------------------------------------------------------------------------------
/**
*/
int
nInputDevice::GetStoreSize()
{
    return this->numValues;
}

//------------------------------------------------------------------------------
/**
*/
void
nInputDevice::SetStoreValue(int index, float value)
{
    n_assert((index >= 0) && (index < this->numValues));
    n_assert(this->valueStore);
    this->valueStore[index] = value;
}

//------------------------------------------------------------------------------
/**
*/
float
nInputDevice::GetStoreValue(int index)
{
    n_assert((index >= 0) && (index < this->numValues));
    n_assert(this->valueStore);
    return this->valueStore[index];
}

//------------------------------------------------------------------------------


