#ifndef N_TUTORIALAPP_H
#define N_TUTORIALAPP_H

#include "application/napplication.h"

class nTutorialAppState;

class nTutorialApp : public nApplication
{
public:
    nTutorialApp();
    virtual ~nTutorialApp();

    virtual bool Open();
    
protected:
    nTutorialAppState* gameState;
};

#endif // N_TUTORIALAPP_H
