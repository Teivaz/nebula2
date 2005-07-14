//----------------------------------------------------------------------------
//  nappopenalstate.h
//
//  (C)2005 Kim, Hyoun Woo
//----------------------------------------------------------------------------
#ifndef N_APPOPENALSTATE_H
#define N_APPOPENALSTATE_H
//----------------------------------------------------------------------------
/**
    @class nAppOpenALState
    @ingroup NOpenALDemoContribModule

    @brief An application state of the nopeal demo.

*/
//----------------------------------------------------------------------------
#include "application/napplication.h"
#include "application/nappstate.h"
#include "nopenaldemo/nopenalhelper.h"

//----------------------------------------------------------------------------
class nAppOpenALState : public nAppState
{
public:
    /// constructor
    nAppOpenALState();
    /// destructor
    virtual ~nAppOpenALState();

    /// @name Overrided functions from nAppState
    /// @{
    virtual void OnStateEnter(const nString &prevState);
    /// called when state is becoming inactive
    virtual void OnStateLeave(const nString& nextState);
    virtual void OnRender3D();
    virtual void OnRender2D();
    virtual void OnFrameBefore();
    virtual void OnFrameRendered();
    virtual void OnFrame();
    /// @}

protected:
    matrix44 viewMatrix;

    /// audio listener instance.
    //nListener3    listener;
    /// nopenal helper.
    nOpenALHelper* audioHelper;
    /// ogg sample sound file object.
    nRef<nOpenALObj> oggObject;
    /// wav sample sound file object.
    nRef<nOpenALObj> wavObject;

};

//----------------------------------------------------------------------------
#endif // N_APPOPENALSTATE_H

