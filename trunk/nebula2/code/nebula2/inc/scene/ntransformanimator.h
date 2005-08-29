#ifndef N_TRANSFORMANIMATOR_H
#define N_TRANSFORMANIMATOR_H
//------------------------------------------------------------------------------
/**
    @class nTransformAnimator
    @ingroup Scene

    @brief Animate position, orientation and scaling of a nTransformNode from
    directly defined key arrays.

    See also @ref N2ScriptInterface_ntransformanimator
    
    (C) 2003 RadonLabs GmbH
*/
#include "scene/nanimator.h"
#include "util/nanimkeyarray.h"

//------------------------------------------------------------------------------
class nTransformAnimator : public nAnimator
{
public:
    /// constructor
    nTransformAnimator();
    /// destructor
    virtual ~nTransformAnimator();
    /// save object to persistent stream
    virtual bool SaveCmds(nPersistServer* ps);

    /// return the type of this animator object (TRANSFORM)
    virtual Type GetAnimatorType() const;
    /// called by scene node objects which wish to be animated by this object
    virtual void Animate(nSceneNode* sceneNode, nRenderContext* renderContext);
    /// add a position key
    void AddPosKey(float time, const vector3& key);
    /// add a euler angle key
    void AddEulerKey(float time, const vector3& key);
    /// add a scale key
    void AddScaleKey(float time, const vector3& key);
    /// get number of position keys
    int GetNumPosKeys() const;
    /// get position key at index
    void GetPosKeyAt(int index, float& time, vector3& key) const;
    /// get number of euler angle keys
    int GetNumEulerKeys() const;
    /// get euler key at index
    void GetEulerKeyAt(int index, float& time, vector3& key) const;
    /// get number of scale keys
    int GetNumScaleKeys() const;
    /// get scale key at index
    void GetScaleKeyAt(int index, float& time, vector3& key) const;

private:
    nAnimKeyArray<nAnimKey<vector3> > posArray;
    nAnimKeyArray<nAnimKey<vector3> > eulerArray;
    nAnimKeyArray<nAnimKey<vector3> > scaleArray;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nTransformAnimator::AddPosKey(float time, const vector3& key)
{
    nAnimKey<vector3> newKey(time, key);
    this->posArray.Append(newKey);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nTransformAnimator::AddEulerKey(float time, const vector3& key)
{
    nAnimKey<vector3> newKey(time, key);
    this->eulerArray.Append(newKey);
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
nTransformAnimator::AddScaleKey(float time, const vector3& key)
{
    nAnimKey<vector3> newKey(time, key);
    this->scaleArray.Append(newKey);
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nTransformAnimator::GetNumPosKeys() const
{
    return this->posArray.Size();
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nTransformAnimator::GetNumEulerKeys() const
{
    return this->eulerArray.Size();
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nTransformAnimator::GetNumScaleKeys() const
{
    return this->scaleArray.Size();
}

//------------------------------------------------------------------------------
/**
    Obtain a position key by its index.

    @param  index   [in]    index of key to get
    @param  time    [out]   the time stamp of the key
    @param  key     [out]   the value of the key
*/
inline
void
nTransformAnimator::GetPosKeyAt(int index, float& time, vector3& key) const
{
    const nAnimKey<vector3>& k = this->posArray[index];
    time = k.GetTime();
    key  = k.GetValue();
}

//------------------------------------------------------------------------------
/**
    Obtain a euler key by its index.

    @param  index   [in]    index of key to get
    @param  time    [out]   the time stamp of the key
    @param  key     [out]   the value of the key
*/
inline
void
nTransformAnimator::GetEulerKeyAt(int index, float& time, vector3& key) const
{
    const nAnimKey<vector3>& k = this->eulerArray[index];
    time = k.GetTime();
    key  = k.GetValue();
}

//------------------------------------------------------------------------------
/**
    Obtain a scale key by its index.

    @param  index   [in]    index of key to get
    @param  time    [out]   the time stamp of the key
    @param  key     [out]   the value of the key
*/
inline
void
nTransformAnimator::GetScaleKeyAt(int index, float& time, vector3& key) const
{
    const nAnimKey<vector3>& k = this->scaleArray[index];
    time = k.GetTime();
    key  = k.GetValue();
}

//------------------------------------------------------------------------------
#endif

