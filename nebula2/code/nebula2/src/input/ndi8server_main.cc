#define N_IMPLEMENTS nDI8Server
//------------------------------------------------------------------------------
//  ndi8server_main.cc
//  (C) 2001 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "input/ndi8server.h"

nNebulaClass(nDI8Server, "ninputserver");

//------------------------------------------------------------------------------
/**
*/
nDI8Server::nDI8Server() :
    refDevices(kernelServer),
    refHwnd(kernelServer),
    di8(0),
    hwnd(0),
    curJoyMouse(0),
    curPadMouse(0),
    curRelMouse(0),
    curJoystick(0),
    curKeyboard(0)
{
    this->refDevices = "/sys/share/input/devs";
    this->refHwnd = "/sys/env/hwnd";
    if (!this->InitDirectInput())
    {
        n_error("nDI8Server: Could not initialize DirectInput8!\n");
    }
}

//------------------------------------------------------------------------------
/**
*/
nDI8Server::~nDI8Server()
{
    this->KillDirectInput();
}

//------------------------------------------------------------------------------
