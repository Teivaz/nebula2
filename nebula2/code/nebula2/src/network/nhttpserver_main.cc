//------------------------------------------------------------------------------
//  nhttpserver_main.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "network/nhttpserver.h"

nNebulaScriptClass(nHttpServer, "nroot");

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
}

//------------------------------------------------------------------------------
/**
*/
void
nHttpServer::Disconnect()
{   
    if (this->httpSession.IsConnected())
    {
        this->httpSession.Disconnect();
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
nHttpServer::IsConnected() const
{
    return this->httpSession.IsConnected();
}

//------------------------------------------------------------------------------
/**
*/
bool
nHttpServer::Get(const nString& object, nString& response)
{
    n_assert(this->httpSession.IsConnected());
    bool res = this->httpSession.SendGetRequest(object, "", response);
    return res;
}

//------------------------------------------------------------------------------
/**
*/
bool
nHttpServer::Post(const nString& object, const nString& postData, nString& response)
{
    n_assert(this->httpSession.IsConnected());
    bool res = this->httpSession.SendPostRequest(object, "", postData, response);
    return res;
}
