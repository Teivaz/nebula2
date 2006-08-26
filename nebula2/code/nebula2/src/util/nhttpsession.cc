//------------------------------------------------------------------------------
//  nhttpsession.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#if defined(__WIN32__) || defined(DOXYGEN)
#include "kernel/nkernelserver.h"
#include "util/nhttpsession.h"
#include "util/nhttpresponsecracker.h"
#include "kernel/nenv.h"
#include "gfx2/ngfxserver2.h"

//------------------------------------------------------------------------------
/**
*/
nHttpSession::nHttpSession() :
    isConnected(false),
    agentName("Nebula2::HttpSession"),
    hInternet(0),
    hConnect(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nHttpSession::~nHttpSession()
{
    if (this->IsConnected())
    {
        this->Disconnect();
    }
}

//------------------------------------------------------------------------------
/**
    Retrieves the last error from the Win32 internet subsystem and sets it
    as this object's error string.
*/
void
nHttpSession::SetInternetLastError()
{
    char buf[1024];
    DWORD bufSize = sizeof(buf);
    DWORD errorCode;
    InternetGetLastResponseInfo(&errorCode, buf, &bufSize);
    this->SetError(buf);
}

//------------------------------------------------------------------------------
/**
    Check if the connection is a dialup connection and do the necessary
    things to connect to the internet. If the method returns false,
    a connection to the internet could not be established.
*/
bool
nHttpSession::DoDialupStuff()
{
    // check if we are on a modem
    DWORD connectedState;
    BOOL res = InternetGetConnectedState(&connectedState, 0);
    if (res)
    {
        // all ok, we are connected...
        return true;
    }
    else
    {
        // no connection to the internet, try dialup...
        nEnv* env = (nEnv*) nKernelServer::Instance()->Lookup("/sys/env/hwnd");
        HWND hwnd = 0;
        if (env)
        {
            // parent window exists...
            hwnd = (HWND) env->GetI();
        }

        nGfxServer2* gfxServer = nGfxServer2::Instance();
        if (hwnd)
        {
            gfxServer->EnterDialogBoxMode();
        }
        DWORD connection;
        DWORD dialRes = InternetDial(hwnd, NULL, INTERNET_AUTODIAL_FORCE_ONLINE, &connection, 0);
        if (hwnd)
        {
            gfxServer->LeaveDialogBoxMode();
        }
        if (dialRes == ERROR_SUCCESS)
        {
            return true;
        }
    }
    return false;
}

//------------------------------------------------------------------------------
/**
*/
bool
nHttpSession::Connect()
{
    n_assert(!this->IsConnected());
    n_assert(0 == this->hInternet);
    n_assert(!this->serverName.IsEmpty());
    n_assert(!this->agentName.IsEmpty());

    // attempt to connect to the Internet, this should bring up
    // a dialup dialog if no connection can be made...
    bool connected = this->DoDialupStuff();
    if (!connected)
    {
        return false;
    }

    // get a handle to the internet
    this->hInternet = InternetOpen(this->agentName.Get(),           // lpszAgent
                                   INTERNET_OPEN_TYPE_PRECONFIG,    // dwAccessType
                                   NULL,                            // lpszProxyName
                                   NULL,                            // lpszProxyBypass
                                   0);                              // dwFlags
    if (0 == this->hInternet)
    {
        // error
        DWORD err = GetLastError();
        this->SetError("InternetOpen() failed!");
        return false;
    }

    // connect to HTTP server
    this->hConnect = InternetConnect(this->hInternet,                   // hInternet
                                     this->serverName.Get(),            // lpszServerName
                                     INTERNET_INVALID_PORT_NUMBER,      // nServerPort (use default)
                                     this->userName.IsEmpty() ? 0 : this->userName.Get(),   // lpszUserName
                                     this->password.IsEmpty() ? 0 : this->password.Get(),   // lpszPassword
                                     INTERNET_SERVICE_HTTP,             // dwService
                                     0,                                 // dwFlags
                                     0);                                // dwContext
    if (0 == this->hConnect)
    {
        this->SetInternetLastError();
        return false;
    }

    this->isConnected = true;
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
nHttpSession::Disconnect()
{
    n_assert(this->IsConnected());
    if (this->hConnect)
    {
        InternetCloseHandle(this->hConnect);
        this->hConnect = 0;
    }
    if (this->hInternet)
    {
        InternetCloseHandle(this->hInternet);
        this->hInternet = 0;
    }
    this->isConnected = false;
}

//------------------------------------------------------------------------------
/**
    This adds optional request headers to the current request.
*/
bool
nHttpSession::AddRequestHeaders(HINTERNET hRequest, const nString& headers)
{
    n_assert(this->hConnect);
    if (!headers.IsEmpty())
    {
        BOOL result = HttpAddRequestHeaders(hRequest, headers.Get(), -1L, HTTP_ADDREQ_FLAG_ADD);
        return (result == TRUE) ? true : false;
    }
    return true;
}

//------------------------------------------------------------------------------
/**
    Read back response of a request.
*/
bool
nHttpSession::ReadResponse(HINTERNET hRequest, nString& response)
{
    response.Clear();

    // read data back
    DWORD numBytesToRead = 0;
    BOOL readSuccess = TRUE;
    while (readSuccess && InternetQueryDataAvailable(hRequest, &numBytesToRead, 0, 0) && (numBytesToRead > 0))
    {
        n_assert(numBytesToRead > 0);
        char* buffer = (char*) n_malloc(numBytesToRead);
        char* ptr = buffer;
        DWORD readNumBytes = numBytesToRead;
        DWORD numBytesRead = 0;
        do
        {
            readSuccess = InternetReadFile(hRequest, ptr, readNumBytes, &numBytesRead);
            readNumBytes -= numBytesRead;
            ptr += numBytesRead;
        }
        while ((!readSuccess) && (numBytesRead > 0));
        response.AppendRange(buffer, numBytesToRead);
        n_free(buffer);
    }
    return true;
}

//------------------------------------------------------------------------------
/**
    Extract the Cookie from the last request.
*/
bool
nHttpSession::ExtractCookie(const nString& objectName)
{
    this->cookie.Clear();

    nString url = "http://";
    url.Append(this->GetServerName());
    url.Append("/");
    url.Append(objectName);

    char buf[1024];
    DWORD bufSize = sizeof(buf);
    BOOL res = InternetGetCookie(url.Get(), NULL, buf, &bufSize);
    if (res)
    {
        this->cookie = buf;
        n_printf("nHttpSession: received cookie '%s'\n", this->cookie.Get());
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
*/
bool
nHttpSession::SendGetRequest(const nString& object, const nString& requestHeaders, nString& response)
{
    n_assert(this->IsConnected());
    n_assert(!object.IsEmpty());

    // open a http request
    HINTERNET hRequest = HttpOpenRequest(this->hConnect,        // hConnect
                                         "GET",                 // lpszVerb
                                         object.Get(),          // lpszObjectName
                                         "HTTP/1.1",            // lpszVersion
                                         NULL,                  // lpszReferer
                                         NULL,                  // lpszAcceptTypes
                                         0,                     // dwFlags
                                         0);                    // dwContext
    if (0 == hRequest)
    {
        this->SetInternetLastError();
        return false;
    }

    // add optional request headers
    this->AddRequestHeaders(hRequest, requestHeaders);

    // send the http request
    BOOL success = HttpSendRequest(hRequest,    // hRequest
                                   NULL,        // lpszHeaders
                                   0,           // dwHeadersLength
                                   NULL,        // lpOptional
                                   0);          // dwOptionalLength
    if (!success)
    {
        this->SetInternetLastError();
        InternetCloseHandle(hRequest);
        return false;
    }

    this->ExtractCookie(object);

    // read back response
    this->ReadResponse(hRequest, response);

    InternetCloseHandle(hRequest);
    return true;
}

//------------------------------------------------------------------------------
/**
*/
bool
nHttpSession::SendPostRequest(const nString& object,
                              const nString& requestHeaders,
                              const nString& data,
                              nString& response)
{
    n_assert(this->IsConnected());
    n_assert(!object.IsEmpty());

    // open a http request
    HINTERNET hRequest = HttpOpenRequest(this->hConnect,        // hConnect
                                         "POST",                // lpszVerb
                                         object.Get(),          // lpszObjectName
                                         "HTTP/1.1",            // lpszVersion
                                         NULL,                  // lpszReferer
                                         NULL,                  // lpszAcceptTypes
                                         0,                     // dwFlags
                                         0);                    // dwContext
    if (0 == hRequest)
    {
        this->SetInternetLastError();
        return false;
    }

    // add additional request headers
    this->AddRequestHeaders(hRequest, requestHeaders);

    // send the http request
    BOOL success = HttpSendRequest(hRequest,                // hRequest
                                   NULL,                    // lpszHeaders
                                   0,                       // dwHeadersLength
                                   (void*)data.Get(),       // lpOptional
                                   data.Length());          // dwOptionalLength
    if (!success)
    {
        this->SetInternetLastError();
        InternetCloseHandle(hRequest);
        return false;
    }

    this->ExtractCookie(object);

    // read data back
    this->ReadResponse(hRequest, response);

    InternetCloseHandle(hRequest);
    return true;
}
#endif /* __WIN32__ or DOXYGEN */

