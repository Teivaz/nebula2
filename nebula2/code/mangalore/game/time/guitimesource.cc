//------------------------------------------------------------------------------
//  guitimesource.cc
//  (C) 2006 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "game/time/guitimesource.h"
#include "ui/server.h"
#include "gui/nguiserver.h"

namespace Game
{
ImplementRtti(Game::GuiTimeSource, Game::TimeSource);
ImplementFactory(Game::GuiTimeSource);

GuiTimeSource* GuiTimeSource::Singleton = 0;

//------------------------------------------------------------------------------
/**
*/
GuiTimeSource::GuiTimeSource()
{
    n_assert(0 == Singleton);
    Singleton = this;
}

//------------------------------------------------------------------------------
/**
*/
GuiTimeSource::~GuiTimeSource()
{
    n_assert(0 != Singleton);
    Singleton = 0;
}

//------------------------------------------------------------------------------
/**
    Distributes the current time to the GUI subsystems.
*/
void
GuiTimeSource::UpdateTime(nTime frameTime)
{
    TimeSource::UpdateTime(frameTime);

    UI::Server::Instance()->SetTime(this->time);
    UI::Server::Instance()->SetFrameTime(this->frameTime);
    nGuiServer::Instance()->SetTime(this->time);
}

//------------------------------------------------------------------------------
/**
    Can never be paused...
*/
void
GuiTimeSource::Pause()
{
    // just return...
}

//------------------------------------------------------------------------------
/**
    Can never be paused...
*/
void
GuiTimeSource::Continue()
{
    // just return...
}

} // namespace Game
