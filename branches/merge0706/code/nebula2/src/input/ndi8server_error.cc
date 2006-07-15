//------------------------------------------------------------------------------
//  ndi8server_error.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "input/ndi8server.h"

//------------------------------------------------------------------------------
/**
    Convert DInput error code to string.
*/
const char*
nDI8Server::Error(HRESULT hr)
{
    const char* s;
    switch (hr)
    {
        case DI_BUFFEROVERFLOW:             s = "DI_BUFFEROVERFLOW"; break;
        case DI_DOWNLOADSKIPPED:            s = "DI_DOWNLOADSKIPPED"; break;
        case DI_EFFECTRESTARTED:            s = "DI_EFFECTRESTARTED"; break;
        case DI_OK:                         s = "DI_OK"; break;
        case DI_POLLEDDEVICE:               s = "DI_POLLEDDEVICE"; break;
        case DI_SETTINGSNOTSAVED:           s = "DI_SETTINGSNOTSAVED"; break;
        case DI_TRUNCATED:                  s = "DI_TRUNCATED"; break;
        case DI_TRUNCATEDANDRESTARTED:      s = "DI_TRUNCATEDANDRESTARTED"; break;
        case DI_WRITEPROTECT:               s = "DI_WRITEPROTECT"; break;
        case DIERR_ACQUIRED:                s = "DIERR_ACQUIRED"; break;
        case DIERR_ALREADYINITIALIZED:      s = "DIERR_ALREADYINITIALIZED"; break;
        case DIERR_BADDRIVERVER:            s = "DIERR_BADDRIVERVER"; break;
        case DIERR_BETADIRECTINPUTVERSION:  s = "DIERR_BETADIRECTINPUTVERSION"; break;
        case DIERR_DEVICEFULL:              s = "DIERR_DEVICEFULL"; break;
        case DIERR_DEVICENOTREG:            s = "DIERR_DEVICENOTREG"; break;
        case DIERR_EFFECTPLAYING:           s = "DIERR_EFFECTPLAYING"; break;
        case DIERR_GENERIC:                 s = "DIERR_GENERIC"; break;
        case DIERR_HANDLEEXISTS:            s = "DIERR_HANDLEEXISTS"; break;
        case DIERR_HASEFFECTS:              s = "DIERR_HASEFFECTS"; break;
        case DIERR_INCOMPLETEEFFECT:        s = "DIERR_INCOMPLETEEFFECT"; break;
        case DIERR_INPUTLOST:               s = "DIERR_INPUTLOST"; break;
        case DIERR_INVALIDPARAM:            s = "DIERR_INVALIDPARAM"; break;
        case DIERR_MAPFILEFAIL:             s = "DIERR_MAPFILEFAIL"; break;
        case DIERR_MOREDATA:                s = "DIERR_MOREDATA"; break;
        case DIERR_NOAGGREGATION:           s = "DIERR_NOAGGREGATION"; break;
        case DIERR_NOINTERFACE:             s = "DIERR_NOINTERFACE"; break;
        case DIERR_NOTACQUIRED:             s = "DIERR_NOTACQUIRED"; break;
        case DIERR_NOTBUFFERED:             s = "DIERR_NOTBUFFERED"; break;
        case DIERR_NOTDOWNLOADED:           s = "DIERR_NOTDOWNLOADED"; break;
        case DIERR_NOTEXCLUSIVEACQUIRED:    s = "DIERR_NOTEXCLUSIVEACQUIRED"; break;
        case DIERR_NOTFOUND:                s = "DIERR_NOTFOUND"; break;
        case DIERR_NOTINITIALIZED:          s = "DIERR_NOTINITIALIZED"; break;
        case DIERR_OLDDIRECTINPUTVERSION:   s = "DIERR_OLDDIRECTINPUTVERSION"; break;
        case DIERR_OUTOFMEMORY:             s = "DIERR_OUTOFMEMORY"; break;
        case DIERR_REPORTFULL:              s = "DIERR_REPORTFULL"; break;
        case DIERR_UNPLUGGED:               s = "DIERR_UNPLUGGED"; break;
        case DIERR_UNSUPPORTED:             s = "DIERR_UNSUPPORTED"; break;
        case E_HANDLE:                      s = "E_HANDLE"; break;
        case E_PENDING:                     s = "E_PENDING"; break;
        case E_POINTER:                     s = "E_POINTER"; break;
        default:                            s = "UNKNOWN"; break;
    }
    return s;
}
