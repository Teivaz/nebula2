#if defined(__WIN32__) && !defined(__XBxX__)
#ifndef N_WIN32LOGHANDLER_H
#define N_WIN32LOGHANDLER_H
//------------------------------------------------------------------------------
/**
    A log handler class for Win32 apps:
    
    - maintains a log file in the application directory where ALL
      output is recorded
    - creates a message box for nKernelServer::Message() 
      and nKernelServer::Error()

    (C) 2003 RadonLabs GmbH
*/
#ifndef N_DEFAULTLOGHANDLER_H
#include "kernel/ndefaultloghandler.h"
#endif

#ifndef N_STRING_H
#include "util/nstring.h"
#endif

//------------------------------------------------------------------------------
class nWin32LogHandler : public nDefaultLogHandler
{
public:
    /// constructor
    nWin32LogHandler(const char* appName);
    /// destructor
    virtual ~nWin32LogHandler();
    /// print a message to the log dump
    virtual void Print(const char* msg, va_list argList);
    /// show an important message (may block the program until the user acks)
    virtual void Message(const char* msg, va_list argList);
    /// show an error message (may block the program until the user acks)
    virtual void Error(const char* msg, va_list argList);

private:
    enum MsgType
    {
        MsgTypeMessage,
        MsgTypeError,
    };

    /// put a message box on screen
    void PutMessageBox(MsgType msgType, const char* msg, va_list argList);

    nString appName;
    FILE* logFile;
};

//------------------------------------------------------------------------------
#endif
#endif /* defined(__WIN32__) && !defined(__XBxX__) */