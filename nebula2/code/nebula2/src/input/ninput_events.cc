//------------------------------------------------------------------------------
//  ninput_events.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "kernel/nenv.h"
#include "input/ninputserver.h"
#include "input/ninputmapping.h"

//------------------------------------------------------------------------------
/**
*/
nInputEvent *nInputServer::NewEvent(void)
{
    return n_new(nInputEvent);
}

//------------------------------------------------------------------------------
/**
*/
void nInputServer::ReleaseEvent(nInputEvent *e)
{
    if (e->IsLinked())
    {
        this->UnlinkEvent(e);
    }
    n_delete(e);
}

//------------------------------------------------------------------------------
/**
*/
void
nInputServer::LinkEvent(nInputEvent *e)
{
    n_assert(!e->IsLinked());
    this->events.AddTail(e);
}

//------------------------------------------------------------------------------
/**
*/
void
nInputServer::UnlinkEvent(nInputEvent *e)
{
    n_assert(e->IsLinked());
    e->Remove();
}

//------------------------------------------------------------------------------
/**
*/
void
nInputServer::FlushEvents(void)
{
    nInputEvent *e;
    while ((e = (nInputEvent *) this->events.RemHead())) n_delete(e);
}

//------------------------------------------------------------------------------
/**
    Flush events, states and mappings.
*/
void
nInputServer::FlushInput()
{
    // flush input events
    this->FlushEvents();

    // flush mappings
    nInputMapping* curMapping;
    for (curMapping = (nInputMapping*) this->im_list.GetHead();
         curMapping;
         curMapping = (nInputMapping*) curMapping->GetSucc())
    {
        curMapping->Clear();
    }

    // flush input states
    nInputState* curState;
    for (curState = (nInputState*) this->is_list.GetHead();
         curState;
         curState = (nInputState*) curState->GetSucc())
    {
        curState->Clear();
    }
}

//------------------------------------------------------------------------------
/**
*/
nInputEvent*
nInputServer::FirstEvent(void)
{
    return (nInputEvent *) this->events.GetHead();
}

//------------------------------------------------------------------------------
/**
*/
nInputEvent*
nInputServer::NextEvent(nInputEvent *e)
{
    return (nInputEvent *) e->GetSucc();
}

//------------------------------------------------------------------------------
/**
*/
bool nInputServer::IsIdenticalEvent(nInputEvent *e0, nInputEvent *e1)
{
    bool eq = false;
    if (e0->GetDeviceId() == e1->GetDeviceId())
    {
        switch(e0->GetType()) {
            case N_INPUT_KEY_DOWN:
            case N_INPUT_KEY_UP:
                if (((e1->GetType() == N_INPUT_KEY_DOWN) ||
                     (e1->GetType() == N_INPUT_KEY_UP)) &&
                     (e0->GetKey() == e1->GetKey()))
                {
                    eq = true;
                }
                break;

            case N_INPUT_KEY_CHAR:
                if ((e1->GetType() == N_INPUT_KEY_CHAR) && (e0->GetChar() == e1->GetChar())) {
                    eq=true;
                }
                break;

            case N_INPUT_BUTTON_DOWN:
            case N_INPUT_BUTTON_UP:
                if (((e1->GetType() == N_INPUT_BUTTON_DOWN) ||
                     (e1->GetType() == N_INPUT_BUTTON_UP)) &&
                     (e0->GetButton() == e1->GetButton()))
                {
                    eq=true;
                }
                break;
            case N_INPUT_AXIS_MOVE:
                if ((e1->GetType() == N_INPUT_AXIS_MOVE) &&
                    (e0->GetAxis() == e1->GetAxis()))
                {
                    eq=true;
                }
                break;
            case N_INPUT_MOUSE_MOVE:
                if (e1->GetType() == N_INPUT_MOUSE_MOVE) {
                    eq=true;
                }
                break;
            default: break;
        }
    }
    return eq;
}

//------------------------------------------------------------------------------
/**
*/
nInputEvent*
nInputServer::FirstIdenticalEvent(nInputEvent *pattern)
{
    nInputEvent *e = (nInputEvent *) this->events.GetHead();
    nInputEvent *eq = NULL;
    if (e) do
    {
        if (this->IsIdenticalEvent(pattern,e))
        {
            eq = e;
        }
    } while ((e=(nInputEvent *)e->GetSucc()) && (!eq));
    return eq;
}

//------------------------------------------------------------------------------
/**
*/
nInputEvent*
nInputServer::NextIdenticalEvent(nInputEvent *pattern, nInputEvent *e)
{
    nInputEvent *eq = NULL;
    while ((e = (nInputEvent *) e->GetSucc()) && (!eq))
    {
        if (this->IsIdenticalEvent(pattern,e))
        {
            eq = e;
        }
    }
    return eq;
}

//------------------------------------------------------------------------------
/**
*/
static int getInt(const char *str, const char *attr)
{
    char buf[128];
    char *kw;
    n_strncpy2(buf,str,sizeof(buf));
    kw = strtok(buf," =");
    if (kw) do {
        if (strcmp(kw,attr)==0) {
            char *val = strtok(NULL," =");
            if (val) return atoi(val);
        }
    } while ((kw = strtok(NULL," =")));
    return 0;
}

//------------------------------------------------------------------------------
/**
    Maps a string of the form "devN:channel" to an nInputEvent.
    The directory structure under /sys/share/input/devs is used to
    determine if the device exists and the channel is supported.
    If not, the nInputEvent is invalid and the function returns false.
*/
bool
nInputServer::MapStrToEvent(const char *str, nInputEvent *ie)
{
    char *dev_str, *chnl_str;
    char buf[128];
    char fname[128];
    nRoot *dev;
    bool retval = false;

    // separate device and channel strings...
    n_strncpy2(buf,str,sizeof(buf));
    dev_str  = buf;
    chnl_str = strchr(buf,':');
    if (chnl_str)
    {
        *chnl_str++ = 0;
    }
    else
    {
        n_printf("':' expected in input event '%s'\n",str);
        return false;
    }

    // search for device
    sprintf(fname,"/sys/share/input/devs/%s",dev_str);
    dev = kernelServer->Lookup(fname);
    if (dev)
    {
        nEnv *channel;
        kernelServer->PushCwd(dev);
        // search for channel
        sprintf(fname,"channels/%s",chnl_str);
        channel = (nEnv *) kernelServer->Lookup(fname);
        if (channel)
        {
            const char *attr = channel->GetS();
            ie->SetDeviceId(getInt(attr, "devid"));
            ie->SetType((nInputType) getInt(attr, "type"));
            switch (ie->GetType())
            {
                case N_INPUT_KEY_DOWN:
                case N_INPUT_KEY_UP:
                    ie->SetKey((nKey) getInt(attr,"key"));
                    break;
                case N_INPUT_AXIS_MOVE:
                    ie->SetAxis(getInt(attr,"axis"));
                    break;
                case N_INPUT_BUTTON_DOWN:
                case N_INPUT_BUTTON_UP:
                    ie->SetButton(getInt(attr,"btn"));
                    break;
                default: break;
            }
            retval = true;
        }
        else
        {
//            n_printf("Unknown channel '%s' for input device '%s'.\n",chnl_str,dev_str);
        }
        kernelServer->PopCwd();
    }
    return retval;
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
