#ifndef N_NBUDDYCOMMANDINTERPRETER_H
#define N_NBUDDYCOMMANDINTERPRETER_H
//------------------------------------------------------------------------------
/**
    @class nBuddyCommandInterpreter

    A Commandinterpretor for the nbuddyserver

    (C) 2006 RadonLabs GmbH
*/
#include "kernel/nroot.h"
#include "util/nstream.h"

//------------------------------------------------------------------------------
class nBuddyCommandInterpreter : public nRoot
{
public:
    /// constructor
    nBuddyCommandInterpreter();
    /// destructor
    virtual ~nBuddyCommandInterpreter();
    /// Executes a query
    void Execute(nString& strResult,nStream& query,int& ClientID);

private:

    /// all user operations
    void CreateUser(nString& strResult,nStream& query,int& MessageID);
    void LoginUser(nString& strResult,nStream& query,int& ClientID,int& MessageID);


    void AddBuddy(nString& strResult,nStream& query,int& ClientID,int& MessageID);
    void GetBuddyList(nString& strResult,nStream& query,int& ClientID,int& MessageID);
    void TransferMessage(nString& strResult,nStream& query,int& ClientID,int& MessageID);

    void SetProfileAttr(nString& strResult,nStream& query,int& ClientID,int& MessageID);
    void GetProfileAttr(nString& strResult,nStream& query,int& ClientID,int& MessageID);



    /// Generate common not logged in message
    void GenerateNotLoggedInMessage(nStream& result,int& MessageID);



};

//------------------------------------------------------------------------------
#endif