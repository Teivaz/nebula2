#ifndef N_VECTORANIMATOR_H
#define N_VECTORANIMATOR_H
//------------------------------------------------------------------------------
/**
    @class nVectorAnimator
    @ingroup NebulaSceneSystemAnimators

    Animates a vector attribute of a nAbstrachShaderNode.

    See also @ref N2ScriptInterface_nvectoranimator
    
    (C) 2003 RadonLabs GmbH
*/
#include "scene/nanimator.h"
#include "gfx2/nshader2.h"
#include "util/nipolkeyarray.h"

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
    /// set the name of the vector parameter to manipulate
    void SetVectorName(const char* name);
    /// get the name of the vector parameter to manipulate
    const char* GetVectorName();
    /// add a key 
    void AddKey(float time, const vector4& key);
    /// get number of keys
    int GetNumKeys() const;
    /// get key at
    void GetKeyAt(int index, float& time, vector4& key) const;

private:
    nShader2::Parameter vectorParameter;
    nIpolKeyArray<vector4> keyArray;
};

//------------------------------------------------------------------------------
#endif

