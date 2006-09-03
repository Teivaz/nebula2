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
#include "anim2/nanimstateinfo.h"

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
    const nArray<nString>& GetNamesOfLoadedAnimations();
    /// Get names of loaded Variations
    const nArray<nString>& GetNamesOfLoadedVariations();

private:
    /// character set factory function
    virtual nCharacter2Set* CreateCharacterSet();
    /// release character set
    virtual void DeleteCharacterSet(nRenderContext* renderContext);

    nArray<nString> LoadAnimationsFromFolder(const nString& path, nArray<nRef<nMemoryAnimation>>& outAnimArray);
    nArray<nCharJoint> EvaluateVariation(nMemoryAnimation* variation);

    nRef<nCombinedAnimation> animations;
    nArray<nRef<nMemoryAnimation> > variationAnims;
    nArray<nRef<nMemoryAnimation> > animationAnims;
    nArray<nString> animationNames;
    nArray<nString> variationNames;
    nArray<nArray<nCharJoint> > variationJoints;

    int characterVariationVarIndex;
    int currentVariation;
};

//------------------------------------------------------------------------------
/**
*/
inline
const nArray<nString>&
nCharacter3SkinAnimator::GetNamesOfLoadedAnimations()
{
    return this->animationNames;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nArray<nString>&
nCharacter3SkinAnimator::GetNamesOfLoadedVariations()
{
    return this->variationNames;
}

//------------------------------------------------------------------------------
#endif
