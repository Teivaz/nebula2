//------------------------------------------------------------------------------
//  ninput_main.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "kernel/nenv.h"
#include "kernel/nscriptserver.h"
#include "input/ninputserver.h"
#include "input/ninputevent.h"
#include "input/ninputmapping.h"
#include "input/nkeycodes.h"

#ifndef __XBxX__
#include "misc/nconserver.h"
#endif

nNebulaScriptClass(nInputServer, "kernel::nroot");
nInputServer* nInputServer::Singleton = 0;

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
