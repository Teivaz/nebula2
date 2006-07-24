//------------------------------------------------------------------------------
//  njoymousedevice.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "input/njoymousedevice.h"
#include "input/ninputserver.h"
#include "kernel/nenv.h"

//------------------------------------------------------------------------------
/**
*/
nJoyMouseDevice::nJoyMouseDevice(nKernelServer* ks, nInputServer* is, int devNum, int numAxs, int numBtns, int numPvs) :
    nInputDevice(ks, is, devNum, numAxs, numBtns,numPvs),
    xAxisMoved(false),
    yAxisMoved(false)
{
    n_assert(0 == this->numPovs);
    if (this->numAxes > 0)
    {
        this->SetStoreSize(this->numAxes * 3);
    }
}

//------------------------------------------------------------------------------
/**
*/
nJoyMouseDevice::~nJoyMouseDevice()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Export device description into Nebula device database.

    @param  dir     points to root directory of device database
*/
void
nJoyMouseDevice::Export(nRoot* dir)
{
    n_assert(dir);

    int devId = N_INPUT_JOYMOUSE(this->deviceNum);

    kernelServer->PushCwd(dir);

    // create the device entry
    char devName[N_MAXNAMELEN];
    sprintf(devName, "joymouse%d", this->deviceNum);
    nRoot* devEntry = kernelServer->New("nroot", devName);

    // write the input channels
    kernelServer->PushCwd(devEntry);
    nRoot* devChannels = kernelServer->New("nroot", "channels");
    n_assert(devChannels);
    kernelServer->PushCwd(devChannels);

    // for each axis, export 2 Nebula axes (positive/negative)
    int curAxis;
    for (curAxis = 0; curAxis < this->numAxes; curAxis++)
    {
        const char* axisName = 0;
        char chnName[N_MAXNAMELEN];
        switch (curAxis)
        {
            case 0: axisName = "x"; break;
            case 1: axisName = "y"; break;
            case 2: axisName = "z"; break;
            case 3: axisName = "r"; break;
            case 4: axisName = "s"; break;
            case 5: axisName = "t"; break;
            case 6: axisName = "u"; break;
            case 7: axisName = "v"; break;
            case 8: axisName = "w"; break;
            default:
                continue;
        }

        int axisOffset = 2 * curAxis;
        int btnOffset  = numButtons + axisOffset;

        // export negative axis
        sprintf(chnName, "-%s", axisName);
        this->ExportAxis(devId, chnName, axisOffset + 0);

        // export positive axis
        sprintf(chnName, "+%s", axisName);
        this->ExportAxis(devId, chnName, axisOffset + 1);

        // export negative axis button
        sprintf(chnName, "-%sbtn", axisName);
        this->ExportButton(devId, chnName, btnOffset + 0);

        // export positive axis button
        sprintf(chnName, "+%sbtn", axisName);
        this->ExportButton(devId, chnName, btnOffset + 1);
    }

    // export buttons
    int curBtn;
    for (curBtn = 0; curBtn < this->numButtons; curBtn++)
    {
        char chnName[N_MAXNAMELEN];
        sprintf(chnName, "btn%d", curBtn);
        this->ExportButton(devId, chnName, curBtn);
    }

    kernelServer->PopCwd();
    kernelServer->PopCwd();
    kernelServer->PopCwd();
}

//------------------------------------------------------------------------------
/**
    This resets the axis moved flags for the x and y axis.
*/
void
nJoyMouseDevice::BeginEmitEvents(double time)
{
    this->xAxisMoved = false;
    this->yAxisMoved = false;
    nInputDevice::BeginEmitEvents(time);
}

//------------------------------------------------------------------------------
/**
*/
void
nJoyMouseDevice::EmitAxisMovedEvents(int axisNum, int value)
{
    n_assert((axisNum >= 0) && (axisNum < this->numAxes));
    int devId = N_INPUT_JOYMOUSE(this->deviceNum);

    // touch the axis moved flags
    if (0 == axisNum)
    {
        this->xAxisMoved = true;
    }
    else if (1 == axisNum)
    {
        this->yAxisMoved = true;
    }

    // relative axis value from -1.0f .. +1.0f
    float minRange = (float) this->axisMinRange;
    float maxRange = (float) this->axisMaxRange;
    float fValue   = (float) value;
    float relVal = (((fValue - minRange) / (maxRange - minRange)) * 2.0f) - 1.0f;
    if ((relVal > -0.01f) && (relVal < 0.01f))
    {
        relVal = 0.0f;
    }
    relVal *= 0.25f;

    // compute absolute axis value by adding it to the previous axis value
    int storeIndex = (this->numAxes * 2) + axisNum;
    float val = this->GetStoreValue(storeIndex);
    val += relVal;
    if (val < -1.0f)
    {
        val = -1.0f;
    }
    else if (val > 1.0f)
    {
        val = 1.0f;
    }
    this->SetStoreValue(storeIndex, val);

    // split index and value into positive/negative axis
    int negAxisIndex = (axisNum * 2) + 0;
    int posAxisIndex = (axisNum * 2) + 1;
    float negAxisVal = (val < 0.0f) ? -val : 0.0f;
    float posAxisVal = (val > 0.0f) ? +val : 0.0f;

    float oldNegAxisVal = this->GetStoreValue(negAxisIndex);
    float oldPosAxisVal = this->GetStoreValue(posAxisIndex);

    // generate positive/negative axis move events
    this->PutAxisEvent(devId, negAxisIndex, negAxisVal);
    this->PutAxisEvent(devId, posAxisIndex, posAxisVal);

    // generate additional axis button events
    this->PutAxisButtonEvents(devId, this->numButtons + negAxisIndex, oldNegAxisVal, negAxisVal);
    this->PutAxisButtonEvents(devId, this->numButtons + posAxisIndex, oldPosAxisVal, posAxisVal);

    this->SetStoreValue(negAxisIndex, negAxisVal);
    this->SetStoreValue(posAxisIndex, posAxisVal);
}

//------------------------------------------------------------------------------
/**
*/
void
nJoyMouseDevice::EmitButtonEvents(int btnNum, bool pressed)
{
    n_assert((btnNum >= 0) && (btnNum < this->numButtons));

    if (pressed)
    {
        this->PutButtonDownEvent(N_INPUT_JOYMOUSE(this->deviceNum), btnNum);
    }
    else
    {
        this->PutButtonUpEvent(N_INPUT_JOYMOUSE(this->deviceNum), btnNum);
    }
}

//------------------------------------------------------------------------------
/**
    Slowly recenter a mouse axis to its zero position.
*/
void
nJoyMouseDevice::RecenterAxis(int axisNum)
{
    int devId = N_INPUT_JOYMOUSE(this->deviceNum);

    // get current axis value
    int storeIndex = (this->numAxes * 2) + axisNum;
    float val = this->GetStoreValue(storeIndex);

    // dont generate events if axis value is already at 0.0
    if (0.0f == val)
    {
        return;
    }

    // else let current value go towards 0.0
    float speed = (float) (0.5f * this->frameTime);
    if (val > 0.0f)
    {
        val -= speed;
        if (val < 0.0f)
        {
            val = 0.0f;
        }
    }
    else
    {
        val += speed;
        if (val > 0.0f)
        {
            val = 0.0f;
        }
    }
    this->SetStoreValue(storeIndex, val);

    // split index and value into positive/negative axis
    int negAxisIndex = (axisNum * 2) + 0;
    int posAxisIndex = (axisNum * 2) + 1;
    float negAxisVal = (val < 0.0f) ? -val : 0.0f;
    float posAxisVal = (val > 0.0f) ? +val : 0.0f;

    float oldNegAxisVal = this->GetStoreValue(negAxisIndex);
    float oldPosAxisVal = this->GetStoreValue(posAxisIndex);

    // generate positive/negative axis move events
    this->PutAxisEvent(devId, negAxisIndex, negAxisVal);
    this->PutAxisEvent(devId, posAxisIndex, posAxisVal);

    // generate additional axis button events
    this->PutAxisButtonEvents(devId, this->numButtons + negAxisIndex, oldNegAxisVal, negAxisVal);
    this->PutAxisButtonEvents(devId, this->numButtons + posAxisIndex, oldPosAxisVal, posAxisVal);

    this->SetStoreValue(negAxisIndex, negAxisVal);
    this->SetStoreValue(posAxisIndex, posAxisVal);
}

//------------------------------------------------------------------------------
/**
    This emits additional recenter-events for the x and y axis if the
    axes have not been moved.
*/
void
nJoyMouseDevice::EndEmitEvents()
{
    // recenter x axis if not moved
    if ((this->numAxes > 0) && (!this->xAxisMoved))
    {
        this->RecenterAxis(0);
    }

    // recenter y axis if not moved
    if ((this->numAxes > 1) && (!this->yAxisMoved))
    {
        this->RecenterAxis(1);
    }
}

//------------------------------------------------------------------------------


