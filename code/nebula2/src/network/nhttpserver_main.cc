//------------------------------------------------------------------------------
//  nhttpserver_main.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "network/nhttpserver.h"

nNebulaScriptClass(nHttpServer, "kernel::nroot");

//------------------------------------------------------------------------------
/**
*/
nHttpServer::nHttpServer()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nHttpServer::~nHttpServer()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
bool
nHttpServer::Connect(const nString& serverName)
{
#ifdef __WIN32__
    if (this->httpSession.IsConnected())
    {
        this->httpSession.Disconnect();
    }
    this->httpSession.SetServerName(serverName);
    bool res = this->httpSession.Connect();
    if (!res)
    {
        n_printf("nHttpServer::Connect(): error '%s'\n", this->httpSession.GetError().Get());
    }
    return res;
#else
    n_printf("nHttpServer::Connect(): not implemented!\n");
    return false;
#endif
}

//------------------------------------------------------------------------------
/**
*/
void
nHttpServer::Disconnect()
{
#ifdef __WIN32__
    if (this->httpSession.IsConnected())
    {
        this->httpSession.Disconnect();
    }
#endif
}

//------------------------------------------------------------------------------
/**
*/
bool
nHttpServer::IsConnected() const
{
#ifdef __WIN32__
    return this->httpSession.IsConnected();
#else
    return false;
#endif
}

//------------------------------------------------------------------------------
/**
*/
bool
nHttpServer::Get(const nString& object, nString& response)
{
#ifdef __WIN32__
    n_assert(this->httpSession.IsConnected());
    bool res = this->httpSession.SendGetRequest(object, "", response);
    return res;
#else
    return false;
#endif
}

//------------------------------------------------------------------------------
/**
*/
bool
nHttpServer::Post(const nString& object, const nString& postData, nString& response)
{
#ifdef __WIN32__
    n_assert(this->httpSession.IsConnected());
    bool res = this->httpSession.SendPostRequest(object, "", postData, response);
    return res;
#else
    return false;
#endif
}

