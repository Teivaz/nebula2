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
    refDevices("/sys/share/input/devs"),
    refHwnd("/sys/env/hwnd"),
    di8(0),
    hwnd(0),
    curJoyMouse(0),
    curPadMouse(0),
    curRelMouse(0),
    curJoystick(0),
    curKeyboard(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nDI8Server::~nDI8Server()
{
    this->KillDirectInput();
}

//------------------------------------------------------------------------------
/**
    InitDirectInput is not done in the constructor anymore. When using Mangalore, a window hasn't been created
    yet at that point.
*/
void
nDI8Server::Open()
{
    nInputServer::Open();

    if (!this->InitDirectInput())
    {
        n_error("nDI8Server: Could not initialize DirectInput8!\n");
    }
}

//------------------------------------------------------------------------------
