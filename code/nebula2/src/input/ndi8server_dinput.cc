//------------------------------------------------------------------------------
//  ndi8server_dinput.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "input/ndi8server.h"
#include "input/ninputdevice.h"
#include "input/njoystickdevice.h"
#include "input/nrelmousedevice.h"
#include "input/nkeyboarddevice.h"
#include "kernel/nenv.h"

//------------------------------------------------------------------------------
/**
    Initialized DirectInput8 and enumerates the input devices.
*/
bool
nDI8Server::InitDirectInput()
{
    HRESULT hr;

    // initialize DirectInput
    HMODULE instance = GetModuleHandle(NULL);
    if (!instance)
    {
        n_printf("nDI8Server: GetModuleHandle() failed!\n");
        return false;
    }
    hr = DirectInput8Create(instance, DIRECTINPUT_VERSION, IID_IDirectInput8A, (void**) &(this->di8), NULL);
    if (FAILED(hr))
    {
        n_printf("nDI8Server: DirectInput8Create() failed with '%s'!\n", this->Error(hr));
        return false;
    }
    n_assert(this->di8);

    // init DirectInput devices
    if (!this->InitDevices())
    {
        n_printf("nDI8Server: Failed to initialize DInput devices!\n");
        return false;
    }

    return true;
}

//------------------------------------------------------------------------------
/**
    Destroy DirectInput.
*/
void
nDI8Server::KillDirectInput()
{
    n_assert(this->di8);

    // shutdown DirectInput devices
    this->KillDevices();

    // shutdown DirectInput
    this->di8->Release();
    this->di8 = 0;
}

//------------------------------------------------------------------------------
/**
    Set a DWORD property on a DirectInput device.
*/
static
void
di8SetDWordProp(IDirectInputDevice8* dev, REFGUID prop, DWORD val)
{
    HRESULT hr;
    DIPROPDWORD dpw;
    memset(&(dpw), 0, sizeof(dpw));
    dpw.diph.dwSize = sizeof(DIPROPDWORD);
    dpw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
    dpw.diph.dwHow = DIPH_DEVICE;
    dpw.diph.dwObj = NULL;
    dpw.dwData     = val;
    hr = dev->SetProperty(prop, (LPDIPROPHEADER) &dpw);
}

//------------------------------------------------------------------------------
/**
    Get a range property from a DirectInput device.
*/
static 
void 
di8GetRangeProp(IDirectInputDevice8* dev,
                REFGUID prop,
                DWORD obj,
                int& minRange,
                int& maxRange)
{
    HRESULT hr;
    DIPROPRANGE dpr;
    memset(&(dpr), 0, sizeof(dpr));
    dpr.diph.dwSize = sizeof(DIPROPRANGE);
    dpr.diph.dwHeaderSize = sizeof(DIPROPHEADER);
    dpr.diph.dwHow = DIPH_BYOFFSET;
    dpr.diph.dwObj = obj;
    hr = dev->GetProperty(prop,(LPDIPROPHEADER)&dpr);
    if (SUCCEEDED(hr))
    {
        minRange = dpr.lMin;
        maxRange = dpr.lMax;
    }
    else
    {
        minRange = 0;
        maxRange = 0;
    }
}

//------------------------------------------------------------------------------
/**
    Callback procedure for EnumDevices. First checks whether the device
    is acceptable to us, then creates a nDI8Device object and adds it to
    the device list.

    @param  lpddi       pointer to a DIDEVICEINSTANCE describing the input device
    @param  pvRef       pointer to nDI8Server object
    @return             DIENUM_CONTINUE continues, DIENUM_STOP stops enumeration
*/
BOOL CALLBACK
di8EnumDevicesCallback(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef)
{
    HRESULT hr;
    nDI8Server* diServer = (nDI8Server*) pvRef;
    IDirectInput8* di8 = diServer->di8;
    n_assert(di8);

    // convert device type into general Nebula device type
    DWORD devType = lpddi->dwDevType & 0xff;
    nDI8Device::DeviceType type;
    switch (devType)
    {
        case DI8DEVTYPE_GAMEPAD:
        case DI8DEVTYPE_JOYSTICK:
            type  = nDI8Device::JOYSTICK;
            break;

        case DI8DEVTYPE_KEYBOARD:
            type = nDI8Device::KEYBOARD;
            break;

        case DI8DEVTYPE_MOUSE:
            type = nDI8Device::MOUSE;
            break;

        default:
            type  = nDI8Device::NONE;
            break;
    }

    // ignore if device type not supported
    if (nDI8Device::NONE == type)
    {
        return DIENUM_CONTINUE;
    }

    // create DirectInput device
    IDirectInputDevice8* diDev = 0;
    hr = di8->CreateDevice(lpddi->guidInstance, &diDev, 0);
    if (FAILED(hr))
    {
        n_printf("nDI8Server: failed to create device '%s' with '%d'\n", 
                 lpddi->tszInstanceName, diServer->Error(hr));
        return DIENUM_CONTINUE;
    }
    n_assert(diDev);

    // get device caps
    DIDEVCAPS caps = { sizeof(DIDEVCAPS), 0 };
    hr = diDev->GetCapabilities(&caps);
    if (FAILED(hr))
    {
        n_printf("nDI8Server: GetCapabilities() failed!\n");
        diDev->Release();
        return DIENUM_CONTINUE;
    }

    // create nDI8Device object and link to device list
    nDI8Device* dev = n_new(nDI8Device);
    dev->SetDevice(diDev);
    dev->SetInstanceName(lpddi->tszInstanceName);
    dev->SetProductName(lpddi->tszProductName);
    dev->SetDeviceType(type);
    dev->SetNumAxes(caps.dwAxes);
    dev->SetNumButtons(caps.dwButtons);
    dev->SetNumPovs(caps.dwPOVs);
    diServer->di8DevList.AddTail(dev);

    // initialize the device
    HWND hwnd = (HWND) diServer->refHwnd->GetI();
    n_assert2(hwnd, "/sys/env/hwnd is NULL -- perhaps nWin32WindowHandler::OpenWindow() has not been called.")
    hr = diDev->SetCooperativeLevel(hwnd, (DISCL_FOREGROUND | DISCL_NOWINKEY | DISCL_NONEXCLUSIVE));
    if (FAILED(hr))
    {
        n_printf("SetCooperativeLevel() failed on '%s' with '%s'\n", lpddi->tszInstanceName, diServer->Error(hr));
    }

    // set data format on DirectInput device
    switch(type)
    {
        int minRange, maxRange;

        case nDI8Device::JOYSTICK:
            // set data format and axis properties
            diDev->SetDataFormat(&c_dfDIJoystick);
            di8SetDWordProp(diDev, DIPROP_BUFFERSIZE, nDI8Server::INPUT_BUFFER_SIZE);
            di8SetDWordProp(diDev, DIPROP_AXISMODE,   DIPROPAXISMODE_ABS);
            di8SetDWordProp(diDev, DIPROP_DEADZONE,   250);
            di8SetDWordProp(diDev, DIPROP_SATURATION, 9999);
            
            // get the range and store in the device object
            di8GetRangeProp(diDev, DIPROP_RANGE, DIJOFS_X, minRange, maxRange);
            dev->SetRange(minRange, maxRange);
            break;

        case nDI8Device::KEYBOARD:
            diDev->SetDataFormat(&c_dfDIKeyboard);
            break;

        case nDI8Device::MOUSE:
            // set data format and axis properties
            diDev->SetDataFormat(&c_dfDIMouse2);
            di8SetDWordProp(diDev, DIPROP_BUFFERSIZE, nDI8Server::INPUT_BUFFER_SIZE); 
            di8SetDWordProp(diDev, DIPROP_AXISMODE, DIPROPAXISMODE_REL);
            dev->SetRange(-50, +50);
            break;

        default:
            break;
    }

    // acquire the device
    diDev->Acquire();

    // continue enumeration...
    return DIENUM_CONTINUE;
}

//------------------------------------------------------------------------------
/**
    Notify the DirectInput devices that the HWND has changed. Called
    by Trigger() when window handle has changed since last frame.
*/
void
nDI8Server::HwndChanged()
{
    HRESULT hr;
    nDI8Device* dev;
    for (dev = (nDI8Device*) this->di8DevList.GetHead();
         dev;
         dev = (nDI8Device*) dev->GetSucc())
    {
        IDirectInputDevice8* diDev = dev->GetDevice();
        n_assert(diDev);
        diDev->Unacquire();
        hr = diDev->SetCooperativeLevel(this->hwnd, (DISCL_FOREGROUND|DISCL_NOWINKEY|DISCL_NONEXCLUSIVE));
        if (FAILED(hr))
        {
            n_printf("SetCooperativeLevel() failed on '%s' with '%s'\n", 
                dev->GetInstanceName(), 
                this->Error(hr));
        }
    }
}

//------------------------------------------------------------------------------
/**
    Enumerate supported DirectInput devices, create a nDI8Device object for each
    DirectInput device.
*/
bool
nDI8Server::InitDevices()
{
    n_assert(this->di8);
    HRESULT hr;

    // enumerate devices
    hr = this->di8->EnumDevices(DI8DEVCLASS_ALL, di8EnumDevicesCallback, (LPVOID) this, DIEDFL_ATTACHEDONLY);
    if (FAILED(hr))
    {
        n_printf("nDI8Server: EnumDevices() failed with '%s'\n", this->Error(hr));
        return false;
    }
    
    // export devices into Nebula's input device database
    this->ExportDevices();
    return true;
}

//------------------------------------------------------------------------------
/**
    Kill the devices in the internal device lists   
*/
void
nDI8Server::KillDevices()
{
    // kill nDI8Device's
    nDI8Device* di8Dev;
    while (di8Dev = (nDI8Device*) this->di8DevList.RemHead())
    {
        n_delete(di8Dev);
    }

    // kill Nebula input device objects
    nInputDevice* dev;
    while (dev = (nInputDevice*) this->nebDevList.RemHead())
    {
        n_delete(dev);
    }
}

//------------------------------------------------------------------------------
/**
    Export recognized devices into the Nebula input device database. This
    creates nInputDevice objects, configures them, and asks them to export
    themselves.
*/
void
nDI8Server::ExportDevices()
{
    this->curJoyMouse = 0;
    this->curPadMouse = 0;
    this->curJoystick = 0;
    this->curKeyboard = 0;

    // scan di8 device list
    nDI8Device* di8Dev;
    for (di8Dev = (nDI8Device*) this->di8DevList.GetHead();
         di8Dev;
         di8Dev = (nDI8Device*) di8Dev->GetSucc())
    {
        // get hardware's number of axes, buttons and povs
        int numAxes    = di8Dev->GetNumAxes();
        int numButtons = di8Dev->GetNumButtons();
        int numPovs    = di8Dev->GetNumPovs();

        // create a nInputDevice object
        switch (di8Dev->GetDeviceType())
        {
            case nDI8Device::MOUSE:
                {
                    // generate a relmouse device object
                    nInputDevice* relMouse = n_new(nRelMouseDevice(
                        kernelServer,
                        this,
                        this->curRelMouse++,
                        numAxes,
                        numButtons,
                        0));
                    relMouse->SetSourceDevice(di8Dev);
                    relMouse->SetAxisRange(di8Dev->GetMinRange(), di8Dev->GetMaxRange());
                    relMouse->Export(this->refDevices.get());
                    this->nebDevList.AddTail(relMouse);
                }
                break;

            case nDI8Device::JOYSTICK:
                {
                    nInputDevice* joystick = n_new(nJoystickDevice(
                        kernelServer, 
                        this, 
                        this->curJoystick++,
                        numAxes,
                        numButtons,
                        numPovs));
                    joystick->SetSourceDevice(di8Dev);
                    joystick->SetAxisRange(di8Dev->GetMinRange(), di8Dev->GetMaxRange());
                    joystick->Export(this->refDevices.get());
                    this->nebDevList.AddTail(joystick);
                }
                break;

            case nDI8Device::KEYBOARD:
                {
                    nInputDevice* keyboard = n_new(nKeyboardDevice(
                        kernelServer,
                        this,
                        this->curKeyboard++,
                        0,
                        numButtons,
                        0));
                    keyboard->SetSourceDevice(di8Dev);
                    keyboard->Export(this->refDevices.get());
                    this->nebDevList.AddTail(keyboard);
                }
                break;
        }
    }
}

//------------------------------------------------------------------------------


