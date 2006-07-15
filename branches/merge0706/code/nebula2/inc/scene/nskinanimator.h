#ifndef N_SKINANIMATOR_H
#define N_SKINANIMATOR_H
//------------------------------------------------------------------------------
/**
    @class nSkinAnimator
    @ingroup Scene

    @brief Provide an animated joint skeleton for a nSkinShapeNode.

    On Animate() invocation, the nSkinAnimator will update its joint
    skeleton and invoke SetCharSkeleton() on the calling 
    scene node (which must be a nSkinShapeNode) with a pointer
    to an uptodate nCharSkeleton object.

    See also @ref N2ScriptInterface_nskinanimator
    
    (C) 2003 RadonLabs GmbH
*/
#include "scene/nanimator.h"
#include "character/ncharskeleton.h"
#include "character/ncharacter2.h"
#include "anim2/nanimstate.h"

class nAnimation;
class nAnimationServer;

//------------------------------------------------------------------------------
class nSkinAnimator : public nAnimator
{
public:
    /// constructor
    nSkinAnimator();
    /// destructor
    virtual ~nSkinAnimator();
    /// object persistency
    virtual bool SaveCmds(nPersistServer *ps);
    /// load resources
    virtual bool LoadResources();
    /// unload resources
    virtual void UnloadResources();
    /// called by app when new render context has been created for this object
    virtual void RenderContextCreated(nRenderContext* renderContext);
    /// called by app when render context is going to be released
    virtual void RenderContextDestroyed(nRenderContext* renderContext);
    /// called by scene node objects which wish to be animated by this object
    virtual void Animate(nSceneNode* sceneNode, nRenderContext* renderContext);
    /// begin adding joints
    void BeginJoints(int numJoints);
    /// add a joint to the skeleton
    void SetJoint(int index, int parentJointIndex, const vector3& poseTranslate, const quaternion& poseRotate, const vector3& poseScale, const nString& name);
    /// finish adding joitns
    void EndJoints();
    /// get number of joints in skeleton
    int GetNumJoints();
    /// get joint attributes
    void GetJoint(int index, int& parentJointIndex, vector3& poseTranslate, quaternion& poseRotate, vector3& poseScale, nString& name);
    /// set name of an anim resource
    void SetAnim(const nString& filename);
    /// get name of an anim resource
    const nString& GetAnim() const;

    /// set channel name which delivers the current anim state index
    void SetStateChannel(const char* name);
    /// get anim state channel name 
    const char* GetStateChannel();

    /// begin anim state definition
    void BeginStates(int num);
    /// add an animation state
    void SetState(int stateIndex, int animGroupIndex, float fadeInTime);
    /// set optional state name
    void SetStateName(int stateIndex, const nString& name);
    /// finish adding states
    void EndStates();
    /// get number of states
    int GetNumStates() const;
    /// get state attributes
    const nAnimState& GetStateAt(int stateIndex);

    /// begin adding clips to a state
    void BeginClips(int stateIndex, int numClips);
    /// add an animation clip to a state
    void SetClip(int stateIndex, int clipIndex, const char* weightChannelName);
    /// finish adding clips to a state
    void EndClips(int stateIndex);
    /// get number of animations in a state
    int GetNumClips(int stateIndex) const;
    /// get animation attributes
    void GetClipAt(int stateIndex, int animIndex, const char*& weightChannelName);

    /// begin adding anim event tracks to a clip
    void BeginAnimEventTracks(int stateIndex, int clipIndex, int numTracks);
    /// begin an event track to the current clip
    void BeginAnimEventTrack(int stateIndex, int clipIndex, int trackIndex, const nString& name, int numEvents);
    /// set an anim event in a track
    void SetAnimEvent(int stateIndex, int clipIndex, int trackIndex, int eventIndex, float time, const vector3& translate, const quaternion& rotate, const vector3& scale);
    /// end the current event track
    void EndAnimEventTrack(int stateIndex, int clipIndex, int trackIndex);
    /// end adding animation event tracks to current clip
    void EndAnimEventTracks(int stateIndex, int clipIndex);

    /// enable/disable animation
    void SetAnimEnabled(bool b);
    /// get animation enabled state
    bool IsAnimEnabled() const;

    /// get a joint index by name
    int GetJointByName(const nString& jointName);

    /// get the index of the nCharacter2 pointer used when accessing the render context in Animate()
    int GetCharacterVarIndex();

protected:
    /// load anim resource
    bool LoadAnim();
    /// unload anim resource
    void UnloadAnim();

    nCharacter2 character;
    nAnimStateArray animStateArray;
    nAutoRef<nAnimationServer> refAnimServer;
    nRef<nAnimation> refAnim;
    nString animName;
    int characterVarIndex;
    nVariable::Handle animStateVarHandle;
    nVariable::Handle animRestartVarHandle;
    uint frameId;
    nClass* skinShapeNodeClass;
    nClass* shadowSkinShapeNodeClass;
    bool animEnabled;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nSkinAnimator::SetAnimEnabled(bool b)
{
    this->animEnabled = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nSkinAnimator::IsAnimEnabled() const
{
    return this->animEnabled;
}

//------------------------------------------------------------------------------
/**
    Begin configuring the joint skeleton.
*/
inline
void
nSkinAnimator::BeginJoints(int numJoints)
{
    this->character.GetSkeleton().BeginJoints(numJoints);
}

//------------------------------------------------------------------------------
/**
    Add a joint to the joint skeleton.
*/
inline
void
nSkinAnimator::SetJoint(int jointIndex, int parentJointIndex, const vector3& poseTranslate, const quaternion& poseRotate, const vector3& poseScale, const nString& name )
{
    this->character.GetSkeleton().SetJoint(jointIndex, parentJointIndex, poseTranslate, poseRotate, poseScale, name);
}

//------------------------------------------------------------------------------
/**
    Finish adding joints to the joint skeleton.
*/
inline
void
nSkinAnimator::EndJoints()
{
    this->character.GetSkeleton().EndJoints();
}

//------------------------------------------------------------------------------
/**
    Get number of joints in joint skeleton.
*/
inline
int
nSkinAnimator::GetNumJoints()
{
    return this->character.GetSkeleton().GetNumJoints();
}

//------------------------------------------------------------------------------
/**
    Get joint attributes.
*/
inline
void
nSkinAnimator::GetJoint(int index, int& parentJointIndex, vector3& poseTranslate, quaternion& poseRotate, vector3& poseScale, nString& name)
{
    nCharJoint& joint = this->character.GetSkeleton().GetJointAt(index);
    parentJointIndex = joint.GetParentJointIndex();
    poseTranslate = joint.GetPoseTranslate();
    poseRotate    = joint.GetPoseRotate();
    poseScale     = joint.GetPoseScale();
    name          = joint.GetName();
}

//------------------------------------------------------------------------------
/**
    Set name of anim resource file.
*/
inline
void
nSkinAnimator::SetAnim(const nString& name)
{
    this->UnloadAnim();
    this->animName = name;
}

//------------------------------------------------------------------------------
/**
    Get name of anim resource file.
*/
inline
const nString&
nSkinAnimator::GetAnim() const
{
    return this->animName;
}

//------------------------------------------------------------------------------
/**
    Get a joint index by name.

    @param jointName  the name of the joint index to retrieve.  Returns -1 if joint not found.
*/
inline
int
nSkinAnimator::GetJointByName(const nString& jointName)
{
    return this->character.GetSkeleton().GetJointIndexByName(jointName);
}

//------------------------------------------------------------------------------
#endif
