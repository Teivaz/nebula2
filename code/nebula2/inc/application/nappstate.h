#ifndef N_APPSTATE_H
#define N_APPSTATE_H
//------------------------------------------------------------------------------
/**
    @class nAppState
    @ingroup Application

    @brief An application state. Application states completely control
    application behaviour and rendering when they are active.

    State behaviour is implemented by deriving subclasses from nAppState.

    (C) 2004 RadonLabs GmbH
*/
#include "kernel/nroot.h"
#include "kernel/nref.h"

class nApplication;

//------------------------------------------------------------------------------
class nAppState : public nRoot
{
public:
    /// constructor
    nAppState();
    /// destructor
    virtual ~nAppState();
    /// called when state is created
    virtual void OnCreate(nApplication* application);
    /// called when state is becoming active
    virtual void OnStateEnter(const nString& prevState);
    /// called when state is becoming inactive
    virtual void OnStateLeave(const nString& nextState);
    /// called on state to perform state logic
    virtual void OnFrame();
    /// called on state to perform 3d rendering
    virtual void OnRender3D();
    /// called on state to perform 2d rendering
    virtual void OnRender2D();
    /// called before nSceneServer::RenderScene()
    virtual void OnFrameBefore();
    /// called after nSceneServer::RenderScene()
    virtual void OnFrameRendered();

protected:
    nRef<nApplication> app;
};

//------------------------------------------------------------------------------
#endif
