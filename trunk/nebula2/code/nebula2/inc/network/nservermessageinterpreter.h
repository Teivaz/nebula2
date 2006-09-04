#ifndef N_NSERVERMESSAGEINTERPRETER_H
#define N_NSERVERMESSAGEINTERPRETER_H
//------------------------------------------------------------------------------
/**
    @class nServerMessageInterpreter

    A server message interpretor for the nbuddyclient

    (C) 2006 RadonLabs GmbH
*/
#include "util/nstream.h"
#include "network/ncommand.h"

//------------------------------------------------------------------------------
class nServerMessageInterpreter
{
public:
    /// constructor
    nServerMessageInterpreter();
    /// destructor
    virtual ~nServerMessageInterpreter();
    /// Executes a query
    void Execute(nStream& query);

    /// response to a request
    void HandleResponse(nStream& query,int& MessageId);

    /// add a command waiting for response
    void AddPendingCommand(nCommand* command);

private:

    /// all server messagehandling
    void HandleMessage(nStream& query);

    nArray<nCommand*> PendingCommands;

};

//------------------------------------------------------------------------------
#endif