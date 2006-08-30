#ifndef INPUT_PRIORITY_H
#define INPUT_PRIORITY_H
//------------------------------------------------------------------------------
/**
    Static declaration of the input priorities of Mangalore.

    (C) 2006 Radon Labs GmbH
*/

namespace Input
{

// used from the Mangalore ui to get left and right mouse button clicks
static int const InputPriority_MangaloreUI = 100;

// the default priority for game input event sinks
static int const InputPriority_DefaultGamePriority = 50;

// track the mouse position of the windows mouse cursor
static int const InputPriority_MousePositionTracking = 0;

}; // namespace Input

#endif