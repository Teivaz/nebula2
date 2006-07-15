#ifndef N_CHARACTER3SKINANIMATOR_H
#define N_CHARACTER3SKINANIMATOR_H
//------------------------------------------------------------------------------
/**
    @class nCharacter3SkinAnimator
    @ingroup Scene

    (C) 2005 RadonLabs GmbH
*/
#include "scene/nskinanimator.h"
#include "scene/nanimator.h"
#include "anim2/nmemoryanimation.h"
#include "anim2/ncombinedanimation.h"
#include "character/ncharskeleton.h"
#include "character/ncharacter2.h"
#include "anim2/nanimstate.h"

class nAnimation;
class nAnimationServer;

//------------------------------------------------------------------------------
class nCharacter3SkinAnimator : public nSkinAnimator
{
public:
    /// constructor
    nCharacter3SkinAnimator();
    /// destructor
    virtual ~nCharacter3SkinAnimator();
    /// load resources
    virtual bool LoadResources();
    /// unload resources
    virtual void UnloadResources();
    /// called by scene node objects which wish to be animated by this object
    virtual void Animate(nSceneNode* sceneNode, nRenderContext* renderContext);

    /// Get names of loaded Animations
    nArray<nString> GetNamesOfLoadedAnimations();
    /// Get names of loaded Variations
    nArray<nString> GetNamesOfLoadedVariations();
    /// set the index for the rendercontext
    void SetCharacterSetIndexHandle(int handle);

private:
        
    nArray<nString> LoadAnimationsFromFolder(nString path,nArray<nRef<nMemoryAnimation> > &animArray);
    nArray<nCharJoint> EvaluteVariation(nRef<nMemoryAnimation> variation);

    nCombinedAnimation* variations;
    nCombinedAnimation* animations;
    nArray<nRef<nMemoryAnimation> > variationAnims;
    nArray<nRef<nMemoryAnimation> > animationAnims;
    nArray<nString> animationNames;
    nArray<nString> variationNames;
    nArray<nArray<nCharJoint> > variationJoints;

    int characterSetIndex;
    int characterVariationVarIndex;
    int currentVariation;
};

//------------------------------------------------------------------------------
#endif
