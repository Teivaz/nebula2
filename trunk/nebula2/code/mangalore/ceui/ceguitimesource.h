//------------------------------------------------------------------------------
//  ceui/ceguitimesource.h
//  (c) 2006 Nebula2 Community
//------------------------------------------------------------------------------
#ifndef CEUI_CEGUITIMESOURCE_H
#define CEUI_CEGUITIMESOURCE_H

#include "game/time/timesource.h"

namespace CEUI
{

class CeGuiTimeSource : public Game::TimeSource
{
    DeclareRtti;
    DeclareFactory(CeGuiTimeSource);

private:
    static CeGuiTimeSource* Singleton;

public:
    /// constructor
    CeGuiTimeSource();
    /// destructor
    virtual ~CeGuiTimeSource();
    /// get instance pointer
    static CeGuiTimeSource* Instance();
    /// update current time (called by time manager)
    virtual void UpdateTime(nTime frameTime);
    /// pause the time source (increments pause counter)
    virtual void Pause();
    /// resume the time source (decrements pause counter)
    virtual void Continue();
};

//------------------------------------------------------------------------------
/**
*/
inline
CeGuiTimeSource* CeGuiTimeSource::Instance() {
    n_assert(Singleton != NULL);
    return Singleton;
}

} // namespace CEUI
//------------------------------------------------------------------------------
#endif
