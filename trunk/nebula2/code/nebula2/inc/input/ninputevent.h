#ifndef N_INPUTEVENT_H
#define N_INPUTEVENT_H
//-------------------------------------------------------------------
//  CLASS
//      nInputEvent -- Nebula-Input-Events
//
//  OVERVIEW
//      nInputEvents werden von beliebigen Quellen an den
//      nInputServer angehaengt.
//      InputEvents besitzen einen Typ, der die Art des
//      Events beschreibt (Taste gedrueckt, Maus bewegt,
//      etc...), einen Device-Typ, der den grundsaetzlichen
//      Typ des Eingabegeraetes beschreibt, und eine Device-
//      Nummer, die groesser 0 ist, wenn mehrere Geraete
//      desselben Typs angeschlossen sind.
//      Diese Informationen koennen in einen String gemappt
//      werden.
//-------------------------------------------------------------------
#ifndef N_TYPES_H
#include "kernel/ntypes.h"
#endif

#ifndef N_NODE_H
#include "util/nnode.h"
#endif

//-------------------------------------------------------------------
enum nInputType {
    N_INPUT_NONE,               // void
    N_INPUT_KEY_DOWN,           // Taste wurde gedrueckt
    N_INPUT_KEY_UP,             // Taste wurde losgelassen
    N_INPUT_KEY_CHAR,           // ASCII Code
    N_INPUT_MOUSE_MOVE,         // absolute Mouse-Position 
    N_INPUT_AXIS_MOVE,          // eine relative Achse (0.0 .. 1.0) 
    N_INPUT_BUTTON_DOWN,        // ein Button wurde gedrueckt
    N_INPUT_BUTTON_UP,          // ein Button wurde losgelassen
    N_INPUT_BUTTON_DBLCLCK,     // ein Button-Doppelklick
};

// Device-Typen
#define N_IDEV_NONE         (0)
#define N_IDEV_KEYBOARD     (1<<16)     // the system keyboard
#define N_IDEV_MOUSE        (1<<17)     // the system mouse
#define N_IDEV_JOYSTICK     (1<<18)     // a joystick or gamepad
#define N_IDEV_JOYMOUSE     (1<<19)     // mouse emulating a recentering joystick
#define N_IDEV_PADMOUSE     (1<<20)     // mouse emulating a non-centering joystick
#define N_IDEV_RELMOUSE     (1<<21)     // emits relative mouse movement

// Macros, um die Device-ID "lesbar" zumachen. Der erste
// Joystick waere N_IDEV_JOYSTICK(0), der zweite N_IDEV_JOYSTICK(1),
// usw...
#define N_INPUT_KEYBOARD(x)     (N_IDEV_KEYBOARD | x)
#define N_INPUT_MOUSE(x)        (N_IDEV_MOUSE | x)
#define N_INPUT_JOYSTICK(x)     (N_IDEV_JOYSTICK | x)
#define N_INPUT_JOYMOUSE(x)     (N_IDEV_JOYMOUSE | x)
#define N_INPUT_PADMOUSE(x)     (N_IDEV_PADMOUSE | x)
#define N_INPUT_RELMOUSE(x)     (N_IDEV_RELMOUSE | x)

// Keycodes fuer N_INPUT_KEY_DOWN/N_INPUT_KEY_UP
enum nKey {
    N_KEY_NONE,

    N_KEY_LBUTTON,
    N_KEY_RBUTTON,
    N_KEY_MBUTTON,
    N_KEY_BACK,
    N_KEY_TAB,
    N_KEY_RETURN,
    N_KEY_SHIFT,
    N_KEY_CONTROL,
    N_KEY_MENU,
    N_KEY_PAUSE,
    N_KEY_CAPITAL,
    N_KEY_ESCAPE,
    N_KEY_SPACE,
    N_KEY_PRIOR,
    N_KEY_NEXT,
    N_KEY_END,
    N_KEY_HOME,
    N_KEY_LEFT,
    N_KEY_UP,
    N_KEY_RIGHT,
    N_KEY_DOWN,
    N_KEY_SELECT,
    N_KEY_PRINT,
    N_KEY_EXECUTE,
    N_KEY_SNAPSHOT,
    N_KEY_INSERT,
    N_KEY_DELETE,
    N_KEY_HELP,

    N_KEY_1,
    N_KEY_2,
    N_KEY_3,
    N_KEY_4,
    N_KEY_5,
    N_KEY_6,
    N_KEY_7,
    N_KEY_8,
    N_KEY_9,
    N_KEY_0,

    N_KEY_A,
    N_KEY_B,
    N_KEY_C,
    N_KEY_D,
    N_KEY_E,
    N_KEY_F,
    N_KEY_G,
    N_KEY_H,
    N_KEY_I,
    N_KEY_J,
    N_KEY_K,
    N_KEY_L,
    N_KEY_M,
    N_KEY_N,
    N_KEY_O,
    N_KEY_P,
    N_KEY_Q,
    N_KEY_R,
    N_KEY_S,
    N_KEY_T,
    N_KEY_U,
    N_KEY_V,
    N_KEY_W,
    N_KEY_X,
    N_KEY_Y,
    N_KEY_Z,

    N_KEY_LWIN,
    N_KEY_RWIN,
    N_KEY_APPS,

    N_KEY_NUMPAD0,
    N_KEY_NUMPAD1,
    N_KEY_NUMPAD2,
    N_KEY_NUMPAD3,
    N_KEY_NUMPAD4,
    N_KEY_NUMPAD5,
    N_KEY_NUMPAD6,
    N_KEY_NUMPAD7,
    N_KEY_NUMPAD8,
    N_KEY_NUMPAD9,

    N_KEY_MULTIPLY,
    N_KEY_ADD,
    N_KEY_SEPARATOR,
    N_KEY_SUBTRACT,
    N_KEY_DECIMAL,
    N_KEY_DIVIDE,
    N_KEY_F1,
    N_KEY_F2,
    N_KEY_F3,
    N_KEY_F4,
    N_KEY_F5,
    N_KEY_F6,
    N_KEY_F7,
    N_KEY_F8,
    N_KEY_F9,
    N_KEY_F10,
    N_KEY_F11,
    N_KEY_F12,
    N_KEY_F13,
    N_KEY_F14,
    N_KEY_F15,
    N_KEY_F16,
    N_KEY_F17,
    N_KEY_F18,
    N_KEY_F19,
    N_KEY_F20,
    N_KEY_F21,
    N_KEY_F22,
    N_KEY_F23,
    N_KEY_F24,

    N_KEY_NUMLOCK,
    N_KEY_SCROLL,
};

//-------------------------------------------------------------------
class nInputEvent : public nNode {
public:
    // constructor
    nInputEvent();
    // set input event type
    void SetType(nInputType t);
    // get input event type
    nInputType GetType();
    // set device id
    void SetDeviceId(int id);
    // get device id
    int GetDeviceId();
    // set key code
    void SetKey(nKey k);
    // get key code
    nKey GetKey();
    // set character code
    void SetChar(int c);
    // get character code
    int GetChar();
    // set button number
    void SetButton(int b);
    // get button number
    int GetButton();
    // set axis number
    void SetAxis(int a);
    // get axis number
    int GetAxis();
    // set pov number
    void SetPov(int p);
    // get pov number
    int GetPov();
    // set axis value
    void SetAxisValue(float v);
    // get axis value
    float GetAxisValue();
    // set absolute mouse position
    void SetAbsPos(int x, int y);
    // get absolute x mouse position
    int GetAbsXPos();
    // get absolute y mouse position
    int GetAbsYPos();
    // set relative mouse position
    void SetRelPos(float x, float y);
    // get relative x mouse position
    float GetRelXPos();
    // get relative y mouse position
    float GetRelYPos();
    // set disabled flag
    void SetDisabled(bool b);
    // get disabled flag
    bool IsDisabled();

private:
    nInputType type;
    int flags;
    int deviceId;
    union
    {
        nKey  key;                          // Type == N_INPUT_KEY_*
        int   chr;                          // Type == N_INPUT_CHAR
        int   btn;                          // Type == N_MOUSE_DOWN/UP/DBLCLICK,N_BTN_DOWN/UP
        int   axis;                         // Type == N_AXIS_MOVE
        int   pov;                          // Type == N_POV_MOVE
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
