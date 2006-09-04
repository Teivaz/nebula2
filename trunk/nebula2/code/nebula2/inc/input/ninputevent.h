#ifndef N_INPUTEVENT_H
#define N_INPUTEVENT_H
//-------------------------------------------------------------------
/**
    @class nInputEvent
    @ingroup Input
    @brief Nebula Input Events

    nInputEvents are attached to the nInputServer from arbitrary sources.

    InputEvents have a type that describe the kind of event it represents
    (Key press, mouse movement, etc), a device type which describes the
    base type of the input device, and a device number greater than 0
    when multiple devices of the same type are attached.

    This information can be mapped into a string.
*/
//-------------------------------------------------------------------
#include "kernel/ntypes.h"
#include "util/nnode.h"
#include "input/nkeycodes.h"

//-------------------------------------------------------------------
/**
    @name Input Types:
    @ingroup NebulaInputSystem
    @{
*/
enum nInputType {
    N_INPUT_KEY_DOWN = 0,       ///< Key was pressed
    N_INPUT_KEY_UP,             ///< Key was released
    N_INPUT_KEY_CHAR,           ///< ASCII Code
    N_INPUT_MOUSE_MOVE,         ///< absolute Mouse position
    N_INPUT_AXIS_MOVE,          ///< A relative movement along an axis (0.0 .. 1.0)
    N_INPUT_BUTTON_DOWN,        ///< A button was pressed
    N_INPUT_BUTTON_UP,          ///< A button was released
    N_INPUT_BUTTON_DBLCLCK,     ///< A button was double clicked

    N_INPUT_OBTAIN_FOCUS,       ///< the input system obtain the focus
    N_INPUT_LOSE_FOCUS,         ///< the input system loose the focus

    N_INPUT_NUM_VALID_TYPES,    ///< num valid types
    N_INPUT_NONE,               ///< void - invalid
};
/// @}

/**
    @name Device Types:
    @ingroup NebulaInputSystem
    @{
*/
#define N_IDEV_NONE         (0)
#define N_IDEV_KEYBOARD     (1<<16)     ///< the system keyboard
#define N_IDEV_MOUSE        (1<<17)     ///< the system mouse
#define N_IDEV_JOYSTICK     (1<<18)     ///< a joystick or gamepad
#define N_IDEV_JOYMOUSE     (1<<19)     ///< mouse emulating a recentering joystick
#define N_IDEV_PADMOUSE     (1<<20)     ///< mouse emulating a non-centering joystick
#define N_IDEV_RELMOUSE     (1<<21)     ///< emits relative mouse movement
/// @}

/**
    @name Device IDs:
    @ingroup NebulaInputSystem
    Macros which are close to the DEVICE ID. The first joystick would be N_IDEV_JOYSTICK(0),
    the second N_IDEV_JOYSTICK(1), etc....
    @{
*/
#define N_INPUT_KEYBOARD(x)     (N_IDEV_KEYBOARD | x)
#define N_INPUT_MOUSE(x)        (N_IDEV_MOUSE | x)
#define N_INPUT_JOYSTICK(x)     (N_IDEV_JOYSTICK | x)
#define N_INPUT_JOYMOUSE(x)     (N_IDEV_JOYMOUSE | x)
#define N_INPUT_PADMOUSE(x)     (N_IDEV_PADMOUSE | x)
#define N_INPUT_RELMOUSE(x)     (N_IDEV_RELMOUSE | x)
/// @}

//-------------------------------------------------------------------
class nInputEvent : public nNode {
public:
    /// constructor
    nInputEvent();
    /// set input event type
    void SetType(nInputType t);
    /// get input event type
    nInputType GetType();
    /// set device id
    void SetDeviceId(int id);
    /// get device id
    int GetDeviceId();
    /// set key code
    void SetKey(nKey k);
    /// get key code
    nKey GetKey();
    /// set character code
    void SetChar(int c);
    /// get character code
    int GetChar();
    /// set button number
    void SetButton(int b);
    /// get button number
    int GetButton();
    /// set axis number
    void SetAxis(int a);
    /// get axis number
    int GetAxis();
    /// set pov number
    void SetPov(int p);
    /// get pov number
    int GetPov();
    /// set axis value
    void SetAxisValue(float v);
    /// get axis value
    float GetAxisValue();
    /// set absolute mouse position
    void SetAbsPos(int x, int y);
    /// get absolute x mouse position
    int GetAbsXPos();
    /// get absolute y mouse position
    int GetAbsYPos();
    /// set relative mouse position
    void SetRelPos(float x, float y);
    /// get relative x mouse position
    float GetRelXPos();
    /// get relative y mouse position
    float GetRelYPos();
    /// set disabled flag
    void SetDisabled(bool b);
    /// get disabled flag
    bool IsDisabled();

private:
    nInputType type;
    int flags;
    int deviceId;
    union
    {
        nKey  key;                          ///< Type == N_INPUT_KEY_*
        int   chr;                          ///< Type == N_INPUT_CHAR
        int   btn;                          ///< Type == N_MOUSE_DOWN/UP/DBLCLICK,N_BTN_DOWN/UP
        int   axis;                         ///< Type == N_AXIS_MOVE
        int   pov;                          ///< Type == N_POV_MOVE
    };
    float axisVal;
    int absPosX;
    int absPosY;
    float relPosX;
    float relPosY;

    enum {
        N_IEF_DISABLED = (1<<0),
    };
};

//------------------------------------------------------------------------------
/**
*/
inline
nInputEvent::nInputEvent() :
    type(N_INPUT_NONE),
    flags(0),
    deviceId(0),
    chr(0),
    axisVal(0.0f),
    absPosX(0),
    absPosY(0),
    relPosX(0),
    relPosY(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nInputEvent::SetType(nInputType t)
{
    this->type = t;
}

//------------------------------------------------------------------------------
/**
*/
inline
nInputType
nInputEvent::GetType()
{
    return this->type;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nInputEvent::SetDeviceId(int id)
{
    this->deviceId = id;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nInputEvent::GetDeviceId()
{
    return this->deviceId;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nInputEvent::SetKey(nKey k)
{
    this->key = k;
}

//------------------------------------------------------------------------------
/**
*/
inline
nKey
nInputEvent::GetKey()
{
    return this->key;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nInputEvent::SetChar(int c)
{
    this->chr = c;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nInputEvent::GetChar()
{
    return this->chr;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nInputEvent::SetButton(int b)
{
    this->btn = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nInputEvent::GetButton()
{
    return this->btn;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nInputEvent::SetAxis(int a)
{
    this->axis = a;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nInputEvent::GetAxis()
{
    return this->axis;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nInputEvent::SetPov(int p)
{
    this->pov = p;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nInputEvent::GetPov()
{
    return this->pov;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nInputEvent::SetAxisValue(float v)
{
    this->axisVal = v;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nInputEvent::GetAxisValue()
{
    return this->axisVal;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nInputEvent::SetAbsPos(int x, int y)
{
    this->absPosX = x;
    this->absPosY = y;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nInputEvent::GetAbsXPos()
{
    return this->absPosX;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nInputEvent::GetAbsYPos()
{
    return this->absPosY;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nInputEvent::SetRelPos(float x, float y)
{
    this->relPosX = x;
    this->relPosY = y;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nInputEvent::GetRelXPos()
{
    return this->relPosX;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nInputEvent::GetRelYPos()
{
    return this->relPosY;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nInputEvent::SetDisabled(bool b)
{
    if (b)
    {
        this->flags |= N_IEF_DISABLED;
    }
    else
    {
        this->flags &= ~N_IEF_DISABLED;
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nInputEvent::IsDisabled()
{
    return (this->flags & N_IEF_DISABLED) ? true : false;
}

//------------------------------------------------------------------------------
#endif
