#define N_IMPLEMENTS nScriptServer
#define N_KERNEL
//--------------------------------------------------------------------
//  nscriptrun.cc
//  (C) 1998 Andre Weissflog
//
//  29-Jun-02   floh    globally disabled the ipc stuff for Nomads
//                      (needs to go into the kernel anyway)
//--------------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "kernel/nscriptserver.h"

//--------------------------------------------------------------------
//  RunInteractive()
//  31-Oct-98   floh    uebernommen aus nTclServer
//--------------------------------------------------------------------
bool nScriptServer::RunInteractive(void)
{
	n_error("nScriptServer::RunInteractive(): pure virtual function called!");
	return false;
}	

//--------------------------------------------------------------------
//  Prompt()
//  23-Feb-99   floh    created
//--------------------------------------------------------------------
char *nScriptServer::Prompt(char *, int)
{
    n_error("pure virtual function called!");
    return NULL;
}

//--------------------------------------------------------------------
//  RunServer()
//
//	Gehe in Server-Modus ueber...
//
//  31-Oct-98   floh    created
//--------------------------------------------------------------------
bool nScriptServer::RunServer(const char *pname)
{
	bool retval = false;
	n_printf("going into server mode, portname: %s\n",pname);
	if (this->InitIpc(pname)) {
		while (this->is_server) {
			this->WaitIpc();
			this->FlushIpc();
		}
		retval = true;
		this->KillIpc();
	}
	n_printf("server mode left.\n");
	return retval;
}

//--------------------------------------------------------------------
//	InitIpc()
//
//	Initialisiere IPC-Server-Objekt.
//
//	31-Oct-98	floh	created
//--------------------------------------------------------------------
bool nScriptServer::InitIpc(const char *pname)
{
/*
	n_assert(pname);
	n_assert(NULL == this->ipc_server);
	this->is_server = false;
	if ((this->ipc_server = n_new nIpcServer(pname))) {
		this->is_server = true;
	}
	return this->is_server;
*/
    return true;
}

//--------------------------------------------------------------------
//	KillIpc()
//
//	Kille IPC-Server-Objekt.
//
//	31-Oct-98	floh	created
//--------------------------------------------------------------------
void nScriptServer::KillIpc(void)
{
/*
	n_assert(this->ipc_server);
	n_delete this->ipc_server;
	this->ipc_server = NULL;
	this->is_server  = false;
*/
}

//--------------------------------------------------------------------
//	WaitIpc()
//
//	Warte auf IPC Message.
//
//	31-Oct-98	floh	created
//--------------------------------------------------------------------
void nScriptServer::WaitIpc(void)
{
/*
	n_assert(this->ipc_server);
	this->ipc_server->WaitMsg();
*/
}

//--------------------------------------------------------------------
//	FlushIpc()
//
//	Hole alle anstehenden IPC Messages, evaluiere sie und
//	schicke das Resultat zurueck.
//
//	31-Oct-98	floh	created
//--------------------------------------------------------------------
void nScriptServer::FlushIpc(void)
{
/*
	n_assert(this->ipc_server);
	nMsgNode *nd;
	long client_id;
	while ((nd = this->ipc_server->GetMsg(&client_id))) {
		char *cmd = (char *) nd->GetMsgPtr();
		const char *result;
		this->Run(cmd, result);
		this->ipc_server->ReplyMsg(nd);
		this->ipc_server->AnswerMsg((void *)result,strlen(result)+1,client_id);
	}
*/
}

//--------------------------------------------------------------------
//	Connect()
//
//	Nimm Verbindung mit einem Server auf (alle Builtin Kommandos
//	werden fortan an den Server geschickt).
//
//	31-Oct-98	floh	created
//--------------------------------------------------------------------
bool nScriptServer::Connect(const char *pname)
{
/*
	n_assert(pname);
	n_assert(NULL == this->ipc_client);
	n_printf("connecting to %s...\n",pname);
	this->is_connected = false;
	if ((this->ipc_client = n_new nIpcClient())) {
		if (this->ipc_client->Connect(pname)) {
			this->replybuf_size = 1<<15;
			this->replybuf = n_new char[this->replybuf_size];
			this->is_connected = true;
		} else {
            n_delete this->ipc_client;
            this->ipc_client = NULL;
	    }
    }
	if (!this->is_connected) n_printf("failed.\n");
	return this->is_connected;
*/
    return true;
}

//--------------------------------------------------------------------
//	Disconnect()
//
//  Trennt Verbindung mit Server. Falls [cancel] true, wird
//  die Verbindung ohne Handshake abgebrochen, das sollte
//  nur gemacht werden, wenn der Server schon tot ist.
//
//	31-Oct-98	floh	created
//--------------------------------------------------------------------
void nScriptServer::Disconnect(bool cancel)
{
/*
	n_assert(this->ipc_client);
	n_assert(this->replybuf);
	if (!cancel) this->ipc_client->Disconnect();
	n_delete this->replybuf;	
	this->replybuf 		= NULL;
	this->replybuf_size = 0;
	n_delete this->ipc_client;
	this->ipc_client 	= NULL;
	this->is_connected	= false;
*/
}

//--------------------------------------------------------------------
//	SendMsg()
//
//	Sende eine Message im Client-Mode.	
//
//	31-Oct-98	floh	created
//  01-Nov/98   floh    disconnected jetzt, wenn Server nicht
//                      mehr erreichbar
//--------------------------------------------------------------------
const char *nScriptServer::SendMsg(const char *msg)
{
/*
	n_assert(this->ipc_client);
	ulong msg_len = strlen(msg)+1;
	char *result = NULL;
	if (msg_len > 1) {
		nMsgNode *nd = this->ipc_client->SendMsg((void *)msg,msg_len);
		if (nd) {
			char *reply_str = (char *) nd->GetMsgPtr();
			strncpy(this->replybuf,reply_str,this->replybuf_size);
  			this->replybuf[this->replybuf_size-1] = 0;
			this->ipc_client->FreeReplyMsgNode(nd);
			result = this->replybuf;
		} else {
            n_printf("Server died, disconnecting.\n");
            this->Disconnect(true); // harter Disconnect ohne Handshake
        }
	}
	return result;
*/
    return "";
}			

//--------------------------------------------------------------------
//  Trigger()
//
//  Trigger sollte in regelmaessigen Abstaenden aufgerufen werden,
//  wenn der ScriptServer nicht im interaktiven Modus ist. Dadurch
//  wird sichergestellt dass irgendwelche abzuarbeitenden Events
//  auch mal drankommen...
//  Wenn alles OK ist, kommt die Routine true zurueck, wenn
//  der User die App beenden moechte, kommt false zurueck.
//
//  21-Dec-98   floh    created
//  31-Aug-99   floh    'quit_requested' wird zurueckgesetzt, wenn
//                      es einmal true war
//--------------------------------------------------------------------
bool nScriptServer::Trigger(void)
{
    // wenn im Server-Modus, IPC-Port flushen
/*
    if (this->is_server) this->FlushIpc();
*/

    bool retval = this->quit_requested;
    this->quit_requested = false;
    return !retval;
}

//--------------------------------------------------------------------
//	EOF
//--------------------------------------------------------------------


