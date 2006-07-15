#ifndef N_HTTPSERVER_H
#define N_HTTPSERVER_H
//------------------------------------------------------------------------------
/**
    @class nHttpServer
    @ingroup Http

    A Nebula2 server object which implements a simple Http client
    (confusing I know ;).
    
    (C) 2004 RadonLabs GmbH
*/
#include "kernel/nroot.h"
#ifdef __WIN32__
#include "util/nhttpsession.h"
#endif

//------------------------------------------------------------------------------
class nHttpServer : public nRoot
{
public:
    /// constructor
    nHttpServer();
    /// destructor
    virtual ~nHttpServer();
    /// open a http connection
    bool Connect(const nString& serverName);
    /// close the current http connection
    void Disconnect();
    /// return true if currently connected
    bool IsConnected() const;
    /// send a GET request to the server
    bool Get(const nString& object, nString& result);
    /// send a POST request to the server
    bool Post(const nString& object, const nString& postData, nString& result);

private:
#ifdef __WIN32__
    nHttpSession httpSession;
#endif
};

//------------------------------------------------------------------------------
#endif    

