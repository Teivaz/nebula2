//------------------------------------------------------------------------------
//  nrelmousedevice.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "input/nrelmousedevice.h"
#include "input/ninputserver.h"

//------------------------------------------------------------------------------
/**
*/
nRelMouseDevice::nRelMouseDevice(nKernelServer* ks, nInputServer* is, int devNum, int numAxs, int numBtns, int numPvs) :
    nInputDevice(ks, is, devNum, numAxs, numBtns,numPvs),
    xAxisMoved(false),
    yAxisMoved(false)
{
    // reserve storage size
    if (this->numAxes > 0)
    {
        this->SetStoreSize(this->numAxes * 2);
    }

    // initialize axis filter 
    int i;
    for (i = 0; i < NUMFILTEREDAXIS; i++)
    {
        this->filterArray[i].SetSize(5);
    }
}

//------------------------------------------------------------------------------
/**
*/
nRelMouseDevice::~nRelMouseDevice()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Export device description into Nebula device database.

    @param  dir     points to root directory of device database
*/
void
nRelMouseDevice::Export(nRoot* dir)
{
    n_assert(dir);

    int devId = N_INPUT_RELMOUSE(this->deviceNum);

    kernelServer->PushCwd(dir);

    // create the device entry
    char devName[N_MAXNAMELEN];
    sprintf(devName, "relmouse%d", this->deviceNum);
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
*/
void
nRelMouseDevice::BeginEmitEvents(double time)
{
    this->xAxisMoved = false;
    this->yAxisMoved = false;
    nInputDevice::BeginEmitEvents(time);
}

//------------------------------------------------------------------------------
/**
*/
void
nRelMouseDevice::EmitAxisMovedEvents(int axisNum, int value)
{
    int devId = N_INPUT_RELMOUSE(this->deviceNum);
    
    // touch the axis moved flags
    if (0 == axisNum)
    {
        this->xAxisMoved = true;
    }
    else if (1 == axisNum)
    {
        this->yAxisMoved = true;
    }

    float val = float(value) * 0.1f;
    int negAxisIndex = (axisNum * 2) + 0;
    int posAxisIndex = (axisNum * 2) + 1;

    // filter value?
    if (axisNum < NUMFILTEREDAXIS)
    {
        this->filterArray[axisNum].PushValue(val);
        val = this->filterArray[axisNum].GetFilteredValue();
    }

    // special case 3rd axis (the mouse wheel, which only generates
    // button events
    if (2 == axisNum)
    {
        if (value > 0)
        {
            // wheel has been moved into the positive direction
            int buttonIndex = this->numButtons + posAxisIndex;
            this->PutButtonDownEvent(devId, buttonIndex);
            this->PutButtonUpEvent(devId, buttonIndex);
        }
        else
        {
            // wheel has been moved into the negative direction
            int buttonIndex = this->numButtons + negAxisIndex;
            this->PutButtonDownEvent(devId, buttonIndex);
            this->PutButtonUpEvent(devId, buttonIndex);
        }
    }
    else
    {
        // a normal axis
        float mouseFactor = this->inputServer->GetMouseFactor();
        bool mouseInvert  = this->inputServer->GetMouseInvert();

        // invert vertical axis?
        if (mouseInvert && (axisNum == 1))
        {
            val = -val;
        }

        float negAxisVal = mouseFactor * ((val < 0.0f) ? -val : 0.0f);
        float posAxisVal = mouseFactor * ((val > 0.0f) ? +val : 0.0f);

        // generate positive/negative axis move events
        this->PutAxisEvent(devId, negAxisIndex, negAxisVal);
        this->PutAxisEvent(devId, posAxisIndex, posAxisVal);

        // generate additional axis button events
        float oldNegAxisVal = this->GetStoreValue(negAxisIndex);
        float oldPosAxisVal = this->GetStoreValue(posAxisIndex);

        this->PutAxisButtonEvents(devId, this->numButtons + negAxisIndex, oldNegAxisVal, negAxisVal);
        this->PutAxisButtonEvents(devId, this->numButtons + posAxisIndex, oldPosAxisVal, posAxisVal);

        this->SetStoreValue(negAxisIndex, negAxisVal);
        this->SetStoreValue(posAxisIndex, posAxisVal);
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
nRelMouseDevice::EmitButtonEvents(int btnNum, bool pressed)
{
    n_assert((btnNum >= 0) && (btnNum < this->numButtons));

    if (pressed)
    {
        this->PutButtonDownEvent(N_INPUT_RELMOUSE(this->deviceNum), btnNum);
    }
    else
    {
        this->PutButtonUpEvent(N_INPUT_RELMOUSE(this->deviceNum), btnNum);
    }
}

//------------------------------------------------------------------------------
/**
    Reset an axis if it has not moved this frame.
*/
void
nRelMouseDevice::ResetAxis(int axisNum)
{
    int devId = N_INPUT_RELMOUSE(this->deviceNum);

    // split index and value into positive/negative axis
    int negAxisIndex = (axisNum * 2) + 0;
    int posAxisIndex = (axisNum * 2) + 1;

    float oldNegAxisVal = this->GetStoreValue(negAxisIndex);
    float oldPosAxisVal = this->GetStoreValue(posAxisIndex);
    
    // generate positive/negative axis move events
    this->PutAxisEvent(devId, negAxisIndex, 0.0f);
    this->PutAxisEvent(devId, posAxisIndex, 0.0f);

    // generate additional axis button events
    this->PutAxisButtonEvents(devId, this->numButtons + negAxisIndex, oldNegAxisVal, 0.0f);
    this->PutAxisButtonEvents(devId, this->numButtons + posAxisIndex, oldPosAxisVal, 0.0f);

    this->SetStoreValue(negAxisIndex, 0.0f);
    this->SetStoreValue(posAxisIndex, 0.0f);
}

//------------------------------------------------------------------------------
/**
    Emit zero events if axis not moved.
*/
void
nRelMouseDevice::EndEmitEvents()
{
    if ((this->numAxes > 0) && (!this->xAxisMoved))
    {
        this->filterArray[0].PushValue(0.0f);
        this->ResetAxis(0);
    }

    if ((this->numAxes > 1) && (!this->yAxisMoved))
    {
        this->filterArray[1].PushValue(0.0f);
        this->ResetAxis(1);
    }
}

//------------------------------------------------------------------------------
