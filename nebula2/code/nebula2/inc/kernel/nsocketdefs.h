#ifndef N_SOCKETDEFS_H
#define N_SOCKETDEFS_H
//------------------------------------------------------------------------------
/**
    Some platform specific wrapper defs for sockets.
    
    (C) 2002 RadonLabs GmbH
*/
#include "kernel/ntypes.h"

#if __XBxX__
#   include "xbox/nxbwrapper.h"
#elif defined(__WIN32__)
#   ifndef _INC_WINDOWS
#   define WIN32_LEAN_AND_MEAN
#   include <windows.h>
#   endif
#   ifndef _WINSOCKAPI_
#   include <winsock.h>
#   endif
#else
#   include <sys/types.h>
#   include <sys/socket.h>
#   include <netinet/in.h>
#   include <arpa/inet.h>
#   include <unistd.h>
#   include <netdb.h>
#   include <fcntl.h>
#   include <errno.h>
#endif

//------------------------------------------------------------------------------
#ifdef __WIN32__
#define N_SOCKET_LAST_ERROR    WSAGetLastError()
#define N_EWOULDBLOCK          WSAEWOULDBLOCK
typedef int socklen_t;
#else
typedef int SOCKET;
#define closesocket            close
#define INVALID_SOCKET         (-1)
#define SOCKET_ERROR           (-1)
#define N_SOCKET_LAST_ERROR    errno
#define N_EWOULDBLOCK          EWOULDBLOCK
#endif

#define N_SOCKET_MIN_PORTNUM   (12000)
#define N_SOCKET_MAX_PORTNUM   (12999)
#define N_SOCKET_PORTRANGE     (N_SOCKET_MAX_PORTNUM - N_SOCKET_MIN_PORTNUM)
//------------------------------------------------------------------------------
#endif
