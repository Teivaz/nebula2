#if (defined __WIN32__ && !defined(__XBxX__)) || defined(DOXYGEN)
//------------------------------------------------------------------------------
//  nwin32loghandler.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "kernel/nsystem.h"
#include "kernel/nkernelserver.h"
#include "kernel/nwin32loghandler.h"
#include "kernel/nfileserver2.h"
#include "kernel/nenv.h"
#include <windows.h>
#include <shfolder.h>
#include <assert.h>

//------------------------------------------------------------------------------
/**
    The constructor takes an application name which is used to name the
    log file and which is shown in the error message box titles.

    @param  app     an application name
    @param  subDir  the name of the subdirectory within which the log file
                    will be created.

    - 03-Mar-04     floh    no longer saves into app directory, since
                            Non-Admin-Users have no write access there
*/
nWin32LogHandler::nWin32LogHandler(const char* app, const char* subDir) :
    logFile(0),
    subDir("RadonLabs/Nebula2")
{
    this->appName = app;
    if (subDir)
    {
        this->subDir = subDir;
    }
}

//------------------------------------------------------------------------------
/**
*/
nWin32LogHandler::~nWin32LogHandler()
{
    if (this->IsOpen())
    {
        this->Close();
    }
}

//------------------------------------------------------------------------------
/**
    Open the log handler. This method is called when the log handler
    is attached to the kernel.
*/
bool
nWin32LogHandler::Open()
{
    n_assert(0 == this->logFile);
    nFileServer2* fileServer = nKernelServer::Instance()->GetFileServer();
    if (nLogHandler::Open())
    {
        // obtain log directory
        char rawPath[MAX_PATH];
        HRESULT hr = nWin32Wrapper::Instance()->SHGetFolderPath(0, // hwndOwner
                        CSIDL_LOCAL_APPDATA | CSIDL_FLAG_CREATE,   // nFolder
                        NULL,                                      // hToken
                        0,                                         // dwFlags
                        rawPath);                                  // pszPath
        n_assert(S_OK == hr);

        // construct path
        nString path(rawPath);
        path.ConvertBackslashes();
        path.Append("/");
        path.Append(this->subDir);
        path.Append("/");

        // make sure path exists
        fileServer->MakePath(path.Get());

        // append logfile name
        path.Append(this->appName);
        path.Append(".log");

        // low level file access functions since we cannot guarantee that
        // a file server exists!
        this->logFile = fileServer->NewFileObject();

        // Note: Failing to open the log file is not an error. There may
        // be several versions of the same application running, which
        // would fight for the log file. The first one wins, the other
        // silently don't log.
        this->logFile->Open(path.Get(), "w");
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    This method is called when the log handler is detached from the kernel,
    or from the destructor.
*/
void
nWin32LogHandler::Close()
{
    n_assert(0 != this->logFile);
    this->logFile->Release();
    this->logFile = 0;
    nLogHandler::Close();
}

//------------------------------------------------------------------------------
/**
*/
void
nWin32LogHandler::Print(const char* msg, va_list argList)
{
    if (this->logFile && this->logFile->IsOpen())
    {
        char buf[1024];
        _vsnprintf(buf, sizeof(buf) - 1, msg, argList);
        this->logFile->PutS(buf);
    }
    this->PutLineBuffer(msg, argList);
}

//------------------------------------------------------------------------------
/**
*/
void
nWin32LogHandler::Message(const char* msg, va_list argList)
{
    if (this->logFile && this->logFile->IsOpen())
    {
        char buf[1024];
        _vsnprintf(buf, sizeof(buf) - 1, msg, argList);
        this->logFile->PutS(buf);
    }
    this->PutLineBuffer(msg, argList);
    this->PutMessageBox(MsgTypeMessage, msg, argList);
}

//------------------------------------------------------------------------------
/**
*/
void
nWin32LogHandler::Error(const char* msg, va_list argList)
{
    if (this->logFile && this->logFile->IsOpen())
    {
        char buf[1024];
        _vsnprintf(buf, sizeof(buf) - 1, msg, argList);
        this->logFile->PutS(buf);
    }
    this->PutLineBuffer(msg, argList);
    this->PutMessageBox(MsgTypeError, msg, argList);
    //this is a error - kill self
    nWin32LogHandler::~nWin32LogHandler();
}

//------------------------------------------------------------------------------
/**
    Send a message to debugger window.

    - 26-Mar-05    kims    created
*/
void
nWin32LogHandler::OutputDebug(const char* msg, va_list argList)
{
    if (this->logFile && this->logFile->IsOpen())
    {
        char buf[1024];
        _vsnprintf(buf, sizeof(buf) - 1, msg, argList);
        this->logFile->PutS(buf);
    }

    this->PutLineBuffer(msg, argList);

    // put the last msg in the buffer to debugger window.
    int lastline = this->lineBuffer.GetTailLine();
    OutputDebugString(this->lineBuffer.GetLine(lastline));
}

//------------------------------------------------------------------------------
/**
    Generate a Win32 message box.
*/
void
nWin32LogHandler::PutMessageBox(MsgType type, const char* msg, va_list argList)
{
    // find app window, and minimize it
    // this is necessary when in Fullscreen mode, otherwise
    // the MessageBox() may not be visible
    if (MsgTypeError == type)
    {
        nEnv* envHwnd = (nEnv*) nKernelServer::Instance()->Lookup("/sys/env/hwnd");
        if (envHwnd)
        {
            HWND hwnd = (HWND) envHwnd->GetI();
            if (hwnd)
            {
                ShowWindow(hwnd, SW_MINIMIZE);
            }
        }
    }

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
    char msgBuf[2048];
    vsnprintf(msgBuf, sizeof(msgBuf), msg, argList);
    MessageBox(0, msgBuf, this->appName.Get(), boxType);
}
#endif /* defined __WIN32__ && !defined(__XBxX__) */

