#ifndef N_CACTOR_H
#define N_CACTOR_H
//------------------------------------------------------------------------------
/**
    @class CCActor
    @ingroup BombsquadBruceContribModule
    @brief The most basic game object that can be triggered.

    (C)	2004 Rafael Van Daele-Hunt
*/
#include "BombsquadBruce/ccroot.h"

//------------------------------------------------------------------------------
class CCActor : public CCRoot
{
public:
    CCActor();
    virtual ~CCActor();
    virtual bool SaveCmds(nPersistServer* persistServer);

    /// Called once per tick to update internal state:
    virtual void Trigger(float dt) { n_assert2( false, "CCActor::Trigger is pure virtual!" ); } // "pure virtual"
};
//------------------------------------------------------------------------------
#endif

