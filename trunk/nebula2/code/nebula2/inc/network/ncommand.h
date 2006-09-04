#ifndef N_COMMAND_H
#define N_COMMAND_H
//------------------------------------------------------------------------------
/**
    @class nCommand
    @ingroup Network

    derive this class your buddy command

    (C) 2006 RadonLabs GmbH
*/
#include "kernel/nroot.h"
#include "kernel/nautoref.h"
#include "util/nstream.h"


//------------------------------------------------------------------------------
class nCommand
{

public:

    enum Status
    {
        COMMAND_DISABLED = 0,
        COMMAMD_PENDING,
        COMMAMD_RESULT,
        COMMAMD_ERROR,
    };

    /// constructor
    nCommand();
    /// destructor
    virtual ~nCommand();

    virtual bool Execute() = 0;
    virtual void EvaluateResult(nStream& result);

    Status GetStatus();
    const nString& GetRequest();

    void Disable();

    int GetId();

protected:

    friend class nBuddyClient;

    Status curStatus;
    nString xmlRequest;
    int uniqueID;
private:
     static int CommandCounter;

};



//------------------------------------------------------------------------------
/**
*/
inline
void
nCommand::Disable()
{
    this->curStatus = COMMAND_DISABLED;
}

//------------------------------------------------------------------------------
/**
*/
inline
nCommand::Status
nCommand::GetStatus()
{
    return this->curStatus;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nCommand::GetId()
{
    return this->uniqueID;
}



//------------------------------------------------------------------------------
/**
*/
inline
const nString&
nCommand::GetRequest()
{
    return this->xmlRequest;
}

#endif