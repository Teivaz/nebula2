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
#include "kernel/nroot.h"
#include "kernel/nautoref.h"

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
