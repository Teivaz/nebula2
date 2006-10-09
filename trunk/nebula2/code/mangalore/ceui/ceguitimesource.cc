//------------------------------------------------------------------------------
//  ceui/ceguitimesource.cc
//  (c) 2006 Nebula2 Community
//------------------------------------------------------------------------------
#include "ceui/ceguitimesource.h"
#include "ceui/server.h"

namespace CEUI
{

ImplementRtti(CEUI::CeGuiTimeSource, Game::TimeSource);
ImplementFactory(CEUI::CeGuiTimeSource);

CeGuiTimeSource* CeGuiTimeSource::Singleton = NULL;

//------------------------------------------------------------------------------
/**
*/
CeGuiTimeSource::CeGuiTimeSource() {
    n_assert(Singleton == NULL);
    Singleton = this;
}

//------------------------------------------------------------------------------
/**
*/
CeGuiTimeSource::~CeGuiTimeSource() {
    n_assert(Singleton != NULL);
    Singleton = NULL;
}

//------------------------------------------------------------------------------
/**
    Distributes the current time to the GUI subsystems.
*/
void CeGuiTimeSource::UpdateTime(nTime frameTime) {
    TimeSource::UpdateTime(frameTime);
    CEUI::Server::Instance()->SetTime(time);
    CEUI::Server::Instance()->SetFrameTime(frameTime);
}

//------------------------------------------------------------------------------
/**
    Can never be paused...
*/
void CeGuiTimeSource::Pause() {
    // just return...
}

//------------------------------------------------------------------------------
/**
    Can never be paused...
*/
void CeGuiTimeSource::Continue() {
    // just return...
}

} // namespace Game
