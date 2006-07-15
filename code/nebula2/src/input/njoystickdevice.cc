//------------------------------------------------------------------------------
//  njoystickdevice.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "input/njoystickdevice.h"
#include "input/ninputserver.h"

//------------------------------------------------------------------------------
/**
*/
nJoystickDevice::nJoystickDevice(nKernelServer* ks, nInputServer* is, int devNum, int numAxs, int numBtns, int numPvs) :
    nInputDevice(ks, is, devNum, numAxs, numBtns,numPvs)
{
    // empty
    if ((this->numAxes > 0) || (this->numPovs > 0))
    {
        this->SetStoreSize((this->numAxes * 2) + (this->numPovs * 4));
    }

    // initialize axis filter 
    int i;
    for (i = 0; i < NUMFILTEREDAXIS; i++)
    {
        this->axisMoved[i] = false;
        this->filterArray[i].SetSize(3);
    }
}

//------------------------------------------------------------------------------
/**
*/
nJoystickDevice::~nJoystickDevice()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Export device description into Nebula device database.

    @param  dir     points to root directory of device database
*/
void
nJoystickDevice::Export(nRoot* dir)
{
    n_assert(dir);
    int devId = N_INPUT_JOYSTICK(this->deviceNum);

    kernelServer->PushCwd(dir);

    // create the device entry
    char devName[N_MAXNAMELEN];
    sprintf(devName, "joy%d", this->deviceNum);
    nRoot* devEntry = kernelServer->New("nroot", devName);

    // write the input channels
    kernelServer->PushCwd(devEntry);
    nRoot* devChannels = kernelServer->New("nroot", "channels");
    n_assert(devChannels);
    kernelServer->PushCwd(devChannels);

    // for each axis, export 2 Nebula axis (positive and negative)
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

    // export optional POV hat as 2 additional axis
    int curPov;
    for (curPov = 0; curPov < this->numPovs; curPov++)
    {
        const char* xName;
        const char* yName;
        char chnName[N_MAXNAMELEN];
        switch (curPov)
        {
            case 0:
                xName = "a";
                yName = "b";
                break;

            case 1:
                xName = "c";
                yName = "d";
                break;

            case 2:
                xName = "e";
                yName = "f";
                break;

            default: continue;
        }

        int axisOffset = (2 * numAxes) + (4 * curPov);
        int btnOffset  = numButtons + axisOffset;

        // export negative horizontal axis
        sprintf(chnName, "-%s", xName);
        this->ExportAxis(devId, chnName, axisOffset + 0);

        // export positive horizontal axis
        sprintf(chnName, "+%s", xName);
        this->ExportAxis(devId, chnName, axisOffset + 1);

        // export negative  vertical axis
        sprintf(chnName, "-%s", yName);
        this->ExportAxis(devId, chnName, axisOffset + 2);

        // export positive vertical  axis
        sprintf(chnName, "+%s", yName);
        this->ExportAxis(devId, chnName, axisOffset + 3);

        // export negative axis button
        sprintf(chnName, "-%sbtn", xName);
        this->ExportButton(devId, chnName, btnOffset + 0);

        // export positive axis button
        sprintf(chnName, "+%sbtn", xName);
        this->ExportButton(devId, chnName, btnOffset + 1);

        // export negative axis button
        sprintf(chnName, "-%sbtn", yName);
        this->ExportButton(devId, chnName, btnOffset + 2);

        // export positive axis button
        sprintf(chnName, "+%sbtn", yName);
        this->ExportButton(devId, chnName, btnOffset + 3);
    }
    kernelServer->PopCwd();        
    kernelServer->PopCwd();
    kernelServer->PopCwd();
}

//------------------------------------------------------------------------------
/**
*/
void
nJoystickDevice::BeginEmitEvents(double time)
{
    int i;
    for (i = 0; i < NUMFILTEREDAXIS; i++)
    {
        this->axisMoved[i] = false;
    }
    nInputDevice::BeginEmitEvents(time);
}

//------------------------------------------------------------------------------
/**
    Split an axis index and axis value into positive and negative 
    indices/values and emit input events.
*/
void
nJoystickDevice::ProcessAnalogAxis(int axisNum, float val)
{
    int devId = N_INPUT_JOYSTICK(this->deviceNum);

    // split index and value into positive/negative axis
    int negAxisIndex = (axisNum * 2) + 0;
    int posAxisIndex = (axisNum * 2) + 1;
    float negAxisVal = (val < 0.0f) ? -val : 0.0f;
    float posAxisVal = (val > 0.0f) ? +val : 0.0f;

    // let the resulting values grow non-linearly
    negAxisVal *= negAxisVal;
    posAxisVal *= posAxisVal;

    n_assert(negAxisIndex < this->numValues);
    n_assert(posAxisIndex < this->numValues);
    
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
    Emit Nebula input events associated with a hardware axis movement.

    @param  axisNum     hardware axis number
    @param  value       new hardware axis value
*/
void
nJoystickDevice::EmitAxisMovedEvents(int axisNum, int value)
{
    n_assert((axisNum >= 0) && (axisNum < this->numAxes));

    // relative axis value from -1.0f .. +1.0f
    float minRange = (float) this->axisMinRange;
    float maxRange = (float) this->axisMaxRange;
    float fValue   = (float) value;
    float val = (((fValue - minRange) / (maxRange - minRange)) * 2.0f) - 1.0f;
    if ((val > -0.01f) && (val < 0.01f))
    {
        val = 0.0f;
    }

    // filter value?
    if (axisNum < NUMFILTEREDAXIS)
    {
        this->axisMoved[axisNum] = true;
        this->filterArray[axisNum].PushValue(val);
        val = this->filterArray[axisNum].GetFilteredValue();
    }
    this->ProcessAnalogAxis(axisNum, val);
}

//------------------------------------------------------------------------------
/**
    Emit a button input event.
*/
void
nJoystickDevice::EmitButtonEvents(int btnNum, bool pressed)
{
    n_assert((btnNum >= 0) && (btnNum < this->numButtons));

    if (pressed)
    {
        this->PutButtonDownEvent(N_INPUT_JOYSTICK(this->deviceNum), btnNum);
    }
    else
    {
        this->PutButtonUpEvent(N_INPUT_JOYSTICK(this->deviceNum), btnNum);
    }
}

//------------------------------------------------------------------------------
/**
    Emit POV input events.

    @param  povNum      hardware POV number
    @param  povVal      POV positional code (see DirectInput POV docs)
*/
void
nJoystickDevice::EmitPovMovedEvents(int povNum, int povVal)
{
    n_assert((povNum >= 0) && (povNum < this->numPovs));

    int devId = N_INPUT_JOYSTICK(this->deviceNum);

    // every pov event generates 4 axis input events
    int axisOffset = (2 * this->numAxes) + (4 * povNum);

    int horiNegAxisIndex = axisOffset + 0;
    int horiPosAxisIndex = axisOffset + 1;
    int vertNegAxisIndex = axisOffset + 2;
    int vertPosAxisIndex = axisOffset + 3;

    // check for centered position
    float horiVal, vertVal;
    if ((povVal & 0xffff) == 0xffff)
    {
        horiVal = 0.0f;
        vertVal = 0.0f;
    }
    else
    {
        if ((povVal > 33750) || (povVal <= 2250))
        {
            // north
            horiVal = 0.0f;
            vertVal = -1.0f;
        }
        else if ((povVal > 2250) && (povVal <= 6750))
        {
            // northeast
            horiVal = 1.0f;
            vertVal = -1.0f;
        }
        else if ((povVal > 6750) && (povVal <= 11250))
        {
            // east
            horiVal = 1.0f;
            vertVal = 0.0f;
        }
        else if ((povVal > 11250) && (povVal <= 15750))
        {
            // southeast
            horiVal = 1.0f;
            vertVal = 1.0f;
        }
        else if ((povVal > 15750) && (povVal <= 20250))
        {
            // south
            horiVal = 0.0f;
            vertVal = 1.0f;
        }
        else if ((povVal > 20250) && (povVal <= 24750))
        {
            // southwest
            horiVal = -1.0f;
            vertVal = 1.0f;
        }
        else if ((povVal > 24750) && (povVal <= 29250))
        {
            // west
            horiVal = -1.0f;
            vertVal = 0.0f;
        }
        else 
        {
            // northwest
            horiVal = -1.0f;
            vertVal = -1.0f;
        }
    }

    // generate the axis input events
    float horiNegAxisVal = (horiVal < 0.0f) ? -horiVal : 0.0f;
    float horiPosAxisVal = (horiVal > 0.0f) ? +horiVal : 0.0f;
    float vertNegAxisVal = (vertVal < 0.0f) ? -vertVal : 0.0f;
    float vertPosAxisVal = (vertVal > 0.0f) ? +vertVal : 0.0f;

    // horizontal negative/positive axis event
    this->PutAxisEvent(devId, horiNegAxisIndex, horiNegAxisVal);
    this->PutAxisEvent(devId, horiPosAxisIndex, horiPosAxisVal);

    // vertical negative axis event
    this->PutAxisEvent(devId, vertNegAxisIndex, vertNegAxisVal);
    this->PutAxisEvent(devId, vertPosAxisIndex, vertPosAxisVal);

    // generate accompanying axis button events
    n_assert(horiNegAxisIndex < this->numValues);
    n_assert(horiPosAxisIndex < this->numValues);
    n_assert(vertNegAxisIndex < this->numValues);
    n_assert(vertPosAxisIndex < this->numValues);

    float oldHoriNegAxisVal = this->GetStoreValue(horiNegAxisIndex);
    float oldHoriPosAxisVal = this->GetStoreValue(horiPosAxisIndex);
    float oldVertNegAxisVal = this->GetStoreValue(vertNegAxisIndex);
    float oldVertPosAxisVal = this->GetStoreValue(vertPosAxisIndex);
    
    this->PutAxisButtonEvents(devId, this->numButtons + horiNegAxisIndex, oldHoriNegAxisVal, horiNegAxisVal);
    this->PutAxisButtonEvents(devId, this->numButtons + horiPosAxisIndex, oldHoriPosAxisVal, horiPosAxisVal);
    this->PutAxisButtonEvents(devId, this->numButtons + vertNegAxisIndex, oldVertNegAxisVal, vertNegAxisVal);
    this->PutAxisButtonEvents(devId, this->numButtons + vertPosAxisIndex, oldVertPosAxisVal, vertPosAxisVal);
    
    this->SetStoreValue(horiNegAxisIndex, horiNegAxisVal);
    this->SetStoreValue(horiPosAxisIndex, horiPosAxisVal);
    this->SetStoreValue(vertNegAxisIndex, vertNegAxisVal);
    this->SetStoreValue(vertPosAxisIndex, vertPosAxisVal);
}

//------------------------------------------------------------------------------
/**
    Fill the axis filter with the current axis value 
*/
void
nJoystickDevice::EndEmitEvents()
{
    int num = (this->numAxes < NUMFILTEREDAXIS) ? this->numAxes : NUMFILTEREDAXIS;
    int axisNum;
    for (axisNum = 0; axisNum < num; axisNum++)
    {
        // replicate the current value
        float curValue = this->filterArray[axisNum].GetRawValue(0);
        this->filterArray[axisNum].PushValue(curValue);
        float val = this->filterArray[axisNum].GetFilteredValue();

        this->ProcessAnalogAxis(axisNum, val);
    }
}

//------------------------------------------------------------------------------
