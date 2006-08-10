//----------------------------------------------------------------------------
//  %(appStateNameL)s.h
//
//  (C)%(curYear)s %(author)s
//----------------------------------------------------------------------------
#ifndef %(classPrefixU)s_%(appStateNameNoPrefixU)s_H
#define %(classPrefixU)s_%(appStateNameNoPrefixU)s_H
//----------------------------------------------------------------------------
/**
    @class %(appStateName)s
    @ingroup %(docGroup)s
    @brief %(appStateBriefDoc)s

*/
//----------------------------------------------------------------------------
#include "application/app.h"
#include "application/gamestatehandler.h"

//----------------------------------------------------------------------------
class %(appStateName)s : public Application::GameStateHandler
{
public:
    /// constructor
    %(appStateName)s();
    /// destructor
    virtual ~%(appStateName)s();

    /// @name Overrided functions from nAppState
    /// @{
    /// called when the state represented by this state handler is entered
    virtual void OnStateEnter(const nString& prevState);
    /// called when the state represented by this state handler is left
    virtual void OnStateLeave(const nString& nextState);
    /// called each frame as long as state is current, return new state
    virtual nString OnFrame();
    /// @}

protected:

};

//----------------------------------------------------------------------------
#endif // %(classPrefixU)s_%(appStateNameNoPrefixU)s_H

