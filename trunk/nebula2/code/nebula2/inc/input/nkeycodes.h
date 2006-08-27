#ifndef N_KEYCODES_H
#define N_KEYCODES_H
//-------------------------------------------------------------------
/**
    @name Keycodes
    @ingroup NebulaInputSystem

    -24-Aug-05    kims    Fixed the bug #128, Added additional key mapping.
                          Thanks Magon Fox and Chris Tencati for the pointing out this.

    Keycodes for N_INPUT_KEY_DOWN/N_INPUT_KEY_UP
    @{
*/
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

    N_KEY_SEMICOLON,
    N_KEY_SLASH,
    N_KEY_TILDE,
    N_KEY_LEFTBRACKET,
    N_KEY_BACKSLASH,
    N_KEY_RIGHTBRACKET,
    N_KEY_QUOTE,
    N_KEY_COMMA,
    N_KEY_UNDERBAR,
    N_KEY_PERIOD,
    N_KEY_EQUALITY,
};
/// @}

#ifndef __XBxX__
struct nStrToKey {
    char *str;
    nKey key;
};

nStrToKey key_remaptable[];
#endif

//------------------------------------------------------------------------------
#endif
