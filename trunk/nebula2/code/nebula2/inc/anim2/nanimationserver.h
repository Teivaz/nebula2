#ifndef N_ANIMATIONSERVER_H
#define N_ANIMATIONSERVER_H
//------------------------------------------------------------------------------
/**
    @class nAnimationServer
    @ingroup NebulaAnimationSystem

    The animation server offers central services of the animation subsystem
    (at the moment it is merely a factory for nAnimation objects).
    
    (C) 2003 RadonLabs GmbH
*/
#ifndef N_ROOT_H
#include "kernel/nroot.h"
#endif

#ifndef N_AUTOREF_H
#include "kernel/nautoref.h"
#endif

#undef N_DEFINES
#define N_DEFINES nAnimationServer
#include "kernel/ndefdllclass.h"

class nResourceServer;
class nAnimation;

//------------------------------------------------------------------------------
class nAnimationServer : public nRoot
{
public:
    /// constructor
    nAnimationServer();
    /// destructor
    virtual ~nAnimationServer();
    /// create an in-memory animation object (always shared)
    virtual nAnimation* NewMemoryAnimation(const char* rsrcName);
    /// create a new streaming animation object (never shared)
    virtual nAnimation* NewStreamingAnimation();

    static nKernelServer* kernelServer;

private:
    nAutoRef<nResourceServer> refResourceServer;
};
//------------------------------------------------------------------------------
#endif
