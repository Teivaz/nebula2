#if (defined __WIN32__ && !defined(__XBxX__)) || defined(DOXYGEN)
//------------------------------------------------------------------------------
//  nwin32loghandler.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "kernel/nwin32loghandler.h"
#include "util/npathstring.h"
#include <windows.h>
#include <assert.h>

//------------------------------------------------------------------------------
/**
    The constructor takes an application name which is used to name the
    log file and which is shown in the error message box titles.

    @param  app     an application name
*/
nWin32LogHandler::nWin32LogHandler(const char* app) :
    logFile(0)
{
    this->appName = app;

    // obtain app directory
    char buf[N_MAXPATH];
    GetModuleFileName(NULL, buf, sizeof(buf));
    nPathString appFileName(buf);
    appFileName.ConvertBackslashes();
    nPathString logFileName = appFileName.ExtractDirName();
    logFileName.Append("/");
    logFileName.Append(appName.Get());
    logFileName.Append(".log");

    // low level file access functions since we cannot guarantee that
    // a file server exists!
    this->logFile = fopen(logFileName.Get(), "w");
    
    // to prevent a infinite loop on error, we cannot simply call n_assert(),
    // since this would in turn ourselves, so handle the error message
    // with our own weapons
    if (!this->logFile)
    {
        MessageBox(0, "nWin32LogHandler: could not open log file!\n", 
                      this->appName.Get(), 
                      (MB_OK | MB_ICONERROR |MB_APPLMODAL | MB_SETFOREGROUND | MB_TOPMOST));
        exit(10);
    }
}

//------------------------------------------------------------------------------
/**
*/
nWin32LogHandler::~nWin32LogHandler()
{
    fclose(this->logFile);
    this->logFile = 0;
}

//------------------------------------------------------------------------------
/**
*/
void
nWin32LogHandler::Print(const char* msg, va_list argList)
{
    vfprintf(this->logFile, msg, argList);
    this->PutLineBuffer(msg, argList);
}

//------------------------------------------------------------------------------
/**
*/
void
nWin32LogHandler::Message(const char* msg, va_list argList)
{
    vfprintf(this->logFile, msg, argList);
    this->PutLineBuffer(msg, argList);
    this->PutMessageBox(MsgTypeMessage, msg, argList);
}

//------------------------------------------------------------------------------
/**
*/
void
nWin32LogHandler::Error(const char* msg, va_list argList)
{
    vfprintf(this->logFile, msg, argList);
    this->PutLineBuffer(msg, argList);
    this->PutMessageBox(MsgTypeError, msg, argList);
    //this is a error - kill self
    nWin32LogHandler::~nWin32LogHandler();
}

//------------------------------------------------------------------------------
/**
    Generate a Win32 message box.
*/
void
nWin32LogHandler::PutMessageBox(MsgType type, const char* msg, va_list argList)
{
    UINT boxType = (MB_OK | MB_APPLMODAL | MB_SETFOREGROUND | MB_TOPMOST);
    switch (type)
    {
        case MsgTypeMessage:
            boxType |= MB_ICONINFORMATION;
            break;
        case MsgTypeError:
            boxType |= MB_ICONERROR;
            break;
    }
    // FIXME: unsafe!
    char msgBuf[2048];
    int len = vsprintf(msgBuf, msg, argList);
    assert(len < (sizeof(msgBuf) - 1));
    MessageBox(0, msgBuf, this->appName.Get(), boxType);
}
#endif /* defined __WIN32__ && !defined(__XBxX__) */

