//----------------------------------------------------------------------------
//  %(appStateNameL)s.h
//
//  (C)%(curYear)s %(author)s
//----------------------------------------------------------------------------
#ifndef %(classPrefixU)s_%(appStateNameNoPrefixU)s_H
#define %(classPrefixU)s_%(appStateNameNoPrefixU)s_H
//----------------------------------------------------------------------------
/**
    @class %(appStateName)s
    @ingroup %(docGroup)s
    @brief %(appStateBriefDoc)s

*/
//----------------------------------------------------------------------------
#include "application/napplication.h"
#include "application/nappstate.h"

//----------------------------------------------------------------------------
class %(appStateName)s : public nAppState
{
public:
    /// constructor
    %(appStateName)s();
    /// destructor
    virtual ~%(appStateName)s();

    /// @name Overrided functions from nAppState
    /// @{
    virtual void OnStateEnter(const nString &prevState);
    virtual void OnRender3D();
    virtual void OnRender2D();
    virtual void OnFrameBefore();
    virtual void OnFrameRendered();
    virtual void OnFrame();
    /// @}

protected:
    matrix44 viewMatrix;

};

//----------------------------------------------------------------------------
#endif // %(classPrefixU)s_%(appStateNameNoPrefixU)s_H

