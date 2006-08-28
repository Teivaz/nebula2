//----------------------------------------------------------------------------
//  %(appNameL)s_main.cc
//
//  (C)%(curYear)s %(author)s
//----------------------------------------------------------------------------
#include "%(subDirL)s/%(appStateNameL)s.h"

//----------------------------------------------------------------------------
/**
*/
%(appStateName)s::%(appStateName)s() 
{
    
}

//----------------------------------------------------------------------------
/**
*/
%(appStateName)s::~%(appStateName)s()
{
}

//----------------------------------------------------------------------------
/**
*/
void %(appStateName)s::OnStateEnter(const nString &prevState)
{
    //TODO: put state entry code.
    
    GameStateHandler::OnStateEnter(prevState);
}

//----------------------------------------------------------------------------
/**
*/
void %(appStateName)s::OnStateLeave(const nString &nextState)
{
    //TODO: put state leave code.

	GameStateHandler::OnStateLeave(nextState);
}

//----------------------------------------------------------------------------
/**
    Calls simulation stuff.
*/
nString %(appStateName)s::OnFrame()
{
    //TODO: put per-frame code.

	return GameStateHandler::OnFrame();
}

//----------------------------------------------------------------------------
// EOF
//----------------------------------------------------------------------------
