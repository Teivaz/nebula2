#ifndef N_VECTORANIMATOR_H
#define N_VECTORANIMATOR_H
//------------------------------------------------------------------------------
/**
    @class nVectorAnimator

    Animates a vector attribute of a nAbstrachShaderNode.

    See also @ref N2ScriptInterface_nvectoranimator
    
    (C) 2003 RadonLabs GmbH
*/
#ifndef N_ANIMATORNODE_H
#include "scene/nanimator.h"
#endif

#undef N_DEFINES
#define N_DEFINES nVectorAnimator
#include "kernel/ndefdllclass.h"

//------------------------------------------------------------------------------
class nVectorAnimator : public nAnimator
{
public:
    /// constructor
    nVectorAnimator();
    /// destructor
    virtual ~nVectorAnimator();
    /// save object to persistent stream
    virtual bool SaveCmds(nPersistServer* ps);

    /// return the type of this animator object (SHADER)
    virtual AnimatorType GetAnimatorType() const;
    /// called by scene node objects which wish to be animated by this object
    virtual void Animate(nSceneNode* sceneNode, nRenderContext* renderContext);
    /// set the name of the vector variable to manipulate
    void SetVectorName(const char* name);
    /// get the name of the vector variable to manipulate
    const char* GetVectorName();
    /// add a key 
    void AddKey(float time, const vector4& key);
    /// get number of keys
    int GetNumKeys() const;
    /// get key at
    void GetKeyAt(int index, float& time, vector4& key) const;

    static nKernelServer* kernelServer;

private:
    class Key
    {
    public:
        /// default constructor
        Key();
        /// constructor
        Key(float t, const vector4& v);

        float time;
        vector4 value;
    };

    nVariable::Handle vectorVarHandle;
    nArray<Key> keyArray;
};

//------------------------------------------------------------------------------
/**
*/
inline
nVectorAnimator::Key::Key()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nVectorAnimator::Key::Key(float t, const vector4& v) :
    time(t),
    value(v)
{
    // empty
}

//------------------------------------------------------------------------------
#endif

