//------------------------------------------------------------------------------
//  ndi8server_trigger.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "input/ndi8server.h"
#include "input/ninputdevice.h"
#include "kernel/nenv.h"

//------------------------------------------------------------------------------
/**
    Take a device type and a DIDEVICEOBJECTDATA element as received by
    GetDeviceData(), and decide whether this element is an axis, button or
    pov, and return the number of the axis, button or pov.

    @param      devType     [in] the device type
    @param      devData     [in] the device object data element from GetDeviceData()
    @param      isAxis      [out] true, if an axis
    @param      isButton    [out] true, if a button
    @param      isPov       [out] true, if a pov
    @param      index       [out] axis, button or pov index
*/
void
nDI8Server::GetDeviceObjectData(nDI8Device::DeviceType devType,
                                DIDEVICEOBJECTDATA& data,
                                bool& isAxis, bool& isButton, bool& isPov,
                                int& index)
{
    isAxis   = false;
    isButton = false;
    isPov    = false;
    index = 0;

    // get input event type and element number
    switch (devType)
    {
        case nDI8Device::MOUSE:
            switch (data.dwOfs)
            {
                case DIMOFS_X:
                    isAxis = true; index = 0;
                    break;

                case DIMOFS_Y:
                    isAxis = true; index = 1;
                    break;

                case DIMOFS_Z:
                    isAxis = true; index = 2;
                    break;

                case DIMOFS_BUTTON0:
                case DIMOFS_BUTTON1:
                case DIMOFS_BUTTON2:
                case DIMOFS_BUTTON3:
                case DIMOFS_BUTTON4:
                case DIMOFS_BUTTON5:
                case DIMOFS_BUTTON6:
                case DIMOFS_BUTTON7:
                    isButton = true; index = data.dwOfs - DIMOFS_BUTTON0;
                    break;

                default:
                    break;
            }
            break;

        case nDI8Device::JOYSTICK:
            switch (data.dwOfs)
            {
                case DIJOFS_X:
                    isAxis = true; index = 0;
                    break;

                case DIJOFS_Y:
                    isAxis = true; index = 1;
                    break;

                case DIJOFS_Z:
                    isAxis = true; index = 2;
                    break;

                case DIJOFS_RZ:
                    isAxis = true; index = 3;
                    break;

                case DIJOFS_BUTTON0:
                case DIJOFS_BUTTON1:
                case DIJOFS_BUTTON2:
                case DIJOFS_BUTTON3:
                case DIJOFS_BUTTON4:
                case DIJOFS_BUTTON5:
                case DIJOFS_BUTTON6:
                case DIJOFS_BUTTON7:
                case DIJOFS_BUTTON8:
                case DIJOFS_BUTTON9:
                case DIJOFS_BUTTON10:
                case DIJOFS_BUTTON11:
                case DIJOFS_BUTTON12:
                case DIJOFS_BUTTON13:
                case DIJOFS_BUTTON14:
                case DIJOFS_BUTTON15:
                    isButton = true; index = data.dwOfs - DIJOFS_BUTTON0;
                    break;

                case DIJOFS_POV(0):
                    isPov = true; index = 0;
                    break;

                case DIJOFS_POV(1):
                    isPov = true; index = 1;
                    break;

                case DIJOFS_POV(2):
                    isPov = true; index = 2;
                    break;

                case DIJOFS_POV(3):
                    isPov = true; index = 3;
                    break;

                default:
                    break;
            }
            break;

        case nDI8Device::KEYBOARD:
            isButton = true; index = data.dwOfs - DIK_ESCAPE;
            break;
    }
}

//------------------------------------------------------------------------------
/**
    The servers trigger method which should be called once a frame.
    Queries input devices and generates Nebula input events which are
    linked into the global input event list.
*/
void
nDI8Server::Trigger(double time)
{
    HRESULT hr;

    // compute frame time
    float frameTime = (float) (time - this->timeStamp);
    if ((frameTime <= 0.0001f) || (frameTime > 2.0f))
    {
        frameTime = 0.0001f;
    }

    // see if hwnd has changed since last frame
    HWND curHwnd = (HWND) this->refHwnd->GetI();
    if (curHwnd != this->hwnd)
    {
        this->hwnd = curHwnd;
        this->HwndChanged();
    }

    // prepare Nebula input device objects for event emission
    nInputDevice* nebDev;
    for (nebDev = (nInputDevice*) this->nebDevList.GetHead();
         nebDev;
         nebDev = (nInputDevice*) nebDev->GetSucc())
    {
        nebDev->BeginEmitEvents(frameTime);
    }

    // query devices and generate Nebula input events...
    nDI8Device* di8Dev;
    for (di8Dev = (nDI8Device*) this->di8DevList.GetHead();
         di8Dev;
         di8Dev = (nDI8Device*) di8Dev->GetSucc())
    {
        IDirectInputDevice8* diDev = di8Dev->GetDevice();
        nDI8Device::DeviceType devType = di8Dev->GetDeviceType();

        // make sure the device is acquired
        hr = diDev->Acquire();
        if ((DI_OK == hr) || (S_FALSE == hr))
        {
            DIDEVICEOBJECTDATA objData[INPUT_BUFFER_SIZE];
            DWORD numData = INPUT_BUFFER_SIZE;

            // poll device and get buffered input data
            hr = diDev->Poll();
            hr = diDev->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), objData, &numData, 0);
            if (FAILED(hr))
            {
                // if failed to get device data, continue silently with next device
                continue;
            }

            // for each Nebula input device which is fed by this DirectInput device...
            for (nebDev = (nInputDevice*) this->nebDevList.GetHead();
                 nebDev;
                 nebDev = (nInputDevice*) nebDev->GetSucc())
            {
                if (nebDev->GetSourceDevice() != di8Dev)
                {
                    continue;
                }

                // for each device data element
                int i;
                for (i = 0; i < (int) numData; i++)
                {
                    bool isAxis, isButton, isPov;
                    int index;

                    // classify current device object data element
                    this->GetDeviceObjectData(devType, objData[i], isAxis, isButton, isPov, index);

                    // generate input events
                    if (isAxis)
                    {
                        nebDev->EmitAxisMovedEvents(index, objData[i].dwData);
                    }
                    else if (isButton)
                    {
                        nebDev->EmitButtonEvents(index, objData[i].dwData & 0x80 ? true : false);
                    }
                    else if (isPov)
                    {
                        nebDev->EmitPovMovedEvents(index, objData[i].dwData);
                    }
                }
            }
        }
    }

    // tell Nebula input devices that event emission is finished for this frame
    for (nebDev = (nInputDevice*) this->nebDevList.GetHead();
         nebDev;
         nebDev = (nInputDevice*) nebDev->GetSucc())
    {
        nebDev->EndEmitEvents();
    }

    // invoke parent class Trigger, which maps input events to input states
    nInputServer::Trigger(time);
}

//------------------------------------------------------------------------------
