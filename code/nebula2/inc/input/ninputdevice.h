#ifndef N_INPUTDEVICE_H
#define N_INPUTDEVICE_H
//------------------------------------------------------------------------------
/**
    @class nInputDevice
    @ingroup Input

    @brief A generic Nebula input translates data from an API-specific input device
    into Nebula input events. 
    
    (C) 2002 RadonLabs GmbH
*/
#include "util/nnode.h"

//------------------------------------------------------------------------------
class nKernelServer;
class nInputServer;
class nRoot;

class nInputDevice : public nNode
{
public:
    /// constructor
    nInputDevice(nKernelServer* ks, nInputServer* is, int devNum, int numAxs, int numBtns, int numPvs);
    /// destructor
    virtual ~nInputDevice();

    /// set a input server specific source device handle
    void SetSourceDevice(void* src);
    /// get input server specific source device handle
    void* GetSourceDevice();
    /// set axis range
    void SetAxisRange(int minR, int maxR);
    /// get min axis range
    int GetMinAxisRange();
    /// get max axis range
    int GetMaxAxisRange();
    /// get device number
    int GetDeviceNum();
    /// get number of input axis
    int GetNumAxes();
    /// get number of input buttons
    int GetNumButtons();
    /// get number of POVs
    int GetNumPovs();
    /// set size of value store
    void SetStoreSize(int s);
    /// get size of value store
    int GetStoreSize();
    /// set a store value
    void SetStoreValue(int index, float val);
    /// get a store value
    float GetStoreValue(int index);

    /// export the device into the Nebula input device database
    virtual void Export(nRoot* dir);
    /// start emitting events
    virtual void BeginEmitEvents(double time);
    /// emit axis move events
    virtual void EmitAxisMovedEvents(int axisNum, int value);
    /// emit button down events
    virtual void EmitButtonEvents(int btnNum, bool pressed);
    /// emit POV event
    virtual void EmitPovMovedEvents(int povNum, int povVal);
    /// finish emitting events
    virtual void EndEmitEvents();

protected:
    /// export a device axis entry into Nebula device database
    virtual void ExportAxis(int devId, const char* chnName, int axis);
    /// export a device button entry into Nebula device database
    virtual void ExportButton(int devId, const char* chnName, int btn);
    /// emit a single axis move event
    virtual void PutAxisEvent(int devId, int axisIndex, float axisVal);
    /// emit a single button down event
    virtual void PutButtonDownEvent(int devId, int buttonIndex);
    /// emit a single button up event
    virtual void PutButtonUpEvent(int devId, int buttonIndex);
    /// emit the axis button events associated with an axis
    virtual void PutAxisButtonEvents(int devId, int buttonIndex, float oldVal, float newVal);

    nKernelServer* kernelServer;
    nInputServer* inputServer;
    double frameTime;               // the time diff to the previous frame
    void* sourceDevice;
    int deviceNum;
    int axisMinRange;
    int axisMaxRange;
    int numAxes;
    int numButtons;
    int numPovs;
    int numValues;
    float* valueStore;
};

//------------------------------------------------------------------------------
#endif


