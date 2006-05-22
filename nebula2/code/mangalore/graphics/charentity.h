#ifndef GRAPHICS_CHARENTITY_H
#define GRAPHICS_CHARENTITY_H
//------------------------------------------------------------------------------
/**
    @class Graphics::CharEntity

    A graphics entity specialized for rendering animated characters.
    
    (C) 2003 RadonLabs GmbH
*/
#include "graphics/entity.h"
#include "graphics/charanimeventhandler.h"
#include "character/ncharjoint.h"
#include "character/ncharacter3set.h"

class nCharacter2;

//------------------------------------------------------------------------------
namespace Graphics
{
class CharEntity : public Entity
{
    DeclareRtti;
	DeclareFactory(CharEntity);

public:
    /// constructor
    CharEntity();
    /// destructor
    virtual ~CharEntity();
    /// called when attached to game entity
    virtual void OnActivate();
    /// called when removed from game entity
    virtual void OnDeactivate();
    /// set animation set name to use
    void SetAnimationSet(const nString& n);
    /// get animation set name to use
    const nString& GetAnimationSet() const;
    /// set the current base animation (usually loop anims)
    void SetBaseAnimation(const nString& anim, float timeOffset);
    /// get current base animation
    const nString& GetBaseAnimation() const;    
    /// get base animation duration
    nTime GetBaseAnimationDuration() const;
    /// set the current overlay animation (usually oneshot anims)
    void SetOverlayAnimation(const nString& anim);
    /// get name of currently active overlay animation
    const nString& GetOverlayAnimation() const;
    /// get overlay animation duration
    nTime GetOverlayAnimationDuration() const;
    /// stop current overlay animation
    void StopOverlayAnimation();
    /// return true if overlay animation is currently active
    bool IsOverlayAnimationActive() const;
    /// called before rendering happens
    virtual void OnRenderBefore();
    /// get pointer to our nCharacter2 object
    nCharacter2* GetCharacterPointer();
    /// return joint index by name
    int GetJointIndexByName(const nString& name);
    /// bring the character's skeleton uptodate
    void EvaluateSkeleton();
    /// return joint at index
    nCharJoint* GetJoint(int jointIndex) const;
    /// return a joint's current matrix in model space
    const matrix44& GetJointMatrix(int jointIndex) const;
    /// return true if the Nebula2 character is in a valid state
    bool IsNebulaCharacterInValidState() const;
    /// get animation event handler
    CharAnimEventHandler* GetAnimationEventHandler() const;
    /// set the characterset
    void SetCharacterSet(nString fileName);
    /// is this a character3 ?
    bool HasCharacter3Set() const;
    /// get the character3 set
    nCharacter3Set* GetCharacter3Set();
    
    /// DEBUG : this is just for the MILESTONE, check actorgraphicsproperty.cc
    void LoadNextSkinListInDirectory();

private:
    /// create animation event handler
    virtual void CreateAnimationEventHandler();
    /// cleanup animation event handler
    virtual void CleanupAnimationEventHandler();
    /// update the render context variables
    virtual void UpdateRenderContextVariables();
    /// update the time used for rendering
    void UpdateTime();
    /// set the current base animation by index
    void SetBaseAnimationByIndex(int i, float timeOffset);
    /// get current base animation index
    int GetBaseAnimationIndex() const;
    /// set the current overlay animation by index
    void SetOverlayAnimationByIndex(int i);
    /// get current overlay animation index
    int GetOverlayAnimationIndex() const;
    /// get the number of animations of this character
    int GetNumAnimations() const;

    nString animationSet;

    int curBaseAnimIndex;           // current base animation
    nTime baseAnimStarted;          // timestamp when base animation has been started
    nTime baseAnimOffset;           // animation offset for base animation
    nTime baseAnimDuration;         // duration of one loop of the base animation
    int curOverlayAnimIndex;        // current overlay animation
    nTime overlayAnimStarted;       // timestamp when overlay animation has been started
    nTime overlayAnimDuration;      // duration of overlay animation

    nVariable::Handle animStateVarHandle;
    nVariable::Handle animOffsetVarHandle;
    nVariable::Handle charPointerHandle;        // handle of character pointer local var in RenderContext
    nVariable::Handle animRestartVarHandle;

    CharAnimEventHandler* animEventHandler;
    nCharacter2* nebCharacter;

    int restartOverlayAnim;        // flag indicating to Nebula2 that animation must be restartet

    nString curOverlayAnimName;
    nString curBaseAnimName;

    bool                character3Mode;
    nCharacter3Set      character3Set;
    nVariable::Handle   char3VarHandle;
    nString             char3SetFileName;
    nCharacter3Node*    char3NodePtr;
};

RegisterFactory(CharEntity);

//------------------------------------------------------------------------------
/**
*/
inline
void
CharEntity::SetAnimationSet(const nString& s)
{
    n_assert(s.IsValid());
    this->animationSet = s;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
CharEntity::GetAnimationSet() const
{
    return this->animationSet;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
CharEntity::IsOverlayAnimationActive() const
{
    return (-1 != this->curOverlayAnimIndex);
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
CharEntity::GetOverlayAnimation() const
{
    return this->curOverlayAnimName;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
CharEntity::GetBaseAnimation() const
{
    return this->curBaseAnimName;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
CharEntity::GetBaseAnimationIndex() const
{
    return this->curBaseAnimIndex;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
CharEntity::GetOverlayAnimationIndex() const
{
    return this->curOverlayAnimIndex;
}

//------------------------------------------------------------------------------
/**
*/
inline
nTime
CharEntity::GetOverlayAnimationDuration() const
{
    return this->overlayAnimDuration;
}

//------------------------------------------------------------------------------
/**
*/
inline
nTime
CharEntity::GetBaseAnimationDuration() const
{
    return this->baseAnimDuration;
}

//------------------------------------------------------------------------------
/**
    Returns the pointer to our nCharacter2 object.
*/
inline
nCharacter2*
CharEntity::GetCharacterPointer()
{
    return this->nebCharacter;
}

//------------------------------------------------------------------------------
/**
    Returns the pointer to our animation event handler.
*/
inline
CharAnimEventHandler*
CharEntity::GetAnimationEventHandler() const
{
    return this->animEventHandler;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
CharEntity::HasCharacter3Set() const
{
    return this->character3Mode;
}

//------------------------------------------------------------------------------
/**
*/
inline
nCharacter3Set*
CharEntity::GetCharacter3Set() 
{
    return &this->character3Set;
}

}; // namespace Graphics

//------------------------------------------------------------------------------
#endif    
