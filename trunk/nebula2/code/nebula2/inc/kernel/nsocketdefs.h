#ifndef N_SOCKETDEFS_H
#define N_SOCKETDEFS_H
//------------------------------------------------------------------------------
/**
    Some platform specific wrapper defs for sockets.
    
    (C) 2002 RadonLabs GmbH
*/
#ifndef N_TYPES_H
#include "kernel/ntypes.h"
#endif

//------------------------------------------------------------------------------
#ifndef __WIN32__
typedef int SOCKET;
#define closesocket close
#define INVALID_SOCKET (-1)
#endif

#define N_SOCKET_MIN_PORTNUM   (12000)
#define N_SOCKET_MAX_PORTNUM   (12999)
#define N_SOCKET_PORTRANGE     (N_SOCKET_MAX_PORTNUM - N_SOCKET_MIN_PORTNUM)
//------------------------------------------------------------------------------
#endif
