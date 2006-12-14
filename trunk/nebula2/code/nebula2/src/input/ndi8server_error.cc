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
    switch (hr)
    {
        case DI_BUFFEROVERFLOW:             return "DI_BUFFEROVERFLOW";
        case DI_DOWNLOADSKIPPED:            return "DI_DOWNLOADSKIPPED";
        case DI_EFFECTRESTARTED:            return "DI_EFFECTRESTARTED";
        case DI_OK:                         return "DI_OK";
        case DI_POLLEDDEVICE:               return "DI_POLLEDDEVICE";
        case DI_SETTINGSNOTSAVED:           return "DI_SETTINGSNOTSAVED";
        case DI_TRUNCATED:                  return "DI_TRUNCATED";
        case DI_TRUNCATEDANDRESTARTED:      return "DI_TRUNCATEDANDRESTARTED";
        case DI_WRITEPROTECT:               return "DI_WRITEPROTECT";
        case DIERR_ACQUIRED:                return "DIERR_ACQUIRED";
        case DIERR_ALREADYINITIALIZED:      return "DIERR_ALREADYINITIALIZED";
        case DIERR_BADDRIVERVER:            return "DIERR_BADDRIVERVER";
        case DIERR_BETADIRECTINPUTVERSION:  return "DIERR_BETADIRECTINPUTVERSION";
        case DIERR_DEVICEFULL:              return "DIERR_DEVICEFULL";
        case DIERR_DEVICENOTREG:            return "DIERR_DEVICENOTREG";
        case DIERR_EFFECTPLAYING:           return "DIERR_EFFECTPLAYING";
        case DIERR_GENERIC:                 return "DIERR_GENERIC";
        case DIERR_HANDLEEXISTS:            return "DIERR_HANDLEEXISTS";
        case DIERR_HASEFFECTS:              return "DIERR_HASEFFECTS";
        case DIERR_INCOMPLETEEFFECT:        return "DIERR_INCOMPLETEEFFECT";
        case DIERR_INPUTLOST:               return "DIERR_INPUTLOST";
        case DIERR_INVALIDPARAM:            return "DIERR_INVALIDPARAM";
        case DIERR_MAPFILEFAIL:             return "DIERR_MAPFILEFAIL";
        case DIERR_MOREDATA:                return "DIERR_MOREDATA";
        case DIERR_NOAGGREGATION:           return "DIERR_NOAGGREGATION";
        case DIERR_NOINTERFACE:             return "DIERR_NOINTERFACE";
        case DIERR_NOTACQUIRED:             return "DIERR_NOTACQUIRED";
        case DIERR_NOTBUFFERED:             return "DIERR_NOTBUFFERED";
        case DIERR_NOTDOWNLOADED:           return "DIERR_NOTDOWNLOADED";
        case DIERR_NOTEXCLUSIVEACQUIRED:    return "DIERR_NOTEXCLUSIVEACQUIRED";
        case DIERR_NOTFOUND:                return "DIERR_NOTFOUND";
        case DIERR_NOTINITIALIZED:          return "DIERR_NOTINITIALIZED";
        case DIERR_OLDDIRECTINPUTVERSION:   return "DIERR_OLDDIRECTINPUTVERSION";
        case DIERR_OUTOFMEMORY:             return "DIERR_OUTOFMEMORY";
        case DIERR_REPORTFULL:              return "DIERR_REPORTFULL";
        case DIERR_UNPLUGGED:               return "DIERR_UNPLUGGED";
        case DIERR_UNSUPPORTED:             return "DIERR_UNSUPPORTED";
        case E_HANDLE:                      return "E_HANDLE";
        case E_PENDING:                     return "E_PENDING";
        case E_POINTER:                     return "E_POINTER";
        default:                            return "UNKNOWN_ERROR";
    }
}
