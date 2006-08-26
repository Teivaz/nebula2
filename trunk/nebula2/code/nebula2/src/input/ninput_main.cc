//------------------------------------------------------------------------------
//  ninput_main.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "kernel/nenv.h"
#include "kernel/nscriptserver.h"
#include "input/ninputserver.h"
#include "input/ninputevent.h"

#ifndef __XBxX__
#include "misc/nconserver.h"
#endif

nNebulaScriptClass(nInputServer, "nroot");
nInputServer* nInputServer::Singleton = 0;

#ifndef __XBxX__
struct nStrToKey {
    char *str;
    nKey key;
};

/*
    -24-Aug-05    kims    Fixed the bug #128, Added additional key mapping.
                          Thanks Magon Fox and Chris Tencati for the pointing out this.
*/
nStrToKey key_remaptable[] = {
    { "bs",         N_KEY_BACK, },
    { "tab",        N_KEY_TAB, },
    { "return",     N_KEY_RETURN, },
    { "shift",      N_KEY_SHIFT, },
    { "ctrl",       N_KEY_CONTROL, },
    { "menu",       N_KEY_MENU, },
    { "pause",      N_KEY_PAUSE, },
    { "capital",    N_KEY_CAPITAL, },
    { "esc",        N_KEY_ESCAPE, },
    { "space",      N_KEY_SPACE, },
    { "prior",      N_KEY_PRIOR, },
    { "next",       N_KEY_NEXT, },
    { "end",        N_KEY_END, },
    { "home",       N_KEY_HOME, },
    { "left",       N_KEY_LEFT, },
    { "up",         N_KEY_UP, },
    { "right",      N_KEY_RIGHT, },
    { "down",       N_KEY_DOWN, },
    { "select",     N_KEY_SELECT, },
    { "print",      N_KEY_PRINT, },
    { "exec",       N_KEY_EXECUTE, },
    { "snap",       N_KEY_SNAPSHOT, },
    { "insert",     N_KEY_INSERT, },
    { "delete",     N_KEY_DELETE, },
    { "help",       N_KEY_HELP, },
    { "1",          N_KEY_1, },
    { "2",          N_KEY_2, },
    { "3",          N_KEY_3, },
    { "4",          N_KEY_4, },
    { "5",          N_KEY_5, },
    { "6",          N_KEY_6, },
    { "7",          N_KEY_7, },
    { "8",          N_KEY_8, },
    { "9",          N_KEY_9, },
    { "0",          N_KEY_0, },
    { "a",          N_KEY_A, },
    { "b",          N_KEY_B, },
    { "c",          N_KEY_C, },
    { "d",          N_KEY_D, },
    { "e",          N_KEY_E, },
    { "f",          N_KEY_F, },
    { "g",          N_KEY_G, },
    { "h",          N_KEY_H, },
    { "i",          N_KEY_I, },
    { "j",          N_KEY_J, },
    { "k",          N_KEY_K, },
    { "l",          N_KEY_L, },
    { "m",          N_KEY_M, },
    { "n",          N_KEY_N, },
    { "o",          N_KEY_O, },
    { "p",          N_KEY_P, },
    { "q",          N_KEY_Q, },
    { "r",          N_KEY_R, },
    { "s",          N_KEY_S, },
    { "t",          N_KEY_T, },
    { "u",          N_KEY_U, },
    { "v",          N_KEY_V, },
    { "w",          N_KEY_W, },
    { "x",          N_KEY_X, },
    { "y",          N_KEY_Y, },
    { "z",          N_KEY_Z, },
    { "lwin",       N_KEY_LWIN, },
    { "rwin",       N_KEY_RWIN, },
    { "apps",       N_KEY_APPS, },
    { "num0",       N_KEY_NUMPAD0, },
    { "num1",       N_KEY_NUMPAD1, },
    { "num2",       N_KEY_NUMPAD2, },
    { "num3",       N_KEY_NUMPAD3, },
    { "num4",       N_KEY_NUMPAD4, },
    { "num5",       N_KEY_NUMPAD5, },
    { "num6",       N_KEY_NUMPAD6, },
    { "num7",       N_KEY_NUMPAD7, },
    { "num8",       N_KEY_NUMPAD8, },
    { "num9",       N_KEY_NUMPAD9, },
    { "mult",       N_KEY_MULTIPLY, },
    { "plus",       N_KEY_ADD, },
    { "sep",        N_KEY_SEPARATOR, },
    { "minus",      N_KEY_SUBTRACT, },
    { "decimal",    N_KEY_DECIMAL, },
    { "divide",     N_KEY_DIVIDE, },
    { "f1",         N_KEY_F1, },
    { "f2",         N_KEY_F2, },
    { "f3",         N_KEY_F3, },
    { "f4",         N_KEY_F4, },
    { "f5",         N_KEY_F5, },
    { "f6",         N_KEY_F6, },
    { "f7",         N_KEY_F7, },
    { "f8",         N_KEY_F8, },
    { "f9",         N_KEY_F9, },
    { "f10",        N_KEY_F10, },
    { "f11",        N_KEY_F11, },
    { "f12",        N_KEY_F12, },
    { "f13",        N_KEY_F13, },
    { "f14",        N_KEY_F14, },
    { "f15",        N_KEY_F15, },
    { "f16",        N_KEY_F16, },
    { "f17",        N_KEY_F17, },
    { "f18",        N_KEY_F18, },
    { "f19",        N_KEY_F19, },
    { "f20",        N_KEY_F20, },
    { "f21",        N_KEY_F21, },
    { "f22",        N_KEY_F22, },
    { "f23",        N_KEY_F23, },
    { "f24",        N_KEY_F24, },
    { "numlock",    N_KEY_NUMLOCK, },
    { "scroll",     N_KEY_SCROLL, },
    { "semicolon",  N_KEY_SEMICOLON, },
    { "slash",      N_KEY_SLASH, },
    { "tilde",      N_KEY_TILDE, },
    { "leftbracket",N_KEY_LEFTBRACKET, },
    { "backslash",  N_KEY_BACKSLASH, },
    { "rightbracket", N_KEY_RIGHTBRACKET, },
    { "quote",        N_KEY_QUOTE, },
    { "comma",        N_KEY_COMMA, },
    { "underbar",     N_KEY_UNDERBAR, },
    { "period",       N_KEY_PERIOD,},
    { "equality",     N_KEY_EQUALITY,},
    { NULL,           N_KEY_NONE },
};
#endif

//------------------------------------------------------------------------------
/**
*/
#ifndef __XBxX__
void
nInputServer::ExportDefaultKeyboard(void)
{
    nRoot *o;
    nEnv *env;
    const char *s;

    o = kernelServer->New("nroot","/sys/share/input/devs/keyb0");
    kernelServer->PushCwd(o);

    env = (nEnv *) kernelServer->New("nenv","inst_name");
    env->SetS("Keyboard 0");
    env = (nEnv *) kernelServer->New("nenv","prod_name");
    env->SetS("Default Keyboard");

    o = kernelServer->New("nroot","channels");
    kernelServer->PushCwd(o);

    int i = 0;
    while ((s = key_remaptable[i].str))
    {
        char buf[128];
        env = (nEnv *) kernelServer->New("nenv",s);
        sprintf(buf,"devid=%d type=%d key=%d",
                N_INPUT_KEYBOARD(0),
                (int)N_INPUT_KEY_DOWN,
                (int)key_remaptable[i].key);
        env->SetS(buf);
        i++;
    }
    kernelServer->PopCwd();
    kernelServer->PopCwd();
}
#endif

//------------------------------------------------------------------------------
/**
*/
#ifndef __XBxX__
void
nInputServer::ExportDefaultMouse(void)
{
    nRoot *o;
    nEnv *env;
    int i;

    o = kernelServer->New("nroot","/sys/share/input/devs/mouse0");
    kernelServer->PushCwd(o);

    env = (nEnv *) kernelServer->New("nenv","inst_name");
    env->SetS("Mouse 0");
    env = (nEnv *) kernelServer->New("nenv","prod_name");
    env->SetS("Default Mouse");

    o = kernelServer->New("nroot","channels");
    kernelServer->PushCwd(o);

    for (i=0; i<3; i++)
    {
        char buf[128];
        sprintf(buf,"btn%d",i);
        env = (nEnv *) kernelServer->New("nenv",buf);
        sprintf(buf,"devid=%d type=%d btn=%d",N_INPUT_MOUSE(0),(int)N_INPUT_BUTTON_DOWN,i);
        env->SetS(buf);
    }

    kernelServer->PopCwd();
    kernelServer->PopCwd();
}
#endif

//------------------------------------------------------------------------------
/**
*/
nInputServer::nInputServer() :
    ref_ss("/sys/servers/script"),
    ref_con("/sys/servers/console"),
    mouseFactor(1.0f),
    mouseInvert(false),
    mute(false),
    log_events(false),
    in_begin_map(false),
    act_script(0),
    double_click_time(0.5),
    long_pressed_time(1.0)
{
    n_assert(0 == Singleton);
    Singleton = this;

    this->ref_statedir   = kernelServer->New("nroot","/sys/share/input/states");
    this->ref_inpdir     = kernelServer->New("nroot","/sys/share/input");

#ifndef __XBxX__
    this->ExportDefaultKeyboard();
    this->ExportDefaultMouse();
#endif

    memset(this->script_array, 0, sizeof(this->script_array));
}

//------------------------------------------------------------------------------
/**
*/
nInputServer::~nInputServer()
{
    n_assert(this->act_script == 0);
    this->FlushEvents();
    if (this->ref_statedir.isvalid())
    {
        this->ref_statedir->Release();
    }
    if (this->ref_inpdir.isvalid())
    {
        this->ref_inpdir->Release();
    }

    // kill input mappings
    nInputMapping *im;
    while ((im = (nInputMapping *) (this->im_list.RemHead())))
    {
        n_delete(im);
    }

    // kill input states
    nInputState *is;
    while ((is = (nInputState *) (this->is_list.RemHead())))
    {
        n_delete(is);
    }

    n_assert(0 != Singleton);
    Singleton = 0;
}

//------------------------------------------------------------------------------
/**
*/
void
nInputServer::Open()
{
    //  empty
}

//------------------------------------------------------------------------------
/**
*/
void
nInputServer::BeginScripts(void)
{
    n_assert(this->act_script == 0);
}

//------------------------------------------------------------------------------
/**
*/
void
nInputServer::AddScript(const char *str)
{
    if (this->act_script < N_MAXNUM_SCRIPTS)
    {
        this->script_array[this->act_script++] = n_strdup(str);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nInputServer::EndScripts(void)
{
    int i;
    for (i=0; i<this->act_script; i++)
    {
        const char *cmd = this->script_array[i];
        // n_printf("running script: %s\n",cmd);

        nString result;
        this->ref_ss->Run(cmd, result);

        n_free((void *)this->script_array[i]);
        this->script_array[i] = NULL;
    }
    this->act_script = 0;
}

//------------------------------------------------------------------------------
/**
*/
void
nInputServer::SetLongPressedTime(float f)
{
    this->long_pressed_time = (double) f;
}

//------------------------------------------------------------------------------
/**
*/
float
nInputServer::GetLongPressedTime(void)
{
    return (float) this->long_pressed_time;
}

//------------------------------------------------------------------------------
/**
*/
void
nInputServer::SetDoubleClickTime(float f)
{
    this->double_click_time = (double) f;
}

//------------------------------------------------------------------------------
/**
*/
float nInputServer::GetDoubleClickTime(void)
{
    return (float) this->double_click_time;
}

//------------------------------------------------------------------------------
/**

    Do the usual per frame stuff, especially mapping raw events
    to cooked input states.

     - 09-Mar-99   floh    created
     - 22-Jul-99   floh    + nInputState-Handling
     - 26-Nov-99   floh    + oops, boeser Bug, innerhalb der Mapping-
                             Schleife konnten Scripts ausgefuehrt werden,
                             die die Mapping-Liste selbst aendern konnten,
                             wodurch die Schleife vollkommen aus dem
                             Tritt geriet und abstuerzen konnte!
     - 10-Dec-99   floh    + Disabled-Events werden jetzt beim Mapping
                             ignoriert (der Console-Server darf Tasten-
                             Events nicht mehr loeschen, sondern disabled
                             sie, das ganze, damit das Input-Recording auch
                             bei offener Konsole klappt)
     - 21-Dec-99   floh    + Modifier-Handling fuer Input-States
                           + neues Script-Handling (nicht mehr
                             generell bei Button-Down, sondern bei "Active"
*/
void
nInputServer::Trigger(double time)
{
    nInputMapping *im;

    this->timeStamp = time;

    // event logging and recording
    if (this->log_events)
    {
        this->LogEvents();
    }

#ifndef __XBxX__
    // trigger console server
    if (this->ref_con.isvalid())
    {
        this->ref_con->Trigger();
    }
#endif

    // perform input mapping
    this->DoInputMapping();

    // handle all active mappings...
    if (!this->mute)
    {
        this->BeginScripts();
        for (im = (nInputMapping *) this->im_list.GetHead();
            im;
            im = (nInputMapping *) im->GetSucc())
        {
            const char *cmd_str = im->GetCmdString();
            nInputState *is     = im->GetInputState();
            if (cmd_str)
            {
                if (im->IsActive()) this->AddScript(cmd_str);
            }
            if (is)
            {
                if (im->IsActive())
                {
                    is->SetButton(true);
                    is->AddSlider(1.0f);
                }
                is->AddSlider(im->GetSlider());
            }
        }
        this->EndScripts();
    }

    // update current mouse pos
    nInputEvent* inputEvent;
    for (inputEvent = this->FirstEvent(); inputEvent; inputEvent = this->NextEvent(inputEvent))
    {
        if (inputEvent->GetDeviceId() == N_INPUT_MOUSE(0))
        {
            if (inputEvent->GetType() == N_INPUT_MOUSE_MOVE)
            {
                this->mousePos.x = inputEvent->GetRelXPos();
                this->mousePos.y = inputEvent->GetRelYPos();
            }
        }
    }
}
