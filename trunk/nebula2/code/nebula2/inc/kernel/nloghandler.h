#ifndef N_LOGHANDLER_H
#define N_LOGHANDLER_H
//------------------------------------------------------------------------------
/**
    The nLogHandler class provides a flexible way to provide text output
    to the user. Whenever the Print(), Message() or Error()
    methods of the kernel server are called, it will invoke the
    currently installed nLogHandler to display the information to the user. 
    There are subclasses of the log handler which will dump the message to stdout,
    a log file, or invoke operating system specific functions. The
    nLogHandler class just dump everything to NIL.

    (C) 2003 RadonLabs GmbH
*/
#ifndef N_TYPES_H
#include "kernel/ntypes.h"
#endif

#ifndef N_LINEBUFFER_H
#include "util/nlinebuffer.h"
#endif

//------------------------------------------------------------------------------
class nLogHandler 
{
public:
    /// constructor
    nLogHandler();
    /// destructor
    virtual ~nLogHandler();
    /// print a message to the log dump
    virtual void Print(const char* msg, va_list argList);
    /// show an important message (may block the program until the user acks)
    virtual void Message(const char* msg, va_list argList);
    /// show an error message (may block the program until the user acks)
    virtual void Error(const char* msg, va_list argList);
    /// get pointer to (optional) internal line buffer
    virtual nLineBuffer* GetLineBuffer();
};
//------------------------------------------------------------------------------
#endif



