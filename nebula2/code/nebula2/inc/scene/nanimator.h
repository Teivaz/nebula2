#ifndef N_ANIMATOR_H
#define N_ANIMATOR_H
//------------------------------------------------------------------------------
/**
    @class nAnimator
    @ingroup NebulaSceneSystemAnimators

    Animator nodes manipulate properties of other scene objects. They are
    not attached to the scene, instead they are called back by scene objects
    which wish to be manipulated.

    See also @ref N2ScriptInterface_nanimator

    (C) 2003 RadonLabs GmbH
*/
#ifndef N_SCENENODE_H
#include "scene/nscenenode.h"
#endif

#ifndef N_AUTOREF_H
#include "kernel/nautoref.h"
#endif

#ifndef N_VARIABLE_H
#include "variable/nvariable.h"
#endif

#undef N_DEFINES
#define N_DEFINES nAnimator
#include "kernel/ndefdllclass.h"

class nVariableServer;

//------------------------------------------------------------------------------
class nAnimator : public nSceneNode
{
public:
    enum AnimatorType
    {
        INVALID_TYPE,   // an invalid type
        TRANSFORM,      // a transform animator
        SHADER,         // a shader animator
    };

    enum LoopType
    {
        LOOP,           // looping
        ONESHOT,        // one shot (clamping)
    };

    /// constructor
    nAnimator();
    /// destructor
    virtual ~nAnimator();
    /// save object to persistent stream
    virtual bool SaveCmds(nPersistServer* ps);
    
    /// return the type of this animator object
    virtual AnimatorType GetAnimatorType() const;
    /// called by scene node objects which wish to be animated by this object
    virtual void Animate(nSceneNode* sceneNode, nRenderContext* renderContext);
    /// set the variable handle which drives this animator object (e.g. time)
    void SetChannel(const char* name);
    /// get the variable which drives this animator object
    const char* GetChannel();
    /// set the loop type
    void SetLoopType(LoopType t);
    /// get the loop type
    LoopType GetLoopType() const;
    /// convert loop type to string
    static const char* LoopTypeToString(LoopType t);
    /// convert string to loop type
    static LoopType StringToLoopType(const char* str);

    static nKernelServer* kernelServer;

protected:
    nAutoRef<nVariableServer> refVariableServer;
    LoopType loopType;
    nVariable::Handle channelVarHandle;
};

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nAnimator::LoopTypeToString(LoopType t)
{
    switch (t)
    {
        case LOOP:      
            return "loop"; 

        case ONESHOT:   
        default:
            return "oneshot";
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
nAnimator::LoopType
nAnimator::StringToLoopType(const char* str)
{
    n_assert(str);
    if (0 == strcmp(str, "loop"))
    {
        return LOOP;
    }
    else
    {
        return ONESHOT;
    }
}

//------------------------------------------------------------------------------
#endif

