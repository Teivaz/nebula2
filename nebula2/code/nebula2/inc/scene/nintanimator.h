#ifndef N_INTANIMATOR_H
#define N_INTANIMATOR_H
//------------------------------------------------------------------------------
/**
    @class nIntAnimator
    @ingroup SceneAnimators

    @brief Animates a int attribute of a nAbstractShaderNode.

    (C) 2004 RadonLabs GmbH
*/
#include "scene/nanimator.h"
#include "gfx2/nshader2.h"

//------------------------------------------------------------------------------
class nIntAnimator : public nAnimator
{
public:
    /// constructor
    nIntAnimator();
    /// destructor
    virtual ~nIntAnimator();
    /// save object to persistent stream
    virtual bool SaveCmds(nPersistServer* ps);

    /// return the type of this animator object (SHADER)
    virtual Type GetAnimatorType() const;
    /// called by scene node objects which wish to be animated by this object
    virtual void Animate(nSceneNode* sceneNode, nRenderContext* renderContext);
    /// set the name of the vector parameter to manipulate
    void SetVectorName(const char* name);
    /// get the name of the vector parameter to manipulate
    const char* GetVectorName();
    /// add a key 
    void AddKey(float time, const int& key);
    /// get number of keys
    int GetNumKeys() const;
    /// get key at
    void GetKeyAt(int index, float& time, int& key) const;

protected:
    /// a key class with a time and a int key value
    class Key
    {
    public:
        /// default constructor
        Key();
        /// constructor
        Key(float t, const int& v);

        float time;
        int value;
    };
    /// get an interpolated key
    bool SampleKey(float time, const nArray<Key>& keyArray, int& result);

private:
    nShaderState::Param vectorParameter;
    nArray<Key> keyArray;
};

//------------------------------------------------------------------------------
/**
*/
inline
nIntAnimator::Key::Key()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nIntAnimator::Key::Key(float t, const int& i) :
    time(t),
    value(i)
{
    // empty
}

//------------------------------------------------------------------------------
#endif
