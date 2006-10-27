#ifndef N_INPUTSERVER_H
#define N_INPUTSERVER_H
//------------------------------------------------------------------------------
/**
    @class nInputServer
    @ingroup Input

    Manage input devices, events and mappings.

    See also @ref N2ScriptInterface_ninputserver

    (C) 2002 RadonLabs GmbH
*/
#include "kernel/ntypes.h"
#include "kernel/nroot.h"
#include "kernel/nautoref.h"
#include "input/ninputevent.h"
#include "input/ninputstate.h"
#include "util/nhashlist.h"

//------------------------------------------------------------------------------
class nScriptServer;
class nConServer;
class nInputServer : public nRoot
{
public:
    /// constructor
    nInputServer();
    /// destructor
    virtual ~nInputServer();
    /// get instance pointer
    static nInputServer* Instance();
    /// open the input server
    virtual void Open();
    /// per frame trigger
    virtual void Trigger(double time);
    /// create a new input event object
    nInputEvent *NewEvent();
    /// release an input event object
    void ReleaseEvent(nInputEvent* event);
    /// link input event object into global input event list
    void LinkEvent(nInputEvent* event);
    /// unlink input event object from global input event list
    void UnlinkEvent(nInputEvent* event);
    /// flush (clear) global input event list
    void FlushEvents();
    /// return pointer to first input event in global input event list
    nInputEvent *FirstEvent();
    /// return pointer to next event in global input event list
    nInputEvent *NextEvent(nInputEvent *);
    /// begin mapping input events
    void BeginMap();
    /// map an input event to an input state
    bool Map(const char* event, const char* state);
    /// finish mapping input
    void EndMap();
    /// get an input state as slider value
    float GetSlider(const char* state);
    /// get an input state as button value
    bool GetButton(const char* state);
    /// get the current mouse pos
    const vector2& GetMousePos() const;
    /// start logging input
    void StartLogging();
    /// stop logging input
    void StopLogging();
    /// is currently logging input?
    bool IsLogging();
    /// set the long pressed time
    void SetLongPressedTime(float);
    /// get the long pressed time
    float GetLongPressedTime();
    /// set the double click time
    void SetDoubleClickTime(float);
    /// get the double click time
    float GetDoubleClickTime();
    /// set mouse sensitivity
    void SetMouseFactor(float s);
    /// get mouse sensitivity
    float GetMouseFactor() const;
    /// set invert mouse flag
    void SetMouseInvert(bool b);
    /// get invert mouse flag
    bool GetMouseInvert() const;
    /// call when input focus obtained
    void ObtainFocus();
    /// call when input focus lost
    void LoseFocus();
    /// flush entire input mapping
    void FlushInput();
    /// set mute mode on/off
    void SetMute(bool b);
    /// get mute mode
    bool GetMute() const;

protected:
    /// convert an input event string into an input event
    bool MapStrToEvent(const char* str, nInputEvent* event);
    /// return pointer to first matching event
    nInputEvent *FirstIdenticalEvent(nInputEvent *pattern);
    /// return pointer to next matching event
    nInputEvent *NextIdenticalEvent(nInputEvent *pattern, nInputEvent *cur);
    /// do actual input mapping
    void DoInputMapping();
    /// begin adding a script to execute
    void BeginScripts();
    /// add a script to execute
    void AddScript(const char* script);
    /// finish adding script
    void EndScripts();
    /// check if events are identical
    bool IsIdenticalEvent(nInputEvent* event0, nInputEvent* event1);
    /// log a single event
    void LogSingleEvent(nInputEvent* event);
    /// log all events
    void LogEvents();
    /// find input state object by name
    nInputState* GetInputState(const char* state);
    /// add a named input state
    nInputState* AddInputState(const char* state);

#ifndef __XBxX__
    /// export the default keyboard
    void ExportDefaultKeyboard();
    /// export the default mouse
    void ExportDefaultMouse();
#endif

    /// build an input mapping name
    const char* BuildInputMappingName(const char* ieStr, const char* isStr, char* buf, int bufSize);

private:
    static nInputServer* Singleton;

protected:
    enum
    {
        N_MAXNUM_SCRIPTS = 16,              // max number of script cmds per frame
    };

    double timeStamp;                       // this frame's timestamp (updated in Trigger())

    nAutoRef<nScriptServer> ref_ss;         // "/sys/servers/script"
    nAutoRef<nConServer>    ref_con;        // "/sys/servers/console"
    nRef<nRoot> ref_statedir;               // "/sys/share/input/states"
    nRef<nRoot> ref_inpdir;                 // "/sys/share/input"

    nList events;               // list on nInputEvents for this frame

    bool log_events;
    bool in_begin_map;
    bool mute;

    nHashList im_list;          // list of nInputMapping's
    nHashList is_list;          // list of nInputState's

    int act_script;
    const char *script_array[N_MAXNUM_SCRIPTS];

    double long_pressed_time;
    double double_click_time;
    float mouseFactor;
    bool mouseInvert;
    vector2 mousePos;
};

//------------------------------------------------------------------------------
/**
*/
inline
nInputServer*
nInputServer::Instance()
{
    n_assert(Singleton);
    return Singleton;
}

//------------------------------------------------------------------------------
/**
    Set mute mode on/off. When in mute mode, only raw events will be
    generated, but no input mapping takes place.
*/
inline
void
nInputServer::SetMute(bool b)
{
    this->mute = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nInputServer::GetMute() const
{
    return this->mute;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector2&
nInputServer::GetMousePos() const
{
    return this->mousePos;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nInputServer::SetMouseFactor(float f)
{
    this->mouseFactor = f;
    n_printf("nInputServer::SetMouseFactor(%f)\n", f);
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nInputServer::GetMouseFactor() const
{
    return this->mouseFactor;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nInputServer::SetMouseInvert(bool b)
{
    this->mouseInvert = b;
    n_printf("nInputServer::SetInvertMouse(%s)\n", b ? "true" : "false");
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nInputServer::GetMouseInvert() const
{
    return this->mouseInvert;
}

//------------------------------------------------------------------------------
#endif
