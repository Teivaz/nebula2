#ifndef N_SKINANIMATOR_H
#define N_SKINANIMATOR_H
//------------------------------------------------------------------------------
/**
    @class nSkinAnimator
    @ingroup NebulaSceneSystemAnimators

    Provide an animated joint skeleton for a nSkinShapeNode. On
    Animate() invocation, the nSkinAnimator will update its joint
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
    /// begin configuring the character skeleton
    void BeginJoints(int numJoints);
    /// add a joint to the skeleton
    void SetJoint(int index, int parentJointIndex, const vector3& poseTranslate, const quaternion& poseRotate, const vector3& poseScale);
    /// finish configuring the joint skeleton
    void EndJoints();
    /// get number of joints in skeleton
    int GetNumJoints();
    /// get joint attributes
    void GetJoint(int index, int& parentJointIndex, vector3& poseTranslate, quaternion& poseRotate, vector3& poseScale);
    /// set name of an anim resource
    void SetAnim(const char* filename);
    /// get name of an anim resource
    const char* GetAnim() const;

    /// set channel name which delivers the current anim state index
    void SetStateChannel(const char* name);
    /// get anim state channel name 
    const char* GetStateChannel();

    /// begin anim state definition
    void BeginStates(int num);
    /// add an animation state
    void SetState(int stateIndex, int animGroupIndex, float fadeInTime);
    /// finish adding states
    void EndStates();
    /// get number of states
    int GetNumStates() const;
    /// get state attributes
    void GetStateAt(int stateIndex, int& animGroupIndex, float& fadeInTime);

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

/*
    /// begin adding hard points
    void BeginHardPoints(int num);
    /// add a hard point
    void AddHardPoint(int index, int jointIndex, const char* pathToNode);
    /// finish defining hard points
    void EndHardPoints();
    /// get number of hard points
    int GetNumHardPoints() const;
    /// get hard point attributes
    void GetHardPoint(int index, int& jointIndex, const char*& pathToNode);
*/

    static nKernelServer* kernelServer;

private:
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
    int frameIdVarIndex;
    nVariable::Handle animStateVarHandle;
    uint frameId;
};

//------------------------------------------------------------------------------
#endif
