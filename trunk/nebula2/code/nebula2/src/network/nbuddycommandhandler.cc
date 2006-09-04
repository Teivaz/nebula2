//------------------------------------------------------------------------------
//  nBuddyCommandHandler.cc
//  (C) 2006 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "network/nBuddyCommandHandler.h"

//nNebulaClass(nBuddyCommandHandler, "nroot");

//------------------------------------------------------------------------------
/**
*/
nBuddyCommandHandler::nBuddyCommandHandler()
{
   this->result.SetFilename("MyData");
   this->result.OpenString("");
}

//------------------------------------------------------------------------------
/**
*/
nBuddyCommandHandler::~nBuddyCommandHandler()
{
    // empty
}


//------------------------------------------------------------------------------
/**
    Executes a query and returns the result
*/
nString nBuddyCommandHandler::Execute(nStream& query)
{
    nString Result;
    if (query.HasAttr("ver"))
    {

    }
    else return "";

    return query.GetCurrentNodeName();
}
