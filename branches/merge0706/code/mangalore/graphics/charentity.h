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
    /// set animation mapping to use
    void SetAnimationMapping(const nString& n);
    /// get animation mapping to use
    const nString& GetAnimationMapping() const;
    /// set the current base animation (usually loop anims)
    void SetBaseAnimation(const nString& anim, nTime fadeIn = 0.0, nTime timeOffset = 0.0, bool onlyIfInactive = true);
    /// set the current base animation mix (usually loop anims)
    void SetBaseAnimationMix(const nArray<nString>& anims, const nArray<float>& weights, nTime fadeIn = 0.0, nTime timeOffset = 0.0);
    /// get current base animation (first of mix)
    nString GetBaseAnimation() const;    
    /// get base animation duration
    nTime GetBaseAnimationDuration() const;
    /// set the current overlay animation (usually oneshot anims)
    void SetOverlayAnimation(const nString& anim, nTime fadeIn = 0.0, nTime overrideDuration = 0.0, bool onlyIfInactive = true);
    /// set the current overlay animation mix (usually one shot anims)
    void SetOverlayAnimationMix(const nArray<nString>& anims, const nArray<float>& weights, nTime fadeIn = 0.0, nTime overrideDuration = 0.0f);
    /// get name of currently active overlay animation (first of mix)
    nString GetOverlayAnimation() const;
    /// get overlay animation duration
    nTime GetOverlayAnimationDuration() const;
    /// stop current overlay animation
    void StopOverlayAnimation(nTime fadeIn);
    /// return true if overlay animation is currently active
    bool IsOverlayAnimationActive() const;
    /// called before rendering happens
    virtual void OnRenderBefore();
    /// get pointer to our nCharacter2 object
    nCharacter2* GetCharacterPointer() const;
    /// return joint index by name
    int GetJointIndexByName(const nString& name);
    /// bring the character's skeleton uptodate
    void EvaluateSkeleton();
    /// return joint at index
    nCharJoint* GetJoint(int jointIndex) const;
    /// return a joint's current matrix in model space
    const matrix44& GetJointMatrix(int jointIndex) const;
    /// get animation event handler
    CharAnimEventHandler* GetAnimationEventHandler() const;
    /// is this a character3 ?
    bool HasCharacter3Set() const;
    /// set the character 3 set to load
    void LoadCharacter3Set(const nString& fileName);
    /// get the character set
    nCharacter2Set* GetCharacterSet() const;

private:
    /// create animation event handler
    virtual void CreateAnimationEventHandler();
    /// cleanup animation event handler
    virtual void CleanupAnimationEventHandler();
    /// update the render context variables
    virtual void UpdateRenderContextVariables();
    /// update the time used for rendering
    void UpdateTime();
    /// activate animations
    void ActivateAnimations(const nArray<nString>& animNames, const nArray<float>& animWeights, nTime fadeIn);
    /// get the number of animations of this character
    int GetNumAnimations() const;
    /// find first instance of a node
    nRoot* FindFirstInstance(nRoot* node, nClass* classType);

    nString animMapping;                ///< name of the animation mapping

    nArray<nString> baseAnimNames;      ///< names of current base animations
    nArray<float> baseAnimWeights;      ///< weights of the base animations
    nTime baseAnimStarted;              ///< timestamp when base animation has been started
    nTime baseAnimOffset;               ///< animation offset for base animation
    nTime baseAnimDuration;             ///< duration of one loop of the base animation
    nTime baseAnimFadeIn;
    bool activateNewBaseAnim;

    nArray<nString> overlayAnimNames;   ///< names of current overlay animations
    nArray<float> overlayAnimWeights;   ///< weights of the overlay animations
    nTime overlayAnimStarted;           ///< timestamp when overlay animation has been started
    nTime overlayAnimDuration;          ///< duration of overlay animation
    int restartOverlayAnim;             ///< flag indicating to Nebula2 that animation must be restartet
    nTime overlayEndFadeIn;             ///< fade in time when overlay animation has finished

    nVariable::Handle characterHandle;      ///< handle of character pointer local var in RenderContext
    nVariable::Handle characterSetHandle;   ///< handle of character set pointer local var in RenderContext
    nVariable::Handle animOffsetVarHandle;
    nVariable::Handle animRestartVarHandle;

    nCharacter2* nebCharacter;
    nCharacter2Set* characterSet;
    CharAnimEventHandler* animEventHandler;

    bool                character3Mode;
    nString             character3SetFileName;
    nCharacter3Node*    character3NodePtr;
};

RegisterFactory(CharEntity);

//------------------------------------------------------------------------------
/**
*/
inline
void
CharEntity::SetAnimationMapping(const nString& name)
{
    this->animMapping = name;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
CharEntity::GetAnimationMapping() const
{
    return this->animMapping;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
CharEntity::IsOverlayAnimationActive() const
{
    return !this->overlayAnimNames.Empty();
}

//------------------------------------------------------------------------------
/**
*/
inline
nString
CharEntity::GetOverlayAnimation() const
{
    return this->overlayAnimNames.Empty() ? "" : this->overlayAnimNames[0];
}

//------------------------------------------------------------------------------
/**
*/
inline
nString
CharEntity::GetBaseAnimation() const
{
    return this->baseAnimNames.Empty() ? "" : this->baseAnimNames[0];
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
CharEntity::GetCharacterPointer() const
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
nCharacter2Set*
CharEntity::GetCharacterSet() const
{
    return this->characterSet;
}

} // namespace Graphics
//------------------------------------------------------------------------------
#endif
