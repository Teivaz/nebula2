#ifndef N_DI8SERVER_H
#define N_DI8SERVER_H
//------------------------------------------------------------------------------
/**
    @class nDI8Server

    A DirectInput8 based input server class.

    (C) 2002 RadonLabs GmbH
*/
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#ifndef N_INPUTSERVER_H
#include "input/ninputserver.h"
#endif

#ifndef N_DI8DEVICE_H
#include "input/ndi8device.h"
#endif

#undef N_DEFINES
#define N_DEFINES nDI8Server
#include "kernel/ndefdllclass.h"

//------------------------------------------------------------------------------
class nDI8Server : public nInputServer
{
public:
    /// constructor
    nDI8Server();
    /// destructor
    virtual ~nDI8Server();
    /// trigger the input server
    virtual void Trigger(double time);

    static nKernelServer* kernelServer;

private:
    /// initialize DInput8
    bool InitDirectInput();
    /// kill DInput8
    void KillDirectInput();
    /// called when HWND changes
    void HwndChanged();
    /// initialize DirectInput devices
    bool InitDevices();
    /// kill DirectInput devices
    void KillDevices();
    /// export the DirectInput devices into the Nebula input device database
    void ExportDevices();
    /// classify DirectInput device object data object
    void GetDeviceObjectData(nDI8Device::DeviceType devType, DIDEVICEOBJECTDATA& data, bool& isAxis, bool& isButton, bool& isPov, int& index);

    // NOTE: public because some enum function need access
public: 
    /// convert error code to string
    const char* Error(HRESULT hr);

    nAutoRef<nRoot> refDevices; // points to the Nebula input device database
    nAutoRef<nEnv> refHwnd;     // points to shared nEnv variable with window handle
    IDirectInput8* di8;         // pointer to DirectInput8 interface
    nList di8DevList;           // list of nDI8Device objects
    nList nebDevList;           // list of nInputDevice objects
    HWND hwnd;                  // current window handle

    int curJoyMouse;
    int curPadMouse;
    int curRelMouse;
    int curJoystick;
    int curKeyboard;

    enum
    {
        INPUT_BUFFER_SIZE = 128,
    };
};
//------------------------------------------------------------------------------
#endif
