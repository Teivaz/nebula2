//------------------------------------------------------------------------------
//  nattachmentnode_main.cc
//  (C) 2004 Megan Fox
//------------------------------------------------------------------------------
#include "scene/nattachmentnode.h"
#include "scene/nsceneserver.h"
#include "gfx2/ngfxserver2.h"

nNebulaScriptClass(nAttachmentNode, "scene::ntransformnode");

//------------------------------------------------------------------------------
/**
*/
nAttachmentNode::nAttachmentNode() :
    isFinalDirty(false),
    isJointSet(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nAttachmentNode::~nAttachmentNode()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Compute the resulting modelview matrix and set it in the scene
    server as current modelview matrix.
*/
bool
nAttachmentNode::RenderTransform(nSceneServer* sceneServer,
                                nRenderContext* renderContext,
                                const matrix44& parentMatrix)
{
    n_assert(sceneServer);
    n_assert(renderContext);

    this->InvokeAnimators(nAnimator::Transform, renderContext);
    this->UpdateFinalTransform();
    if (this->GetLockViewer())
    {
        // if lock viewer active, copy viewer position
        const matrix44& viewMatrix = nGfxServer2::Instance()->GetTransform(nGfxServer2::InvView);
        matrix44 m = this->finalMatrix;
        m.M41 = viewMatrix.M41;
        m.M42 = viewMatrix.M42;
        m.M43 = viewMatrix.M43;
        sceneServer->SetModelTransform(m * parentMatrix);
    }
    else
    {
        // default case
        sceneServer->SetModelTransform(this->finalMatrix * parentMatrix);
    }

    this->isFinalDirty = true;

    return true;

}

//------------------------------------------------------------------------------
/**
    Compute the final transformation matrix for the nAttachmentNode
*/
void
nAttachmentNode::UpdateFinalTransform()
{
    // A joint must be set for this to do anything at all.  If no joint is set, default
    // to the normal transform value (act as an nTransformNode).
    if (!this->isJointSet)
    {
        this->finalMatrix = this->tform.getmatrix();
        return;
    }

    if (this->isFinalDirty)
    {
        // Multiply the local matrix by the joint matrix (which we get through lengthy means via the parent nSkinShapeNode)
        finalMatrix = this->tform.getmatrix() * ((nSkinShapeNode *)this->parent)->GetCharSkeleton()->GetJointAt(this->jointIndex).GetMatrix();

        this->isFinalDirty = false;
    }
}

//------------------------------------------------------------------------------
/**
    Specifies the target joint by name

    @param jointName  the name of the joint index to target
*/
void
nAttachmentNode::SetJointByName(const char *jointName)
{
    //if (strcmp(this->parent->GetClass()->GetName(), "nskinshapenode"))
    //{
    //    n_printf("Error: nAttachmentNode can only function if it is parented by an nSkinShapeNode\n");
    //    return;
    //}

    //kernelServer->PushCwd(this->parent);
    //nSkinAnimator *skinAnimator = (nSkinAnimator *)this->kernelServer->Lookup(((nSkinShapeNode*)this->parent)->GetSkinAnimator());
    //n_assert(skinAnimator);
    //kernelServer->PopCwd();

    //int newIndex = skinAnimator->GetJointByName(jointName);
    //if (newIndex != -1)
    //{
    //    this->jointIndex = newIndex;
    //    this->isJointSet = true;
    //}
    //else
    //{
    //    n_printf("Error: Unable to find joint of name '%s' on parent nSkinAnimator\n", jointName);
    //}
}

//------------------------------------------------------------------------------
/**
    Specifies the target joint by joint index

    @param newIndex  the joint index to target
*/
void
nAttachmentNode::SetJointByIndex(unsigned int newIndex)
{
    if (strcmp(this->parent->GetClass()->GetName(), "nskinshapenode"))
    {
        n_printf("Error: nAttachmentNode can only function if it is parented by an nSkinShapeNode\n");
        return;
    }

    this->jointIndex = newIndex;
    this->isJointSet = true;
}
