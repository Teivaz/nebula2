#ifndef N_INPUTMAPPING_H
#define N_INPUTMAPPING_H
//-------------------------------------------------------------------
/**
    @class nInputMapping
    @ingroup NebulaInputSystem

    @brief Bind an input event to a named state. Automatically handle complex
    mappings with .down, .up, .double, .long modifiers...
*/
//-------------------------------------------------------------------
#include "util/nhashnode.h"

//-------------------------------------------------------------------
class nInputServer;
class nInputEvent;
class nInputState;

class nInputMapping : public nHashNode 
{
    nInputServer *iserv;    ///< pointer to input server
    nInputEvent *ie;        ///< input event itself
    nInputState *is;        ///< either bound to input state...
    const char *cmd;        ///< ... or to script command
    double down_tstamp;     ///< timestamp of last N_IMSTATE_DOWN
    double cur_tstamp;      ///< timestamp at Trigger()
    double longpressed_dt;  ///< delta t for '.long' modifier
    double doubleclick_dt;  ///< delta t for '.double' modifier
    float slider_val;

    bool state_valid;       ///< is state valid?
    int state;              ///< combination of N_IMSTATE_* flags
    int mask;               ///< combination of N_IMSTATE_* flags

    int prev_flags;         ///< flags at previous trigger
    int flags;              ///< current flags
    enum {
        N_IMF_KILLME        = (1<<0),
        N_IMF_BTN_UP        = (1<<1),   ///< button released this frame
        N_IMF_BTN_DOWN      = (1<<2),   ///< button down this frame
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
