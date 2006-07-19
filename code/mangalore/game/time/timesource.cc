//------------------------------------------------------------------------------
//  timesource.cc
//  (C) 2006 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "game/time/timesource.h"

namespace Attr
{
    DefineString(TimeSourceId);
    DefineFloat(TimeSourceTime);
    DefineFloat(TimeSourceFactor);
    DefineInt(TimeSourceFrameId);
};

namespace Game
{
ImplementRtti(Game::TimeSource, Foundation::RefCounted);
ImplementFactory(Game::TimeSource);

//------------------------------------------------------------------------------
/**
*/
TimeSource::TimeSource() :
    frameTime(0.001f),
    time(0.0),
    pauseCounter(0),
    timeFactor(1.0f),
    frameId(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
TimeSource::~TimeSource()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
TimeSource::OnActivate()
{
    // empty
    this->time = 0.0;
    this->pauseCounter = 0;
}

//------------------------------------------------------------------------------
/**
*/
void
TimeSource::OnDeactivate()
{
    // empty, override in subclass as needed
}

//------------------------------------------------------------------------------
/**
*/
void
TimeSource::OnLoad(Db::Reader* dbReader)
{
    n_assert(0 != dbReader);
    this->time         = dbReader->GetFloat(Attr::TimeSourceTime);
    this->timeFactor   = dbReader->GetFloat(Attr::TimeSourceFactor);
    this->frameId      = dbReader->GetInt(Attr::TimeSourceFrameId);
    this->pauseCounter = 0;
    this->frameTime    = 0.0;
}

//------------------------------------------------------------------------------
/**
*/
void
TimeSource::OnSave(Db::Writer* dbWriter)
{
    n_assert(0 != dbWriter);
    dbWriter->BeginRow();
    dbWriter->SetString(Attr::TimeSourceId, this->GetClassName());
    dbWriter->SetFloat(Attr::TimeSourceTime, float(this->time));
    dbWriter->SetFloat(Attr::TimeSourceFactor, this->timeFactor);
    dbWriter->SetInt(Attr::TimeSourceFrameId, this->frameId);
    dbWriter->EndRow();
}

//------------------------------------------------------------------------------
/**
    This method is called when the internal time should be updated. It will
    be called early in the frame by the TimeManager. Think of this method
    as the time source's OnFrame() method.
*/
void
TimeSource::UpdateTime(nTime frameTime)
{
    this->frameTime = frameTime;
    this->frameId++;
    if (!this->IsPaused())
    {
        this->time += frameTime * this->timeFactor;
    }
}





};