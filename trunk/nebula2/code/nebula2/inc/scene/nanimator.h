#ifndef N_ANIMATOR_H
#define N_ANIMATOR_H
//------------------------------------------------------------------------------
/**
    @class nAnimator
    @ingroup NebulaSceneSystemAnimators

    @brief Animator nodes manipulate properties of other scene objects.
    
    They are not attached to the scene, instead they are called back by scene
    objects which wish to be manipulated.

    See also @ref N2ScriptInterface_nanimator

    (C) 2003 RadonLabs GmbH
*/
#include "scene/nscenenode.h"
#include "kernel/nautoref.h"
#include "variable/nvariable.h"

class nVariableServer;

//------------------------------------------------------------------------------
class nAnimator : public nSceneNode
{
public:
    enum Type
    {
        InvalidType,    // an invalid type
        Transform,      // a transform animator
        Shader,         // a shader animator
        BlendShape,     // a blend shape animator
    };

    enum LoopType
    {
        Loop,           // looping
        OneShot,        // one shot (clamping)
    };

    /// constructor
    nAnimator();
    /// destructor
    virtual ~nAnimator();
    /// save object to persistent stream
    virtual bool SaveCmds(nPersistServer* ps);
    
    /// return the type of this animator object
    virtual Type GetAnimatorType() const;
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

protected:
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
        case Loop:  
            return "loop"; 

        case OneShot:
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
    if (0 == strcmp(str, "loop")) return Loop;
    else                          return OneShot;
}

//------------------------------------------------------------------------------
#endif

