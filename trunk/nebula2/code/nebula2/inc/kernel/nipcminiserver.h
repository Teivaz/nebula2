#ifndef N_IPCMINISERVER_H
#define N_IPCMINISERVER_H
//------------------------------------------------------------------------------
/**
    A nIpcServer creates one nIpcMiniServer for each connecting client.

    (C) 2002 RadonLabs GmbH
*/
#ifndef N_NODE_H
#include "util/nnode.h"
#endif

//------------------------------------------------------------------------------
class nIpcServer;
class nThread;
class nIpcMiniServer : public nNode 
{
public:
    /// constructor
    nIpcMiniServer(nIpcServer* _server);
    /// destructor
    ~nIpcMiniServer();
    /// listen on socket and wait for new client
    bool Listen();
    /// ignore the new client for any reason
    void Ignore();
    /// check for and pull incoming messages, call this frequently!
    bool Poll();
    /// send a message to the client
    bool Send(void* buf, int bufSize);

private:
    friend class nIpcServer;

    /// close the internal receiver socket
    void CloseRcvrSocket();

    enum 
    {
        RCRVBUFSIZE = 4096,
    };
    
    nIpcServer *server;             // my mother server
    int id;                                      
    SOCKET srvrSocket;
    SOCKET rcvrSocket;
    struct sockaddr_in clientAddr;
    char rcvrBuf[RCRVBUFSIZE];
};

//------------------------------------------------------------------------------
#endif
