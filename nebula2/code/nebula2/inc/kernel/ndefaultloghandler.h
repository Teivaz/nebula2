#ifndef N_DEFAULTLOGHANDLER_H
#define N_DEFAULTLOGHANDLER_H
//------------------------------------------------------------------------------
/**
    @class nDefaultLogHandler
    @ingroup NebulaLogging

    The default log handler routes all messages to stdout.
    
    (C) 2003 RadonLabs GmbH
*/
#ifndef N_LOGHANDLER_H
#include "kernel/nloghandler.h"
#endif

//------------------------------------------------------------------------------
class nDefaultLogHandler : public nLogHandler
{
public:
    /// constructor
    nDefaultLogHandler();
    /// destructor
    virtual ~nDefaultLogHandler();
    /// print a message to the log dump
    virtual void Print(const char* msg, va_list argList);
    /// show an important message (may block the program until the user acks)
    virtual void Message(const char* msg, va_list argList);
    /// show an error message (may block the program until the user acks)
    virtual void Error(const char* msg, va_list argList);
    /// fill a use
    virtual nLineBuffer* GetLineBuffer();

protected:
    /// add a line to the internal line buffer
    void PutLineBuffer(const char* msg, va_list argList);

    nLineBuffer lineBuffer;
};
//------------------------------------------------------------------------------
#endif
