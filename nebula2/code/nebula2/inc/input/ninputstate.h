#ifndef N_INPUTSTATE_H
#define N_INPUTSTATE_H
//-------------------------------------------------------------------
//  OVERVIEW
//  nInputState objects are named input 'slots' which are
//  bound to input events (or a simple combination of ie's)
//  through a user defined mapping table.
//-------------------------------------------------------------------
#ifndef N_TYPES_H
#include "kernel/ntypes.h"
#endif

#ifndef N_HASHNODE_H
#include "util/nhashnode.h"
#endif

#ifndef N_INPUTEVENT_H
#include "input/ninputevent.h"
#endif

//-------------------------------------------------------------------
//  nInputState
//  Keep single "button" and "slider" state and have a name. 
//  21-Dec-99   floh    completely rewritten    
//  29-Dec-99   floh    + ref_count
//-------------------------------------------------------------------
class nInputState : public nHashNode {
    int ref_count;
    bool  btn;
    float val;

public:
    nInputState(const char *_name) 
        : nHashNode(_name)
    {
        ref_count = 0;
        btn = false;
        val = 0.0f;
    };
    ~nInputState() {
        // n_printf("~nInputState(%s)\n",this->GetName());
    };
    void AddRef(void) {
        ref_count++;
    };
    void RemRef(void) {
        n_assert(this->ref_count > 0);
        ref_count--;
    };
    int GetRef(void) {
        return this->ref_count;
    };
    void SetButton(bool b) {
        btn = b;
    };
    void SetSlider(float f) {
        val = f;
    };
    void AddSlider(float f) {
        val += f;
        if (val > 1.0f)      val = 1.0f;
        else if (val < 0.0f) val = 0.0f;
    };
    bool GetButton(void) {
        return btn;
    };
    float GetSlider(void) {
        return val;
    };
    /// clear input state
    void Clear()
    {
        this->btn = false;
        this->val = 0.0f;
    };
};

//-------------------------------------------------------------------
//  nInputMapping
//  Bind an input event to a named state. Automatically handle complex
//  mappings with .down, .up, .double, .long modifiers...
//-------------------------------------------------------------------
class nInputServer;
class nInputMapping : public nHashNode 
{
    nInputServer *iserv;    // pointer to input server
    nInputEvent *ie;        // input event itself
    nInputState *is;        // either bound to input state...
    const char *cmd;        // ... or to script command
    double down_tstamp;     // timestamp of last N_IMSTATE_DOWN
    double cur_tstamp;      // timestamp at Trigger()
    double longpressed_dt;  // delta t for '.long' modifier
    double doubleclick_dt;  // delta t for '.double' modifier
    float slider_val;

    bool state_valid;       // is state valid?
    int state;              // combination of N_IMSTATE_* flags
    int mask;               // combination of N_IMSTATE_* flags

    int prev_flags;         // flags at previous trigger
    int flags;              // current flags
    enum {
        N_IMF_KILLME        = (1<<0),
        N_IMF_BTN_UP        = (1<<1),   // button released this frame
        N_IMF_BTN_DOWN      = (1<<2),   // button down this frame
    };

public:
    enum {
        N_IMSTATE_PRESSED       = (1<<0),
        N_IMSTATE_DOWN          = (1<<1),
        N_IMSTATE_UP            = (1<<2),
        N_IMSTATE_LONGDOWN      = (1<<3),
        N_IMSTATE_LONGPRESSED   = (1<<4),
        N_IMSTATE_DOUBLECLICKED = (1<<5),
    };
    nInputMapping(nInputServer *, const char *, nInputEvent *, double, double, int);
    ~nInputMapping(void);

    void PreTrigger(double);
    void PostTrigger(void);
    void SetInputState(nInputState *);
    void SetCmdString(const char *);
    void ReleaseInputState(void);
    void ReleaseCmdString(void);

    void SetButtonDown(void) {
        this->flags |= N_IMF_BTN_DOWN;
    };
    void SetButtonUp(void) {
        this->flags |= N_IMF_BTN_UP;
    };
    nInputState *GetInputState(void) {
        return this->is;
    };
    const char *GetCmdString(void) {
        return this->cmd;
    };
    nInputEvent *GetInputEvent(void) {
        return this->ie;
    };
    bool GetButtonState(void) {
        return (this->state & N_IMSTATE_PRESSED) ? true : false;
    };
    void SetKillMe(bool b) {
        if (b) this->flags |= N_IMF_KILLME;
        else   this->flags &= ~N_IMF_KILLME;
    };
    bool GetKillMe(void) {
        return (this->flags & N_IMF_KILLME) ? true : false;
    };
    bool IsActive(void) {
        // ONLY VALID AFTER PostTrigger()!
        n_assert(this->state_valid);
        return (this->state & this->mask) ? true : false;
    };
    void SetSlider(float f) {
        this->slider_val = f;
    };
    float GetSlider(void) {
        return this->slider_val;
    };
	/// clear buttonDown/Up flags
	void Clear();
};

//-------------------------------------------------------------------
/**
*/
inline
void
nInputMapping::Clear()
{
	this->flags = 0;
	this->state = 0;
}

//-------------------------------------------------------------------
#endif
