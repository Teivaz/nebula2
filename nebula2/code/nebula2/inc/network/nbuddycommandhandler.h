#ifndef N_NBUDDYCOMMANDHANDLER_H
#define N_NBUDDYCOMMANDHANDLER_H
//------------------------------------------------------------------------------
/**
    @class nBuddyCommandHandler

    A Handler for a single command

    (C) 2006 RadonLabs GmbH
*/
#include "kernel/nroot.h"
#include "util/nStream.h"

//------------------------------------------------------------------------------
class nBuddyCommandHandler
{
public:
    /// constructor
    nBuddyCommandHandler();
    /// destructor
    virtual ~nBuddyCommandHandler();
    /// Executes a query
    nString Execute(nStream& query);

private:
    nStream result;

};

//------------------------------------------------------------------------------
#endif