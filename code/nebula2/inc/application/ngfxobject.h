#ifndef N_GFXOBJECT_H
#define N_GFXOBJECT_H
//------------------------------------------------------------------------------
/**
    @class nGfxObject
    @ingroup Application

    @brief A wrapper object for loading and rendering a Nebula2 graphics
    object.
    
    (C) 2004 RadonLabs GmbH
*/
#include "kernel/ntypes.h"
#include "util/nstring.h"
#include "mathlib/transform44.h"
#include "mathlib/bbox.h"
#include "scene/nrendercontext.h"
#include "scene/ntransformnode.h"

//------------------------------------------------------------------------------
class nGfxObject
{
public:
    /// constructor
    nGfxObject();
    /// destructor
    ~nGfxObject();
    /// load resources associated with gfx object
    bool Load();
    /// unload resources associated with gfx object
    void Unload();
    /// return true if gfx object is currently valid
    bool IsValid() const;
    /// set the resource name
    void SetResourceName(const nString& n);
    /// get the resource name
    const nString& GetResourceName() const;
    /// set current time
    void SetTime(nTime t);
    /// get current time
    nTime GetTime() const;
    /// access to render context
    nRenderContext& RenderContext();
    /// access to transform object
    transform44& Transform();
    /// get local bounding box
    const bbox3& GetLocalBox();
    /// get global bounding box
    const bbox3& GetGlobalBox();
    /// return the visibity status against a view volume
    bool IsVisible(const matrix44& viewProj);
    /// render the object
    void Render();

private:
    nString resourceName;
    nRef<nTransformNode> refTransformNode;
    nRenderContext renderContext;
    transform44 transform;
    nVariable::Handle timeHandle;
    nTime time;
    bbox3 globalBox;
    bool globalBoxDirty;
};

//------------------------------------------------------------------------------
/**
*/
inline
bool
nGfxObject::IsValid() const
{
    return this->refTransformNode.isvalid();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGfxObject::SetResourceName(const nString& n)
{
    this->resourceName = n;
    this->globalBoxDirty = true;
}

//------------------------------------------------------------------------------
/*
*/
inline
const nString&
nGfxObject::GetResourceName() const
{
    return this->resourceName;
}

//------------------------------------------------------------------------------
/*
*/
inline
void
nGfxObject::SetTime(nTime t)
{
    this->time = t;
}

//------------------------------------------------------------------------------
/*
*/
inline
nTime
nGfxObject::GetTime() const
{
    return this->time;
}

//------------------------------------------------------------------------------
/*
*/
inline
nRenderContext&
nGfxObject::RenderContext()
{
    return this->renderContext;
}

//------------------------------------------------------------------------------
/*
*/
inline
transform44&
nGfxObject::Transform()
{
    return this->transform;
}

//------------------------------------------------------------------------------
/*
*/
inline
const bbox3&
nGfxObject::GetLocalBox()
{
    if (this->IsValid())
    {
        return this->refTransformNode->GetLocalBox();
    }
    else
    {
        static const bbox3 dummyBox;
        return dummyBox;
    }
}

//------------------------------------------------------------------------------
#endif    
