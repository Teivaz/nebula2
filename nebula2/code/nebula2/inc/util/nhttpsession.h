#ifndef N_HTTPSESSION_H
#define N_HTTPSESSION_H
#ifndef __WIN32__
#error "nHttpSession not implemented!"
#endif

#if defined(__WIN32__) || defined(DOXYGEN)
//------------------------------------------------------------------------------
/**
    @class nHttpSession
    @ingroup Util

    A simple object for communicating with HTTP servers. Builds on
    top of the Win32 internet functions.

    (C) 2004 RadonLabs GmbH
*/
#include "kernel/ntypes.h"
#include "util/nstring.h"
#include "kernel/nipcbuffer.h"

#include <windows.h>
#include <wininet.h>

//------------------------------------------------------------------------------
class nHttpSession
{
public:
    /// constructor
    nHttpSession();
    /// destructor
    ~nHttpSession();
    /// set optional login username (for secure sites)
    void SetUserName(const nString& s);
    /// get optional login username
    const nString& GetUserName() const;
    /// set optional login password
    void SetPassword(const nString& s);
    /// get optional login password
    const nString& GetPassword() const;
    /// set the name of the HTTP server
    void SetServerName(const nString& s);
    /// get the name of the HTTP server
    const nString& GetServerName() const;
    /// set the HTTP user agent name
    void SetAgentName(const nString& n);
    /// get the HTTP user agent name
    const nString& GetAgentName() const;
    /// connect to a HTTP server
    bool Connect();
    /// disconnect from HTTP server
    void Disconnect();
    /// return true if connection open
    bool IsConnected() const;
    /// send a GET HTTP request and return result
    bool SendGetRequest(const nString& object, const nString& requestHeaders, nString& response);
    /// send a POST HTTP request, with the post data as string
    bool SendPostRequest(const nString& object, const nString& requestHeaders, const nString& data, nString& response);
    /// get an error string
    const nString& GetError() const;
    /// get the last cookie string
    const nString& GetCookie() const;

private:
    /// set error string
    void SetError(const nString& e);
    /// retrieve last error from Win32 internet subsystem
    void SetInternetLastError();
    /// add additional request headers (can be empty)
    bool AddRequestHeaders(HINTERNET hRequest, const nString& headers);
    /// read back request response
    bool ReadResponse(HINTERNET hRequest, nString& response);
    /// extract cookie from request
    bool ExtractCookie(const nString& object);
    /// check for dial-up connection and do the necessary stuff to connect
    bool DoDialupStuff();

    bool isConnected;
    nString userName;
    nString password;
    nString serverName;
    nString agentName;
    nString errorString;
    nString cookie;
    HINTERNET hInternet;
    HINTERNET hConnect;
};

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
nHttpSession::GetCookie() const
{
    return this->cookie;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nHttpSession::IsConnected() const
{
    return this->isConnected;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nHttpSession::SetUserName(const nString& s)
{
    this->userName = s;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
nHttpSession::GetUserName() const
{
    return this->userName;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nHttpSession::SetAgentName(const nString& n)
{
    this->agentName = n;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
nHttpSession::GetAgentName() const
{
    return this->agentName;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nHttpSession::SetPassword(const nString& s)
{
    this->password = s;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
nHttpSession::GetPassword() const
{
    return this->password;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nHttpSession::SetServerName(const nString& s)
{
    this->serverName = s;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
nHttpSession::GetServerName() const
{
    return this->serverName;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nHttpSession::SetError(const nString& e)
{
    this->errorString = e;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
nHttpSession::GetError() const
{
    return this->errorString;
}

//------------------------------------------------------------------------------
#endif /* __WIN32__ or DOXYGEN */
#endif

