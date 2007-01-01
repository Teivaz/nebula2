#ifndef N_LOGHANDLER_H
#define N_LOGHANDLER_H
//------------------------------------------------------------------------------
/**
    @class nLogHandler
    @ingroup Logging

    The nLogHandler class provides a flexible way to provide text output
    to the user. Whenever the Print(), Message() or Error()
    methods of the kernel server are called, it will invoke the
    currently installed nLogHandler to display the information to the user.
    There are subclasses of the log handler which will dump the message to stdout,
    a log file, or invoke operating system specific functions. The
    nLogHandler class just dump everything to NIL.

    (C) 2003 RadonLabs GmbH
*/
#include "kernel/ntypes.h"
#include "util/nlinebuffer.h"

#if defined(__LINUX__) || defined(__MACOSX__)
#include <stdarg.h>
#endif

//------------------------------------------------------------------------------
class nLogHandler
{
public:
    /// constructor
    nLogHandler();
    /// destructor
    virtual ~nLogHandler();
    /// open the log handler (called by nKernelServer when necessary)
    virtual bool Open();
    /// close the log handler (called by nKernelServer when necessary)
    virtual void Close();
    /// return open status
    bool IsOpen() const;
    /// print a message to the log dump
    virtual void Print(const char* msg, va_list argList);
    /// show an important message (may block the program until the user acks)
    virtual void Message(const char* msg, va_list argList);
    /// show an error message (may block the program until the user acks)
    virtual void Error(const char* msg, va_list argList);
    /// sends a string to the debugger for display.
    virtual void OutputDebug(const char* msg, va_list argList);
    /// get pointer to (optional) internal line buffer
    virtual nLineBuffer* GetLineBuffer();

private:
    bool isOpen;
};

//------------------------------------------------------------------------------
/**
*/
inline
bool
nLogHandler::IsOpen() const
{
    return this->isOpen;
}

//------------------------------------------------------------------------------
#endif
