#if (defined(__WIN32__) && !defined(__XBxX__)) || defined(DOXYGEN)
#ifndef N_WIN32LOGHANDLER_H
#define N_WIN32LOGHANDLER_H
//------------------------------------------------------------------------------
/**
    @class nWin32LogHandler
    @ingroup Logging

    A log handler class for Win32 apps:

    - maintains a log file where ALL output is recorded
    - creates a message box for nKernelServer::Message()
      and nKernelServer::Error()
    - Places log files in <tt>C:\\Documents and Settings\\&lt;your username&gt;\
      Local Settings\\Application Data\\&lt;subdir&gt;</tt>.
      By default, this will be
      <tt>C:\\Documents and Settings\\&lt;your username&gt;\
      Local Settings\\Application Data\\RadonLabs\\Nebula2</tt>.  The
      subdirectory name is passed to the constructor.
    - The actual file name is specified by the @c appName argument to the
      constructor.

    (C) 2003 RadonLabs GmbH
*/
#include "kernel/ndefaultloghandler.h"
#include "kernel/nfile.h"
#include "util/nstring.h"

//------------------------------------------------------------------------------
class nWin32LogHandler : public nDefaultLogHandler
{
public:
    /// constructor
    nWin32LogHandler(const char* appName, const char* subDir = 0);
    /// destructor
    virtual ~nWin32LogHandler();
    /// open the log handler (called by nKernelServer when necessary)
    virtual bool Open();
    /// close the log handler (called by nKernelServer when necessary)
    virtual void Close();
    /// print a message to the log dump
    virtual void Print(const char* msg, va_list argList);
    /// show an important message (may block the program until the user acks)
    virtual void Message(const char* msg, va_list argList);
    /// show an error message (may block the program until the user acks)
    virtual void Error(const char* msg, va_list argList);
    /// send a message to the debugger window.
    virtual void OutputDebug(const char* msg, va_list argList);

private:
    enum MsgType
    {
        MsgTypeMessage,
        MsgTypeError,
    };

    /// put a message box on screen
    void PutMessageBox(MsgType msgType, const char* msg, va_list argList);

    nString appName;
    nString subDir;
    nFile* logFile;
};

//------------------------------------------------------------------------------
#endif
#endif /* defined(__WIN32__) && !defined(__XBxX__) */
