#define N_IMPLEMENTS nIpcServer
#define N_KERNEL
//------------------------------------------------------------------------------
//  nipcserver.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "kernel/nthread.h" 
#include "kernel/nipcserver.h"
#include "util/nhashtable.h"
#include "kernel/nipcminiserver.h"

//------------------------------------------------------------------------------
/**
    The listener thread. Simply creates one nIpcMiniServer object
    for each client which connects.
*/
int 
N_THREADPROC 
n_listener_tfunc(nThread *t)
{
    // tell thread object that we have started
    t->ThreadStarted();

    // get pointer to ipc server object
    nIpcServer *is = (nIpcServer *) t->LockUserData();
    t->UnlockUserData();

    do 
    {
		nIpcMiniServer *ims;
        n_printf("nIpcServer: listening on port %d...\n", ntohs(is->hostAddr.sin_port));
        ims = n_new nIpcMiniServer(is);
        if (ims) 
        {
            if (ims->Listen()) 
            {
                // Eine Verbindung wurde beantragt. Die kann
                // von der Wakeupfunc kommen, deshalb hier den
                // ThreadStopRequested() Status abfragen, ob
                // dem so ist...
                if (t->ThreadStopRequested()) 
                {
                    ims->Ignore();
                    n_printf("nIpcServer: wakeupfunc client connected, shutting down.\n");
                } 
                else 
                {
                    n_printf("nIpcServer: a client has connected.\n");
                }
            }
        }                     
    } while (!t->ThreadStopRequested());

	n_printf("nIpcServer: shutting down listener thread.\n");
    t->ThreadHarakiri();
    return 0;
}

//------------------------------------------------------------------------------
/**
    Weckt den Listener-Thread auf, indem ein Dummy-Socket
    erzeugt wird, der mit dem Listener-Socket Verbindung 
    aufnimmt. Damit sollte accept() aufwachen und ein
    neues nIpcMiniServer-Object erzeugen (ok, das ist etwas 
    Overhead, aber sauber), der Listener-Thread merkt
    dann, dass er sich beenden soll (in ThreadStopRequested()),
    und faehrt dann auch alle nIpcMiniServers runter.

    28-Oct-98   floh    created
    31-Oct-98   floh    Linux reagiert offensichtlich sehr
                        empfindlich darauf, einfach accept() den
                        Socket unterm Arsch wegzuschliessen, also
                        nochmal die traditionelle Methode...
*/
void 
n_listener_wakeup(nThread *t)
{
    sockaddr_in my_addr;
    memset(&my_addr, 0, sizeof(my_addr));
    
    nIpcServer *is = (nIpcServer *) t->LockUserData();
    t->UnlockUserData();

    my_addr.sin_family = AF_INET;
    my_addr.sin_port   = is->hostAddr.sin_port; 

#if defined(__WIN32__)
    my_addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
#elif defined(__LINUX__)
    my_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
#endif

    SOCKET my_sock = socket(AF_INET, SOCK_STREAM, 0);

    if (my_sock != INVALID_SOCKET) 
    {
        int res;
        res = connect(my_sock, (struct sockaddr *) &my_addr, sizeof(my_addr));
        if (res != 0) 
        {
            n_printf("n_listener_wakeup, connection failed!\n");
        }
        shutdown(my_sock, 2);
        closesocket(my_sock);
    }                
}

//------------------------------------------------------------------------------
/**
    31-Oct-98   floh    created
*/
short 
nIpcServer::GetPortNumFromName(const char* portName)
{
    short pnum = ((short)hash(pname.Get(), N_SOCKET_PORTRANGE)) + N_SOCKET_MIN_PORTNUM;
    return pnum;
} 

//------------------------------------------------------------------------------
/**
    28-Oct-98   floh    created
    08-Dec-98   floh    hmm, boese Falle, bind() kam im Fehlerfall
                        mit -98 zurueck..., ich habe auf 1 
                        getestet...
    08-Jun-99   floh    + nThreadSafeList
*/
nIpcServer::nIpcServer(const char *portName)
{
    n_assert(portName);

    this->numMiniServers = 0;
    this->pname = portName;
    memset(&(this->hostAddr), 0, sizeof(this->hostAddr));
            
    // create socket
    this->sock = socket(AF_INET, SOCK_STREAM, 0);
    n_assert(this->sock != INVALID_SOCKET);

    int true_int = 1;
    int res = setsockopt(this->sock, SOL_SOCKET, SO_REUSEADDR, (const char *)&true_int, sizeof(true_int)); 
    n_assert(res != -1);
        
    // Socket an eine Adresse (Portnummer) binden
    this->hostAddr.sin_family = AF_INET;
    this->hostAddr.sin_addr.s_addr = INADDR_ANY;   // die eigene bitte
    memset(&(this->hostAddr.sin_zero), 0, sizeof(this->hostAddr.sin_zero));
	
    bool bound = false;
	int portNum = this->GetPortNumFromName(portName);
	while (!bound) 
    {
	    this->hostAddr.sin_port = htons(portNum);
        res = bind(this->sock, (struct sockaddr *) &(this->hostAddr), sizeof(this->hostAddr));
        if (res != 0) 
        {
            if (portNum < N_SOCKET_MAX_PORTNUM) 
            {
                portNum++;
                this->hostAddr.sin_port = htons(portNum);
            } 
            else 
            {
                n_error("nIpcServer::nIpcServer(): max number of servers started!");
            }
        } 
        else 
        {
            bound = true;
        }
    }     
    
    // Listener Thread starten... 
    this->listenerThread = n_new nThread(n_listener_tfunc,
                                         0,                  // Default-Stacksize
                                         n_listener_wakeup,
                                         &(this->msgList),
                                         (void *) this);
    n_assert(this->listenerThread);
}

//------------------------------------------------------------------------------
/**
    28-Oct-98   floh    created
*/
nIpcServer::~nIpcServer()
{
    n_printf("-> ~nIpcServer()\n");
    
    // der Thread muss vor den Miniservers gekillt werden, weil
    // im Thread noch ein MiniServer "lebt".
    n_delete this->listenerThread;

    // kille alle MiniServers...
    nIpcMiniServer *ims;
    this->miniServerList.Lock();
    while ((ims = (nIpcMiniServer *) this->miniServerList.RemHead())) 
    {
        n_delete ims;
    }
    this->miniServerList.Unlock();
    
    // Thread und Socket killen
    if (this->sock) 
    {
        shutdown(this->sock, 2);
        closesocket(this->sock);
        this->sock = 0;
    }
    
    // alle noch ausstehenden Messages killen
    nMsgNode *nd;
    while ((nd = (nMsgNode *) this->msgList.RemHead())) 
    {
        n_delete nd;
    }
    
    n_printf("<- ~nIpcServer()\n");    
}

//------------------------------------------------------------------------------
/**
    Poll ipc mini servers for new messages. If there are any to process, poll
    will return true.
*/
bool
nIpcServer::Poll()
{
    // poll all our miniservers...
    nIpcMiniServer *ims;
    this->miniServerList.Lock();
    for (ims = (nIpcMiniServer *) this->miniServerList.GetHead();
         ims;
         ims = (nIpcMiniServer *) ims->GetSucc())
    {
        ims->Poll();
    }
    this->miniServerList.Unlock();

    // check for new messages on the msg list...
    this->msgList.Lock();
    nMsgNode* first = (nMsgNode*) this->msgList.GetHead();
    this->msgList.Unlock();

    return (first != 0);
}

//------------------------------------------------------------------------------
/**
    28-Oct-98   floh    created
*/
nMsgNode*
nIpcServer::GetMsg(int& outClientId)
{
    // check if any messages came in...
    this->msgList.Lock();
    nMsgNode* nd = (nMsgNode *) this->msgList.RemHead();
    this->msgList.Unlock();

    // fill outClientId with the client id which has send the message
    if (nd) 
    {
        outClientId = (int) nd->GetPtr();
    }
    else    
    {
        outClientId = -1;
    }
    return nd;
}

//------------------------------------------------------------------------------
/**
    28-Oct-98   floh    created
*/
void 
nIpcServer::ReplyMsg(nMsgNode* nd)
{
    this->listenerThread->ReplyMsg(nd);
}
    
//------------------------------------------------------------------------------
/**
    28-Oct-98   floh    created
*/
bool 
nIpcServer::AnswerMsg(void* buf, int size, int clientId)
{
    n_assert(buf);
    n_assert(size > 0);
    n_assert(clientId >= 0);
    bool retval = false;
    
    // suche den richtigen Client
    nIpcMiniServer *ims;
    this->miniServerList.Lock();
    for (ims = (nIpcMiniServer *) this->miniServerList.GetHead();
         ims;
         ims = (nIpcMiniServer *) ims->GetSucc())
    {
        if (clientId == ims->id) 
        {
            // das ist der richtige Client
            ims->Send(buf, size);
            retval = true;
            break;
        }
    }
    this->miniServerList.Unlock();

    return retval;
}

//------------------------------------------------------------------------------
/**
    Broadcast a message to all clients.
*/
void
nIpcServer::BroadcastMsg(void* buf, int size)
{
    nIpcMiniServer *ims;
    this->miniServerList.Lock();
    for (ims = (nIpcMiniServer *) this->miniServerList.GetHead();
         ims;
         ims = (nIpcMiniServer *) ims->GetSucc())
    {
        ims->Send(buf, size);
    }
    this->miniServerList.Unlock();
}

//------------------------------------------------------------------------------
//  EOF
//------------------------------------------------------------------------------
