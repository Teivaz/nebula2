//------------------------------------------------------------------------------
//  ngfxobject.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "application/ngfxobject.h"
#include "variable/nvariableserver.h"
#include "scene/nsceneserver.h"

//------------------------------------------------------------------------------
/**
*/
nGfxObject::nGfxObject() :
    time(0.0),
    globalBoxDirty(true)
{
    this->timeHandle = nVariableServer::Instance()->GetVariableHandleByName("time");
    this->renderContext.AddVariable(nVariable(nVariable::Float, this->timeHandle));
}

//------------------------------------------------------------------------------
/**
*/
nGfxObject::~nGfxObject()
{
    if (this->IsValid())
    {
        this->Unload();
    }
}

//------------------------------------------------------------------------------
/**
    Load the shared gfx resource (a Nebula2 .n2 object) from disk and 
    store it under /res.
*/
bool
nGfxObject::Load()
{
    n_assert(!this->resourceName.IsEmpty());
    n_assert(!this->IsValid());
    nKernelServer* kernelServer = nKernelServer::Instance();

    // make sure the "/res" Nebula2 object exists
    nRoot* resourceRoot = kernelServer->Lookup("/res");
    if (0 == resourceRoot)
    {
        resourceRoot = kernelServer->New("nroot", "/res");
    }

    // get name of object
    // Note: objects with the same name but in different directories
    // will be considered the same.
    nString tmpName = this->resourceName.ExtractFileName();
    tmpName.StripExtension();
    // if the resource is already loaded, just increment its refcount and return
    nTransformNode* resNode = (nTransformNode*) resourceRoot->Find(tmpName.Get());
    if (resNode)
    {
        // the resource is already loaded
        this->refTransformNode = resNode;
        resNode->AddRef();
    }
    else
    {
        // the resource must be loaded
        kernelServer->PushCwd(resourceRoot);
        this->refTransformNode = (nTransformNode*) kernelServer->Load(this->resourceName.Get());
        kernelServer->PopCwd();
    }
    n_assert(this->refTransformNode->IsA(kernelServer->FindClass("ntransformnode")));

    // initialize the render context
    if (!this->renderContext.IsValid())
    {
        this->renderContext.SetRootNode(this->refTransformNode.get());
        this->refTransformNode->RenderContextCreated(&(this->renderContext));
    }
    return true;
}

//------------------------------------------------------------------------------
/**
    Unload the shared resource object if it's loaded. If there are other
    gfx objects using the resource, only the reference count of the
    resource will be decremented.
*/
void
nGfxObject::Unload()
{
    if (this->IsValid())
    {
        this->refTransformNode->Release();
        this->refTransformNode.invalidate();
    }
}

//------------------------------------------------------------------------------
/**
    Returns the global bounding box.
*/
const bbox3&
nGfxObject::GetGlobalBox()
{
    // check if the global bounding box needs updating
    if (this->globalBoxDirty)
    {
        this->globalBoxDirty = false;

        // transform the object's local box into global space
        const matrix44& m = this->transform.getmatrix();
        this->globalBox = this->GetLocalBox();
        this->globalBox.transform(m);
    }
    return this->globalBox;
}

//------------------------------------------------------------------------------
/**
    Return the object's visibility status against the given view volume. To
    check against the current view volume, just use the current ViewProjection
    matrix from the nGfxServer2 object.
*/
bool
nGfxObject::IsVisible(const matrix44& viewProj)
{
    const bbox3& globalBox = this->GetGlobalBox();
    bbox3::ClipStatus clipStatus = globalBox.clipstatus(viewProj);
    return (clipStatus != bbox3::Outside);
}

//------------------------------------------------------------------------------
/**
    Render the object. This does no visibility check, you should do this
    beforehand and don't render the object when it's not visible (either
    use the this object's IsVisible() method, or a hierarchical culling
    method, for instance through using the nQuadTree class.
*/
void
nGfxObject::Render()
{
    n_assert(this->IsValid());

    // update render context variables
    this->renderContext.SetTransform(this->transform.getmatrix());
    nSceneServer::Instance()->Attach(&(this->renderContext));
}
