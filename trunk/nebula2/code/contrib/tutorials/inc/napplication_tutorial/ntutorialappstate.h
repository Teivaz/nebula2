#ifndef N_TUTORIALAPPSTATE_H
#define N_TUTORIALAPPSTATE_H

#include "application/nappstate.h"
#include "application/ngfxobject.h"
#include "application/nappcamera.h"

class nTutorialAppState: public nAppState
{
public:
    nTutorialAppState();
    virtual ~nTutorialAppState();
    virtual void OnStateEnter(const nString& prevState);
    virtual void OnRender3D();
    virtual void OnRender2D();
    virtual void OnFrame();
    
protected:
    nGfxObject* world;
    nGfxObject* testObject;
};

#endif // N_TUTORIALAPPSTATE_H
