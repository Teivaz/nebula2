//------------------------------------------------------------------------------
//  managers/timemanager.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "managers/timemanager.h"
#include "kernel/ntimeserver.h"
#include "db/server.h"
#include "db/reader.h"
#include "db/writer.h"

namespace Managers
{
ImplementRtti(Managers::TimeManager, Game::Manager);
ImplementFactory(Managers::TimeManager);

TimeManager* TimeManager::Singleton = 0;

using namespace Game;

//------------------------------------------------------------------------------
/**
*/
TimeManager::TimeManager() :
    time(0.0)
{
    n_assert(0 == Singleton);
    Singleton = this;
}

//------------------------------------------------------------------------------
/**
*/
TimeManager::~TimeManager()
{
    n_assert(Singleton);
    Singleton = 0;
}

//------------------------------------------------------------------------------
/**
    Activate the time manager. This will create all the standard time
    sources for Mangalore.
*/
void
TimeManager::OnActivate()
{
    // get current global time
    this->time = nTimeServer::Instance()->GetTime();

    // create standard time sources...
    Manager::OnActivate();
}

//------------------------------------------------------------------------------
/**
    Deactivate the time manager.
*/
void
TimeManager::OnDeactivate()
{
    // cleanup time sources...
    while (this->timeSourceArray.Size() > 0)
    {
        this->RemoveTimeSource(this->timeSourceArray[0]);
    }
    Manager::OnDeactivate();
}

//------------------------------------------------------------------------------
/**
    Attach a time source to the time manager. This will invoke
    OnActivate() on the time source.
*/
void
TimeManager::AttachTimeSource(TimeSource* timeSource)
{
    n_assert(timeSource);
    n_assert(!this->timeSourceArray.Find(timeSource));
    timeSource->OnActivate();
    this->timeSourceArray.Append(timeSource);
}

//------------------------------------------------------------------------------
/**
    Remove a time source from the time manager. This will invoke OnDeactivate()
    on the time source.
*/
void
TimeManager::RemoveTimeSource(TimeSource* timeSource)
{
    n_assert(timeSource);
    int index = this->timeSourceArray.FindIndex(timeSource);
    n_assert(-1 != index);
    this->timeSourceArray[index]->OnDeactivate();
    this->timeSourceArray.Erase(index);
}

//------------------------------------------------------------------------------
/**
    Returns number of time sources attached to the time manager.
*/
int
TimeManager::GetNumTimeSources() const
{
    return this->timeSourceArray.Size();
}

//------------------------------------------------------------------------------
/**
    Gets pointer to time source object by index.
*/
TimeSource*
TimeManager::GetTimeSourceByIndex(int index) const
{
    return this->timeSourceArray[index];
}

//------------------------------------------------------------------------------
/**
    Get pointer to time source object by class name, returns 0 if not found.
*/
TimeSource*
TimeManager::GetTimeSourceByClassName(const nString& n) const
{
    n_assert(n.IsValid());
    int i;
    for (i = 0; i < this->timeSourceArray.Size(); i++)
    {
        if (this->timeSourceArray[i]->GetClassName() == n)
        {
            return this->timeSourceArray[i];
        }
    }
    return 0;
}

//------------------------------------------------------------------------------
/**
    Checks whether the TimeSources table exists in the database, if
    yes invokes OnLoad() on all time sources...
*/
void
TimeManager::OnLoad()
{
    Db::Server* dbServer = Db::Server::Instance();
    if (dbServer->HasTable("TimeSources"))
    {
        // create a db reader...
        Ptr<Db::Reader> dbReader = Db::Reader::Create();
        dbReader->SetTableName("TimeSources");
        if (dbReader->Open())
        {
            int rowIndex;
            int numRows = dbReader->GetNumRows();
            for (rowIndex = 0; rowIndex < numRows; rowIndex++)
            {
                dbReader->SetToRow(rowIndex);
                TimeSource* timeSource = this->GetTimeSourceByClassName(dbReader->GetString(Attr::TimeSourceId));
                if (timeSource)
                {
                    timeSource->OnLoad(dbReader);
                }
            }
            dbReader->Close();
        }
    }
}

//------------------------------------------------------------------------------
/**
    Ask all time sources to save their status to the database.
*/
void
TimeManager::OnSave()
{
    Ptr<Db::Writer> dbWriter = Db::Writer::Create();
    dbWriter->SetTableName("TimeSources");
    dbWriter->SetPrimaryKey(Attr::TimeSourceId);
    if (dbWriter->Open())
    {
        int i;
        for (i = 0; i < this->timeSourceArray.Size(); i++)
        {
            this->timeSourceArray[i]->OnSave(dbWriter);
        }
        dbWriter->Close();
    }
}

//------------------------------------------------------------------------------
/**
    Reset all time sources. This is usually called at the beginning
    of an application state.
*/
void
TimeManager::ResetAll()
{
    int i;
    for (i = 0; i < this->timeSourceArray.Size(); i++)
    {
        this->timeSourceArray[i]->Reset();
    }
}

//------------------------------------------------------------------------------
/**
    Pause all time sources. NOTE: there's an independent pause counter inside each
    time source, a pause just increments the counter, a continue decrements
    it, when the pause counter is != 0 it means, pause is activated.
*/
void
TimeManager::PauseAll()
{
    int i;
    for (i = 0; i < this->timeSourceArray.Size(); i++)
    {
        this->timeSourceArray[i]->Pause();
    }
}

//------------------------------------------------------------------------------
/**
    Unpause all time sources.
*/
void
TimeManager::ContinueAll()
{
    int i;
    for (i = 0; i < this->timeSourceArray.Size(); i++)
    {
        this->timeSourceArray[i]->Continue();
    }
}

//------------------------------------------------------------------------------
/**
    Update all time sources. This method is called very early in the frame
    by the current application state handler. This will get the current frame time
    and call UpdateTime() on all attached time sources.
*/
void
TimeManager::Update()
{
    // compute the current frame time
    nTimeServer* timeServer = nTimeServer::Instance();
    timeServer->Trigger();
    nTime curTime = timeServer->GetTime();
    nTime frameTime = curTime - this->time;
    if (frameTime <= 0.0)
    {
        frameTime = 0.0001f;
    }
    else if (frameTime > 0.25f)
    {
        frameTime = 0.25f;
    }
    this->time = curTime;

    // update all time sources
    int i;
    for (i = 0; i < this->timeSourceArray.Size(); i++)
    {
        this->timeSourceArray[i]->UpdateTime(frameTime);
    }
}

} // namespace Managers
