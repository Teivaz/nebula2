#ifndef N_UVANIMATOR_H
#define N_UVANIMATOR_H
//------------------------------------------------------------------------------
/**
    @class nUvAnimator
    @ingroup SceneAnimators

    @brief Animates the UV transform parameters on an nAbstractShaderNode.
    
    See also @ref N2ScriptInterface_nuvanimator

    (C) 2004 RadonLabs GmbH
*/
#include "scene/nanimator.h"
#include "gfx2/ngfxserver2.h"
#include "util/nipolkeyarray.h"

//------------------------------------------------------------------------------
class nUvAnimator : public nAnimator
{
public:
    /// constructor
    nUvAnimator();
    /// destructor
    virtual ~nUvAnimator();
    /// save object to persistent stream
    virtual bool SaveCmds(nPersistServer* ps);

    /// return the type of this animator object (SHADER)
    virtual Type GetAnimatorType() const;
    /// called by scene node objects which wish to be animated by this object
    virtual void Animate(nSceneNode* sceneNode, nRenderContext* renderContext);
    /// add a position key
    void AddPosKey(uint texLayer, float time, const vector2& key);
    /// add a euler angle key
    void AddEulerKey(uint texLayer, float time, const vector2& key);
    /// add a scale key
    void AddScaleKey(uint texLayer, float time, const vector2& key);
    /// get number of position keys
    int GetNumPosKeys(uint texLayer) const;
    /// get position key at index
    void GetPosKeyAt(uint texLayer, uint keyIndex, float& time, vector2& key) const;
    /// get number of euler angle keys
    int GetNumEulerKeys(uint texLayer) const;
    /// get euler key at index
    void GetEulerKeyAt(uint texLayer, uint keyIndex, float& time, vector2& key) const;
    /// get number of scale keys
    int GetNumScaleKeys(uint texLayer) const;
    /// get scale key at index
    void GetScaleKeyAt(uint texLayer, uint keyIndex, float& time, vector2& key) const;
    
private:
    nIpolKeyArray<vector2> posArray[nGfxServer2::MaxTextureStages];
    nIpolKeyArray<vector2> eulerArray[nGfxServer2::MaxTextureStages];
    nIpolKeyArray<vector2> scaleArray[nGfxServer2::MaxTextureStages];
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nUvAnimator::AddPosKey(uint texLayer, float time, const vector2& key)
{
    n_assert(texLayer < nGfxServer2::MaxTextureStages);
    this->posArray[texLayer].AddKey(time, key);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nUvAnimator::AddEulerKey(uint texLayer, float time, const vector2& key)
{
    n_assert(texLayer < nGfxServer2::MaxTextureStages);
    this->eulerArray[texLayer].AddKey(time, key);
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
nUvAnimator::AddScaleKey(uint texLayer, float time, const vector2& key)
{
    n_assert(texLayer < nGfxServer2::MaxTextureStages);
    this->scaleArray[texLayer].AddKey(time, key);
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nUvAnimator::GetNumPosKeys(uint texLayer) const
{
    n_assert(texLayer < nGfxServer2::MaxTextureStages);
    return this->posArray[texLayer].GetNumKeys();
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nUvAnimator::GetNumEulerKeys(uint texLayer) const
{
    n_assert(texLayer < nGfxServer2::MaxTextureStages);
    return this->eulerArray[texLayer].GetNumKeys();
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nUvAnimator::GetNumScaleKeys(uint texLayer) const
{
    n_assert(texLayer < nGfxServer2::MaxTextureStages);
    return this->scaleArray[texLayer].GetNumKeys();
}

//------------------------------------------------------------------------------
/**
    Obtain a position key by its index.

    @param  texLayer    [in]    texture layer index
    @param  keyIndex    [in]    index of key to get
    @param  time        [out]   the time stamp of the key
    @param  key         [out]   the value of the key
*/
inline
void
nUvAnimator::GetPosKeyAt(uint texLayer, uint keyIndex, float& time, vector2& key) const
{
    n_assert(texLayer < nGfxServer2::MaxTextureStages);
    this->posArray[texLayer].GetKeyAt(keyIndex, time, key);
}

//------------------------------------------------------------------------------
/**
    Obtain a euler key by its index.

    @param  texLayer    [in]    texture layer index
    @param  keyIndex    [in]    index of key to get
    @param  time        [out]   the time stamp of the key
    @param  key         [out]   the value of the key
*/
inline
void
nUvAnimator::GetEulerKeyAt(uint texLayer, uint keyIndex, float& time, vector2& key) const
{
    n_assert(texLayer < nGfxServer2::MaxTextureStages);
    this->eulerArray[texLayer].GetKeyAt(keyIndex, time, key);
}

//------------------------------------------------------------------------------
/**
    Obtain a scale key by its index.

    @param  texLayer    [in]    texture layer index
    @param  keyIndex    [in]    index of key to get
    @param  time        [out]   the time stamp of the key
    @param  key         [out]   the value of the key
*/
inline
void
nUvAnimator::GetScaleKeyAt(uint texLayer, uint keyIndex, float& time, vector2& key) const
{
    n_assert(texLayer < nGfxServer2::MaxTextureStages);
    this->scaleArray[texLayer].GetKeyAt(keyIndex, time, key);
}
//------------------------------------------------------------------------------
#endif

