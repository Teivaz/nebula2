#ifndef N_UPDATE_H
#define N_UPDATE_H
//------------------------------------------------------------------------------
/**
    @class nUpdate
    @ingroup Network

    derive this class for your buddy update

    (C) 2006 RadonLabs GmbH
*/
#include "kernel/nroot.h"
#include "kernel/nautoref.h"
#include "util/nstream.h"


//------------------------------------------------------------------------------
class nUpdate
{

public:

    enum Status
    {
        UPDATE_PENDING = 0,
        UPDATE_OBSOLETE,
    };

    /// constructor
    nUpdate();
    /// destructor
    virtual ~nUpdate();

    Status GetStatus();
    void SetStatus(Status s);

    nString GetName();

protected:

    friend class nServerMessageInterpreter;

    Status curStatus;

    nString name;

};


//------------------------------------------------------------------------------
/**
*/
inline
nUpdate::Status
nUpdate::GetStatus()
{
    return this->curStatus;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nUpdate::SetStatus(Status s)
{
    this->curStatus = s;
}

//------------------------------------------------------------------------------
/**
*/
inline
nString
nUpdate::GetName()
{
    return this->name;
}


#endif