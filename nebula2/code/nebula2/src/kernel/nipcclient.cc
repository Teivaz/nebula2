#define N_IMPLEMENTS nIpcClient
#define N_KERNEL
//------------------------------------------------------------------------------
//  nipcclient.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "kernel/nipcclient.h"

//------------------------------------------------------------------------------
/**
*/
nIpcClient::nIpcClient() :
    serverPortNum(0),
    sock(0)
{
#   ifdef __WIN32__
	struct WSAData wsa_data;
	WSAStartup(0x101, &wsa_data);
#   endif

    memset(&(this->serverAddr), 0, sizeof(this->serverAddr));
}

//------------------------------------------------------------------------------
/**
*/
nIpcClient::~nIpcClient()
{
    if (this->sock) 
    {
        shutdown(this->sock, 2);
        closesocket(this->sock);
        this->sock = 0;
    }

#   ifdef __WIN32__
    WSACleanup();
#   endif
}

//------------------------------------------------------------------------------
/**
    Der uebergebene Name hat folgenden Aufbau:
  
    [hostname:]portname 

    Hostname ist optional und nur notwendig, wenn
    sich die Adresse auf einem anderen Rechner befindet.
    Statt eines Namens kann auch eine TCP/IP Adresse
    angegeben sein.
    "portname" bezeichnet den Namen des Ports, der auf
    "hostname" angesprochen werden soll.
  
    Die Routine fuellt die [server_addr] Struktur in
    this aus.
*/
bool 
nIpcClient::FillServerAddr(const char* name)
{
    n_assert(name);

    char nameBuf[128];
    char *hostname = NULL;
    char *portname = NULL;
    char *tmp;
    struct hostent *he;
    
    // split host and port name
    n_strncpy2(nameBuf, name, sizeof(nameBuf));
    if ((tmp = strchr(nameBuf, ':'))) 
    {
        *tmp = 0;
        hostname = nameBuf;
        portname = tmp + 1;
    } else {
        hostname = "localhost";
        portname = nameBuf;
    }

    this->serverHostName = hostname;
    this->serverPortName = portname;
    n_assert(hostname[0] != 0);
    n_assert(portname[0] != 0);
    
    // Hostname und initiale Portnummer eintragen
    he = gethostbyname(hostname);
    if (!he) {
        n_printf("nIpcClient(): unknown host!");
        return FALSE;
    }
    this->serverPortNum = this->GetPortNumFromName(portname);
    this->serverAddr.sin_family = AF_INET;
    this->serverAddr.sin_port   = htons(this->serverPortNum);
    this->serverAddr.sin_addr.S_un.S_addr = inet_addr(hostname);
    this->serverAddr.sin_addr   = *((struct in_addr *)he->h_addr);
    return true;
}

//------------------------------------------------------------------------------
/**
*/
short 
nIpcClient::GetPortNumFromName(const char *portName)
{
    n_assert(portName);
    short pnum = ((short)hash(portName, N_SOCKET_PORTRANGE)) + N_SOCKET_MIN_PORTNUM;
    return pnum;
} 

//------------------------------------------------------------------------------
/**
    28-Oct-98   floh    created
    31-Oct-98   floh    falls ein Port nicht zuhoerte, wurde kein
                        neuer Socket aufgemacht, was dazu fuehrte,
                        dass sich die Routine in einer Endlosschleife
                       aufhing, weil keine Connection mehr geklappt
                        hatte.
*/
bool 
nIpcClient::Connect(const char* portName)
{
    n_assert(this->sock == 0);
    bool retval = false;
   
    // resolve portname
    if (this->FillServerAddr(portName)) 
    {
        int res;
        char msg_buf[128];
        bool connected = false;
        bool error     = false;
        int num_retries = 0;
        
        while ((!connected) && (!error) && (num_retries < 4)) 
        {    
            // falls Retry, alten Socket killen
            if (this->sock) 
            {
				shutdown(this->sock,2);
                closesocket(this->sock);
                this->sock = 0;
                num_retries++;
            }
            
            // einen neuen Socket initialisieren                
        	this->sock = socket(AF_INET, SOCK_STREAM, 0);
            if (this->sock == INVALID_SOCKET) 
            {
		        n_printf("nIpcClient::nIpcClient(): could not create socket!");
		        return FALSE;		
            }

            n_printf("nIpcClient: trying %s:%s, port %d...\n",
                      this->serverHostName.Get(), this->serverPortName.Get(),
                      this->serverPortNum);

            res = connect(this->sock, (struct sockaddr *) &(this->serverAddr),
                          sizeof(struct sockaddr));
			if (res != -1) 
            {
				// eine Verbindung steht, richtiger Port?
                sprintf(msg_buf, "~handshake %s", this->serverPortName.Get());
        	    res = send(this->sock, msg_buf, strlen(msg_buf)+1, 0);
            
            	// warte auf Antwort...
            	if (res != -1) 
                {
                	n_printf("nIpcClient: sending handshake... ");
                	res = recv(this->sock, msg_buf, sizeof(msg_buf), 0);
                    if ((res==-1)||(res==0)) 
                    {
                    	n_printf("nIpcClient::Connect(): ~handshake recv() failed!\n");
                    	error = true;
                    } 
                    else 
                    {                    
                    	// positive Antwort?
                    	if (strcmp(msg_buf,"~true") == 0) 
                        {
							connected=TRUE;
							n_printf("accepted.\n");
                    	} 
                        else 
                        {
                        	// falscher Portname, neue probieren...
                        	this->serverPortNum++;
                        	this->serverAddr.sin_port = htons(this->serverPortNum);
							n_printf("wrong portname.\n");
                    	}
                	}
            	} 
                else 
                {
                	n_printf("nIpcClient::Connect(): ~handshake send() failed!");
                	error = TRUE;
            	}
        	} 
            else 
            {
				// auf diesem Port hoert niemand zu, naechste Portnummer...
              	this->serverPortNum++;
               	this->serverAddr.sin_port = htons(this->serverPortNum);
			}
		}
        if (connected) 
        {
            n_printf("nIpcClient: connected.\n");
            retval = true;
        } 
        else 
        {
            n_printf("nIpcClient: Could not connect to a valid server!\n");
            shutdown(this->sock,2);
			closesocket(this->sock);
			this->sock = 0;
		}
    }
    return retval;
}                                                                  
        
//------------------------------------------------------------------------------
/**
*/
void 
nIpcClient::Disconnect()
{
    n_assert(this->sock);
    int res;
    char *cmd = "~close";
    
    // sende ~close Befehl
    res = send(this->sock, cmd, strlen(cmd)+1, 0);
    if (res != -1) 
    {
        // Antwort abholen
        char buf[128];
        res = recv(this->sock, buf, sizeof(buf), 0);
        if ((res == -1) || (res == 0)) 
        {
            n_printf("nIpcClient::Disconnect(): recv(~close) failed!\n");    
        }
    } 
    else 
    {
        n_printf("nIpcClient::Disconnect(): send(~close) failed!\n");
    } 

    // kill the socket
    shutdown(this->sock,2);
    closesocket(this->sock);
    this->sock = 0;
}

//------------------------------------------------------------------------------
/**
*/
nMsgNode*
nIpcClient::SendMsg(void* buf, int size)
{
	n_assert(buf);
	n_assert(size > 0);

    nMsgNode *nd = NULL;
    int res;
    res = send(this->sock, (char *) buf, size, 0);
    if (res == -1) 
    {
        n_printf("nIpcClient::SendMsg(): send() failed!");
    } 
    else 
    {
        // wait for an answer...
        res = recv(this->sock, this->receiveBuffer, RECEIVEBUFFERSIZE, 0);
        if ((res == -1) || (res == 0)) 
        {
            n_printf("nIpcClient::SendMsg(): recv() failed!\n");
        } 
        else if (res > 0) 
        {
            // baue eine nMsgNode mit dem Ergebnis
            nd = new nMsgNode(this->receiveBuffer, res);
        }
    }
    return nd;
}

//------------------------------------------------------------------------------
/**
*/
void 
nIpcClient::FreeReplyMsgNode(nMsgNode *nd)
{
    delete nd;
}

//------------------------------------------------------------------------------
/**
*/
const char*
nIpcClient::GetHostName() const
{
    return this->serverHostName.IsEmpty() ? 0 : this->serverHostName.Get();
}

//------------------------------------------------------------------------------
/**
*/
const char*
nIpcClient::GetPortName() const
{
    return this->serverPortName.IsEmpty() ? 0 : this->serverPortName.Get();
}

