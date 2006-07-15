#ifndef N_EXITAPPSTATE_H
#define N_EXITAPPSTATE_H
//------------------------------------------------------------------------------
/**
    @class nExitAppState
    @ingroup Application

    @brief This app state will simply ask the application to quit.
    
    (C) 2004 RadonLabs GmbH
*/
#include "application/nappstate.h"

//------------------------------------------------------------------------------
class nExitAppState : public nAppState
{
public:
    /// constructor
    nExitAppState();
    /// destructor
    virtual ~nExitAppState();
    /// called when state is becoming active
    virtual void OnStateEnter(const nString& prevState);
};
//------------------------------------------------------------------------------
#endif
