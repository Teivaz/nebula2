#ifndef N_TRANSFORMANIMATOR_H
#define N_TRANSFORMANIMATOR_H
//------------------------------------------------------------------------------
/**
    Animate position, orientation and scaling of a nTransformNode from
    directly defined key arrays.
    
    (C) 2003 RadonLabs GmbH
*/
#ifndef N_ANIMATOR_H
#include "scene/nanimator.h"
#endif

#undef N_DEFINES
#define N_DEFINES nAnimator
#include "kernel/ndefdllclass.h"

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
    virtual AnimatorType GetAnimatorType() const;
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

    static nKernelServer* kernelServer;

private:
    /// a private key class
    class Key
    {
    public:
        /// default constructor
        Key();
        /// constructor
        Key(float t, const vector3& v);

        float time;
        vector3 value;
    };

    /// get an interpolated key
    bool SampleKey(float time, const nArray<Key>& keyArray, vector3& result);

    nArray<Key> posArray;
    nArray<Key> eulerArray;
    nArray<Key> scaleArray;
};

//------------------------------------------------------------------------------
/**
*/
inline
nTransformAnimator::Key::Key() :
    time(0.0f)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nTransformAnimator::Key::Key(float t, const vector3& v) :
    time(t),
    value(v)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nTransformAnimator::AddPosKey(float time, const vector3& key)
{
    Key newKey(time, key);
    this->posArray.PushBack(newKey);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nTransformAnimator::AddEulerKey(float time, const vector3& key)
{
    Key newKey(time, key);
    this->eulerArray.PushBack(newKey);
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
nTransformAnimator::AddScaleKey(float time, const vector3& key)
{
    Key newKey(time, key);
    this->scaleArray.PushBack(newKey);
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
    @param  pos     [out]   the value of the key
*/
inline
void
nTransformAnimator::GetPosKeyAt(int index, float& time, vector3& key) const
{
    const Key& k = this->posArray[index];
    time = k.time;
    key  = k.value;
}

//------------------------------------------------------------------------------
/**
    Obtain a euler key by its index.

    @param  index   [in]    index of key to get
    @param  time    [out]   the time stamp of the key
    @param  pos     [out]   the value of the key
*/
inline
void
nTransformAnimator::GetEulerKeyAt(int index, float& time, vector3& key) const
{
    const Key& k = this->eulerArray[index];
    time = k.time;
    key  = k.value; 
}

//------------------------------------------------------------------------------
/**
    Obtain a scale key by its index.

    @param  index   [in]    index of key to get
    @param  time    [out]   the time stamp of the key
    @param  pos     [out]   the value of the key
*/
inline
void
nTransformAnimator::GetScaleKeyAt(int index, float& time, vector3& key) const
{
    const Key& k = this->scaleArray[index];
    time = k.time;
    key  = k.value;
}

//------------------------------------------------------------------------------
#endif

