#ifndef N_CACTOR_H
#define N_CACTOR_H
//------------------------------------------------------------------------------
/**
    @class BBActor
    @ingroup BombsquadBruceContribModule
    @brief The most basic game object that can be triggered.

    (C)	2004 Rafael Van Daele-Hunt
*/
#include "BombsquadBruce/bbroot.h"

//------------------------------------------------------------------------------
class BBActor : public BBRoot
{
public:
    BBActor();
    virtual ~BBActor();
    virtual bool SaveCmds(nPersistServer* persistServer);

    /// Called once per tick to update internal state:
    virtual void Trigger(float dt) { n_assert2( false, "BBActor::Trigger is pure virtual!" ); } // "pure virtual"
};
//------------------------------------------------------------------------------
#endif

