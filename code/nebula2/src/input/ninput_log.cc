//------------------------------------------------------------------------------
//  ninput_log.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "kernel/nenv.h"
#include "input/ninputserver.h"
#include "input/ninputevent.h"

//------------------------------------------------------------------------------
/**
*/
void
nInputServer::StartLogging(void)
{
    if (this->log_events)
    {
        n_printf("already logging\n");
    }
    else
    {
        this->log_events = true;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nInputServer::StopLogging(void)
{
    if (this->log_events)
    {
        this->log_events = false;
    }
    else
    {
        n_printf("not logging!\n");
    }
}

//------------------------------------------------------------------------------
/**
*/
bool nInputServer::IsLogging(void)
{
    return this->log_events;
}

//------------------------------------------------------------------------------
/**
*/
void nInputServer::LogSingleEvent(nInputEvent *ie)
{
    char *dev_type;
    int dev_num = ie->GetDeviceId() & 0xffff;
    char *e_type;
    if (ie->GetDeviceId() & N_IDEV_KEYBOARD)      dev_type = "keyb";
    else if (ie->GetDeviceId() & N_IDEV_MOUSE)    dev_type = "mouse";
    else if (ie->GetDeviceId() & N_IDEV_JOYSTICK) dev_type = "joy";
    else if (ie->GetDeviceId() & N_IDEV_JOYMOUSE) dev_type = "joymouse";
    else if (ie->GetDeviceId() & N_IDEV_PADMOUSE) dev_type = "padmouse";
    else dev_type = "unknown";
    switch (ie->GetType())
    {
        case N_INPUT_KEY_DOWN:          e_type = "key_down"; break;
        case N_INPUT_KEY_UP:            e_type = "key_up"; break;
        case N_INPUT_KEY_CHAR:          e_type = "char"; break;
        case N_INPUT_MOUSE_MOVE:        e_type = "mouse_move"; break;
        case N_INPUT_AXIS_MOVE:         e_type = "axis_move"; break;
        case N_INPUT_BUTTON_DOWN:       e_type = "button_down"; break;
        case N_INPUT_BUTTON_UP:         e_type = "button_up"; break;
        default: e_type = "unknown"; break;
    }

    switch (ie->GetType())
    {
        case N_INPUT_KEY_DOWN:
        case N_INPUT_KEY_UP:
            n_printf("dev=%s%d type=%s key=%d\n",dev_type,dev_num,e_type,(int)ie->GetKey());
            break;
        case N_INPUT_KEY_CHAR:
            n_printf("dev=%s%d type=%s key=%d\n",dev_type,dev_num,e_type,ie->GetChar());
            break;
        case N_INPUT_BUTTON_DOWN:
        case N_INPUT_BUTTON_UP:
            n_printf("dev=%s%d type=%s btn=%d xy=[%d,%d]\n", dev_type, dev_num, e_type, ie->GetButton(), ie->GetAbsXPos(), ie->GetAbsYPos());
            break;
        case N_INPUT_MOUSE_MOVE:
            n_printf("dev=%s%d type=%s abs=[%d,%d] rel=[%f,%f]\n",
                dev_type, dev_num, e_type,
                ie->GetAbsXPos(), ie->GetAbsYPos(),
                ie->GetRelXPos(), ie->GetRelYPos());
            break;
        case N_INPUT_AXIS_MOVE:
            n_printf("dev=%s%d type=%s axis=%d val=%f\n", dev_type, dev_num, e_type, ie->GetAxis(), ie->GetAxisValue());
            break;
        default:
            n_printf("unknown event!\n");
            break;
    }
}

//------------------------------------------------------------------------------
/**
*/
void nInputServer::LogEvents(void)
{
    nInputEvent *ie;
    for (ie = (nInputEvent *) this->events.GetHead();
         ie;
         ie = (nInputEvent *) ie->GetSucc())
    {
        this->LogSingleEvent(ie);
    }
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
