//-----------------------------------------------------------------------------
//  nmaxskinanimator.cc
//
//  (c)2004 Kim, Hyoun Woo
//-----------------------------------------------------------------------------
#include "export2/nmax.h"
#include "export2/nmaxoptions.h"
#include "export2/nmaxnotetrack.h"
#include "export2/nmaxbones.h"
#include "export2/nmaxskinanimator.h"
#include "pluginlibs/nmaxdlg.h"
#include "pluginlibs/nmaxlogdlg.h"

#include "scene/nskinanimator.h"

//-----------------------------------------------------------------------------
/**
*/
nMaxSkinAnimator::nMaxSkinAnimator()
{
}

//-----------------------------------------------------------------------------
/**
*/
nMaxSkinAnimator::~nMaxSkinAnimator()
{
}

//-----------------------------------------------------------------------------
/**
*/
nAnimator* nMaxSkinAnimator::Export(const char* animatorName, const char* animFilename)
{
    n_assert(animatorName);
    n_assert(animFilename);

    nSkinAnimator* animator = (nSkinAnimator*)this->CreateNebulaObject("nskinanimator", animatorName);

    if (animator)
    {
        // build joints from bone array of the scene.
        nMaxBoneManager* boneManager = nMaxBoneManager::Instance();
        nArray<nMaxBoneManager::Bone>& boneArray = boneManager->GetBoneArray();

        this->BuildJoints(animator, boneArray);

        animator->SetChannel("time");
        animator->SetLoopType(nAnimLoopType::Loop);

        animator->SetAnim(animFilename);
        animator->SetStateChannel("chnCharState");

        // build animation states.
        nMaxNoteTrack& noteTrack = boneManager->GetNoteTrack();
        this->BuildAnimStates(animator, noteTrack);
    }
    else
    {
        n_maxlog(Error, "Failed to create a nskinanimator.");
    }

    return animator;
}

//-----------------------------------------------------------------------------
/**
    Specifies joints of skin animator.

    - 01-Mar-05 kims added joint name export option.
*/
void nMaxSkinAnimator::BuildJoints(nSkinAnimator* animator, 
                                   const nArray<nMaxBoneManager::Bone> &boneArray)
{
    Matrix3 localTM;
    AffineParts ap;

    int numJoints = nMaxBoneManager::Instance()->GetNumBones();

    // build joints.
    animator->BeginJoints(numJoints);

    for (int i=0; i<numJoints; i++)
    {
        const nMaxBoneManager::Bone &bone = nMaxBoneManager::Instance()->GetBone(i);

        localTM = bone.localTransform;

        // transform scale.
        float scale = nMaxOptions::Instance()->GetGeomScaleValue();
        if (scale != 0.0f)
        {
            Point3 scaleVal(scale, scale, scale);

            Matrix3 scaleTM;
            scaleTM.IdentityMatrix();
            scaleTM.Scale(scaleVal);

            localTM = localTM * scaleTM;
        }

        localTM.NoScale();

        decomp_affine(localTM, &ap);

        vector3 poseTranlator (-ap.t.x, ap.t.z, ap.t.y);
        quaternion poseRotate (-ap.q.x, ap.q.z, ap.q.y, -ap.q.w);
        vector3 poseScale (ap.k.x, ap.k.z, ap.k.y);

        animator->SetJoint(bone.id, 
                           bone.parentID,
                           poseTranlator,
                           poseRotate,
                           poseScale,
                           bone.name.Get());
    }

    animator->EndJoints();
}

//-----------------------------------------------------------------------------
/**
    Build animation states and clips.
*/
void nMaxSkinAnimator::BuildAnimStates(nSkinAnimator* animator, nMaxNoteTrack& noteTrack)
{
    int numStates = noteTrack.GetNumStates();

    animator->BeginStates(numStates);

    for (int j=0; j<numStates; j++)
    {
        const nMaxAnimState& state = noteTrack.GetState(j);

        animator->SetState(j, j, state.fadeInTime);
        animator->SetStateName(j, state.name);

        int numClips = state.clipArray.Size();
        animator->BeginClips(j, numClips);

        for (int k=0; k<numClips; k++)
        {
            const nString& weightChannelName = state.GetClip(k);
            animator->SetClip(j, k, weightChannelName.Get());
        }

        animator->EndClips(j);
    }

    animator->EndStates();
}
