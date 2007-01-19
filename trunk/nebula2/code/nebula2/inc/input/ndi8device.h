#ifndef N_DI8DEVICE_H
#define N_DI8DEVICE_H
//------------------------------------------------------------------------------
/**
    @class nDI8Device
    @ingroup Input

    @brief Hold information about a DirectInput8 device. There is one nDI8Device
    object per supported DirectInput device.

    (C) 2002 RadonLabs GmbH
*/
#include "util/nnode.h"
#include "util/nstring.h"

//------------------------------------------------------------------------------
class nDI8Device : public nNode
{
public:
    /// device types
    enum DeviceType
    {
        NONE,
        KEYBOARD,           ///< a keyboard
        MOUSE,              ///< a mouse
        JOYSTICK,           ///< a joystick OR gamepad
    };

    /// constructor
    nDI8Device();
    /// destructor
    ~nDI8Device();
    /// set pointer to DirectInput device
    void SetDevice(IDirectInputDevice8* dev);
    /// get pointer to DirectInput device
    IDirectInputDevice8* GetDevice();
    /// set instance name
    void SetInstanceName(const char* name);
    /// get instance name
    const char* GetInstanceName();
    /// set product name
    void SetProductName(const char* name);
    /// get product name
    const char* GetProductName();
    /// set device type
    void SetDeviceType(DeviceType type);
    /// get device type
    DeviceType GetDeviceType();
    /// set range of device
    void SetRange(int mini, int maxi);
    /// get min range value of device
    int GetMinRange();
    /// get max range value of device
    int GetMaxRange();
    /// set number of axes
    void SetNumAxes(int num);
    /// get number of axes
    int GetNumAxes();
    /// set number of buttons
    void SetNumButtons(int num);
    /// get number of buttons
    int GetNumButtons();
    /// set number of POVs
    void SetNumPovs(int num);
    /// get number of POVs
    int GetNumPovs();

private:
    IDirectInputDevice8* device;
    nString instanceName;
    nString productName;
    DeviceType deviceType;
    int minRange;
    int maxRange;
    int numAxes;
    int numButtons;
    int numPovs;
    int numValues;
    float* valueStore;
    bool hasMoved;
};

//------------------------------------------------------------------------------
/**
*/
inline
nDI8Device::nDI8Device() :
    device(0),
    deviceType(NONE),
    minRange(0),
    maxRange(0),
    numAxes(0),
    numButtons(0),
    numPovs(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nDI8Device::~nDI8Device()
{
    if (this->device)
    {
        this->device->Unacquire();
        this->device->Release();
        this->device = 0;
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nDI8Device::SetInstanceName(const char* name)
{
    n_assert(name);
    this->instanceName = name;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nDI8Device::GetInstanceName()
{
    if (this->instanceName.IsEmpty())
    {
        return 0;
    }
    return this->instanceName.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nDI8Device::SetProductName(const char* name)
{
    n_assert(name);
    this->productName = name;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nDI8Device::GetProductName()
{
    if (this->productName.IsEmpty())
    {
        return 0;
    }
    return this->productName.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nDI8Device::SetDeviceType(nDI8Device::DeviceType type)
{
    this->deviceType = type;
}

//------------------------------------------------------------------------------
/**
*/
inline
nDI8Device::DeviceType
nDI8Device::GetDeviceType()
{
    return this->deviceType;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nDI8Device::SetDevice(IDirectInputDevice8* dev)
{
    n_assert(dev);
    this->device = dev;
}

//------------------------------------------------------------------------------
/**
*/
inline
IDirectInputDevice8*
nDI8Device::GetDevice()
{
    return this->device;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nDI8Device::SetRange(int mini, int maxi)
{
    this->minRange = mini;
    this->maxRange = maxi;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nDI8Device::GetMinRange()
{
    return this->minRange;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nDI8Device::GetMaxRange()
{
    return this->maxRange;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nDI8Device::SetNumAxes(int num)
{
    n_assert(num >= 0);
    this->numAxes = num;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nDI8Device::GetNumAxes()
{
    return this->numAxes;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nDI8Device::SetNumButtons(int num)
{
    n_assert(num >= 0);
    this->numButtons = num;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nDI8Device::GetNumButtons()
{
    return this->numButtons;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nDI8Device::SetNumPovs(int num)
{
    n_assert(num >= 0);
    this->numPovs = num;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nDI8Device::GetNumPovs()
{
    return this->numPovs;
}

//------------------------------------------------------------------------------
#endif

