//------------------------------------------------------------------------------
//  input/ninputmappping.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "input/ninputmapping.h"
#include "input/ninputserver.h"

//------------------------------------------------------------------------------
/**
*/
nInputMapping::nInputMapping(nInputServer *_iserv,
                             const char *_name,        // name to identify mapping
                             nInputEvent *_ie,         // input event
                             double _longpressed_dt,   // config: delta t for 'long pressed' state
                             double _doubleclick_dt,   // config: delta t for 'double clicked' state
                             int _mask)                // activation mask (N_IMSTATE_PRESSED...)
    : nHashNode(_name)
{
    this->iserv = _iserv;
    this->ie    = _ie;
    this->is    = NULL;
    this->cmd   = NULL;
    this->down_tstamp = 0.0;
    this->cur_tstamp  = 0.0;
    this->longpressed_dt = _longpressed_dt;
    this->doubleclick_dt = _doubleclick_dt;
    this->state_valid = false;
    this->state      = 0;
    this->mask       = _mask;
    this->prev_flags = 0;
    this->flags      = 0;
    this->slider_val = 0.0f;
}

//------------------------------------------------------------------------------
/**
*/
nInputMapping::~nInputMapping(void)
{
    // n_printf("~nInputMapping('%s')\n",this->GetName());
    if (this->ie)  this->iserv->ReleaseEvent(ie);
    if (this->cmd) this->ReleaseCmdString();
    if (this->is)  this->ReleaseInputState();
}

//------------------------------------------------------------------------------
/**
*/
void
nInputMapping::ReleaseInputState(void)
{
    n_assert(this->is);
    this->is->Release();
    if (this->is->GetRefCount() <= 0)
    {
        this->is->Remove();
        n_delete(this->is);
    }
    this->is = NULL;
}

//------------------------------------------------------------------------------
/**
*/
void
nInputMapping::ReleaseCmdString(void)
{
    n_assert(this->cmd);
    n_free((void *)this->cmd);
    this->cmd = NULL;
}

//------------------------------------------------------------------------------
/**
    Can deal with recycled and redundant input mappings, so
    that no input events 'get lost or get stuck' during a
    input remapping operation.
*/
void
nInputMapping::SetInputState(nInputState *_is)
{
    n_assert(_is);

    // if we're mapped to command string, free it!
    if (this->cmd) this->ReleaseCmdString();

    // previously mapped to an input state?
    if (this->is)
    {
        // identical input state? in that case, nothing changes,
        // including our own internal state
        if (this->is == _is)
        {
            return;
        }
        else
        {
            // mapped to a different input state, in that case,
            // our own internal state may change, so that we dont
            // get stuck...
            if (this->state & N_IMSTATE_PRESSED)
            {
                this->SetButtonUp();
            }
            this->ReleaseInputState();
        }
    }
    n_assert(NULL == this->is);
    this->is = _is;
    this->is->AddRef();
}

//------------------------------------------------------------------------------
/**
*/
void
nInputMapping::SetCmdString(const char *_cmd)
{
    n_assert(_cmd);
    if (this->cmd)
    {
        this->ReleaseCmdString();
    }

    this->cmd = n_strdup(_cmd);

    // previousely mapped to an input state?
    if (this->is)
    {
        this->ReleaseInputState();
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nInputMapping::PreTrigger(double tstamp)
{
    this->state_valid = false;
    this->cur_tstamp  = tstamp;

    // deactivate our input state
    if (this->is)
    {
        this->is->SetButton(false);
        this->is->SetSlider(0.0f);
    }

    // kill our own 'one-frame' state (everything expt pressed)
    this->state &= (N_IMSTATE_PRESSED|N_IMSTATE_LONGPRESSED);

    // check for long pressed status
    if ((this->state & N_IMSTATE_PRESSED) &&
        ((this->cur_tstamp - this->down_tstamp) > this->longpressed_dt))
    {
        // set longdown flag?
        if (0 == (this->state & N_IMSTATE_LONGPRESSED))
        {
            this->state |= N_IMSTATE_LONGDOWN;
        }
        this->state |= N_IMSTATE_LONGPRESSED;
    }
}

//------------------------------------------------------------------------------
/**
    08-Mar-06   floh    if time was stopped, each click would register as
                        a double click
*/
void
nInputMapping::PostTrigger(void)
{
    if (this->flags & N_IMF_BTN_DOWN)
    {
        // down will only be accepted if the mapping is not already
        // in pressed state!
        if (0 == (this->state & N_IMSTATE_PRESSED))
        {
            this->state |= (N_IMSTATE_DOWN | N_IMSTATE_PRESSED);

            // check for double click
            double timeDiff = this->cur_tstamp - this->down_tstamp;
            if ((timeDiff > 0.0) && (timeDiff < this->doubleclick_dt))
            {
                this->state |= N_IMSTATE_DOUBLECLICKED;
            }
            this->down_tstamp = this->cur_tstamp;
        }
    }

    if (this->flags & N_IMF_BTN_UP)
    {
        this->state &= ~N_IMSTATE_PRESSED;
        this->state |= N_IMSTATE_UP;
    }

    this->flags &= ~(N_IMF_BTN_UP | N_IMF_BTN_DOWN);
    this->state_valid = true;
}
