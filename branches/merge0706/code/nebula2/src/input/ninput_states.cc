//------------------------------------------------------------------------------
//  ninput_states.cc
//  (C) 1999 A.Weissflog
//------------------------------------------------------------------------------
#include "kernel/nenv.h"
#include "input/ninputserver.h"

//------------------------------------------------------------------------------
/**
*/
nInputState*
nInputServer::GetInputState(const char *n)
{
    n_assert(n);
    return (nInputState *) this->is_list.Find(n);
}

//------------------------------------------------------------------------------
/**
*/
nInputState*
nInputServer::AddInputState(const char *n)
{
    n_assert(n);

    // export into Nebula file system first
    char o_name[N_MAXPATH];
    sprintf(o_name,"/sys/share/input/states/%s",n);
    kernelServer->New("nroot",o_name);

    // create and link input state object
    nInputState *is = n_new(nInputState(n));
    this->is_list.AddTail(is);
    return is;
}

//------------------------------------------------------------------------------
/**
*/
void 
nInputServer::BeginMap(void)
{
    n_assert(!this->in_begin_map);

    this->in_begin_map = true;

    // set the N_IMF_KILLME flag, so that EndMap() can
    // throw away all existing mappings that have not been 
    // touched
    nInputMapping *im;
    for (im = (nInputMapping *) this->im_list.GetHead();
         im;
         im = (nInputMapping *) im->GetSucc())
    {
        im->SetKillMe(true);
    }
}

//------------------------------------------------------------------------------
/**
    Build input mapping name from input event string and input state
    string.

    @param  ieStr       input event string
    @param  isStr       input state string
    @param  buf         target char buffer
    @param  bufSize     byte size of target buffer
    @return             pointer to buf
*/
const char*
nInputServer::BuildInputMappingName(const char* ieStr, const char* isStr, char* buf, int bufSize)
{
    n_assert(ieStr && isStr);
    n_assert(int((strlen(ieStr) + strlen(isStr))) < bufSize);
    sprintf(buf, "%s_%s", ieStr, isStr);
    return (const char*) buf;
}

//------------------------------------------------------------------------------
/**
*/
bool nInputServer::Map(const char *ie_str, const char *is_str)
{
    n_assert(this->in_begin_map);
    n_assert(ie_str);
    n_assert(is_str);
    char buf[N_MAXPATH];

    // build input mapping name
    const char* imName = this->BuildInputMappingName(ie_str, is_str, buf, sizeof(buf));

    // recycle existing mapping?
    nInputMapping *im = (nInputMapping *) this->im_list.Find(imName);
    if (!im) 
    {
        // no, create new mapping
        nInputEvent *ie  = NULL;
        char *ie_part  = NULL;
        char *mod_part = NULL;
        char buf[N_MAXNAMELEN];

        // analyze input event string...
        n_strncpy2(buf, ie_str, sizeof(buf));
        ie_part  = strchr(buf,'&');
        mod_part = strchr(buf,'.');
        if (ie_part) 
        {
            // ...has qualifier (*** OBSOLETE AND IGNORED! ***)
            *ie_part++ = 0;
        } else {
            // ...no qualifier
            ie_part  = buf;
        }
        if (mod_part) {
            // ...has modifier
            *mod_part++ = 0;
        }

        // generate input event object...
        if (ie_part) 
        {
            ie = this->NewEvent();
            if (!this->MapStrToEvent(ie_part,ie)) 
            {
                this->ReleaseEvent(ie);
                return false;
            }
        }

        // get modifier flags...
        int mod_flags = 0;
        if (mod_part) 
        {
            if (strcmp(mod_part, "pressed")       == 0) mod_flags = nInputMapping::N_IMSTATE_PRESSED;
            else if (strcmp(mod_part, "down")     == 0) mod_flags = nInputMapping::N_IMSTATE_DOWN;
            else if (strcmp(mod_part, "up")       == 0) mod_flags = nInputMapping::N_IMSTATE_UP;
            else if (strcmp(mod_part, "long")     == 0) mod_flags = nInputMapping::N_IMSTATE_LONGPRESSED;
            else if (strcmp(mod_part, "double")   == 0) mod_flags = nInputMapping::N_IMSTATE_DOUBLECLICKED;
            else if (strcmp(mod_part, "longdown") == 0) mod_flags = nInputMapping::N_IMSTATE_LONGDOWN;
            else 
            {
                n_printf("nInputServer::Map(%s,%s): invalid modifier '%s'!\n",
                         ie_str,is_str,mod_part);
            }
        }

        // create and link input mapping object...
        im = n_new(nInputMapping(this,
                                 imName,
                                 ie,
                                 this->long_pressed_time,
                                 this->double_click_time,
                                 mod_flags));
        this->im_list.AddTail(im);
    }

    // bind input state or script command
    char *scr_kw = "script:";
    int len_scr_kw = strlen(scr_kw);
    if (strncmp(is_str,scr_kw,len_scr_kw)==0) 
    {
        // script command...
        im->SetCmdString(&(is_str[len_scr_kw]));
    } 
    else 
    {
        // normal input state...
        nInputState *is = this->GetInputState(is_str);
        if (!is) is = this->AddInputState(is_str);
        im->SetInputState(is);
    }
    im->SetKillMe(false);
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void 
nInputServer::EndMap(void)
{
    n_assert(this->in_begin_map);
    this->in_begin_map = false;

    // throw away all untouched mapping objects
    nInputMapping *im = (nInputMapping *) this->im_list.GetHead();
    nInputMapping *next_im;
    if (im) do 
    {
        next_im = (nInputMapping *) im->GetSucc();
        if (im->GetKillMe()) 
        {
            im->Remove();
            n_delete(im);
        }
    } while ((im = next_im));

    // FIXME: debug cross check
    for (im = (nInputMapping *) this->im_list.GetHead();
         im;
         im = (nInputMapping *) im->GetSucc())
    {
        if (im->GetInputState()) 
        {
            nInputState *is;
            bool exists = false;
            for (is = (nInputState *) this->is_list.GetHead();
                 is;
                 is = (nInputState *) is->GetSucc())
            {
                if (is == im->GetInputState()) 
                {
                    exists = true;
                    break;
                }
            }
            if (!exists) 
            {
                n_error("nInputServer::EndMap(): ALERT! ALERT! ALERT!\n");
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
float 
nInputServer::GetSlider(const char *n)
{
    nInputState *is = this->GetInputState(n);
    if (is) 
    {    
        return is->GetSlider();
    } 
    else 
    {
        return 0.0f;
    }
}

//------------------------------------------------------------------------------
/**
*/
bool 
nInputServer::GetButton(const char *n)
{
    nInputState *is = this->GetInputState(n);
    if (is) 
    {
        return is->GetButton();
    } 
    else 
    {
        return false;
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
nInputServer::DoInputMapping(void)
{
    nInputMapping *im;

    // pre-trigger input mapping objects
    for (im=(nInputMapping *)this->im_list.GetHead();
         im;
         im=(nInputMapping *)im->GetSucc())
    {
        im->PreTrigger(this->timeStamp);
    }

    // update actual input events...
    for (im = (nInputMapping *) this->im_list.GetHead();
         im;
         im = (nInputMapping *) im->GetSucc())
    {
        nInputEvent *ie = im->GetInputEvent();
        n_assert(ie);
        nInputEvent *cur_ie = this->FirstIdenticalEvent(ie);
        if (cur_ie) do 
        {
            if (!cur_ie->IsDisabled()) 
            {
                switch (cur_ie->GetType()) 
                {
                    case N_INPUT_KEY_DOWN:
                    case N_INPUT_BUTTON_DOWN:
                        im->SetButtonDown();
                        break;
                    case N_INPUT_KEY_UP:
                    case N_INPUT_BUTTON_UP:
                        im->SetButtonUp();
                        break;
                    case N_INPUT_AXIS_MOVE:
                    case N_INPUT_MOUSE_MOVE:
                        im->SetSlider(cur_ie->GetAxisValue());
                        break;
                    default: break;
                }
            }
        } while ((cur_ie = this->NextIdenticalEvent(ie,cur_ie)));
    }

    // post-trigger input mapping objects
    for (im=(nInputMapping *)this->im_list.GetHead();
         im;
         im=(nInputMapping *)im->GetSucc())
    {
        im->PostTrigger();
    }
}

//------------------------------------------------------------------------------
/**
*/
void nInputServer::ObtainFocus(void)
{ 
    // all active input mappings must be reset
    nInputMapping *im;
    for (im = (nInputMapping *) this->im_list.GetHead();
         im;
         im = (nInputMapping *) im->GetSucc())
    {
        im->SetButtonUp();
        im->SetSlider(0.0f);
    }
}

//------------------------------------------------------------------------------
/**
*/
void nInputServer::LoseFocus(void)
{
    // all active input mappings must be reset
    nInputMapping *im;
    for (im = (nInputMapping *) this->im_list.GetHead();
         im;
         im = (nInputMapping *) im->GetSucc())
    {
        if (im->GetButtonState())
        {
            im->SetButtonUp();
        }
        im->SetSlider(0.0f);
    }
}

