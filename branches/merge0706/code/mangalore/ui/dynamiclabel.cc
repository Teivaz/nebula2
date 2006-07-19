//------------------------------------------------------------------------------
//  ui/dynamiclabel.cc
//  (C) 2006 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "ui/dynamiclabel.h"
#include "ui/server.h"
#include "foundation/factory.h"
#include "graphics/lightentity.h"
#include "kernel/nfileserver2.h"
#include "game/time/guitimesource.h"

namespace UI
{
ImplementRtti(UI::DynamicLabel, UI::Label);
ImplementFactory(UI::DynamicLabel);

//------------------------------------------------------------------------------
/**
*/
DynamicLabel::DynamicLabel()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
DynamicLabel::~DynamicLabel()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    This creates a graphics entity representing the visual of the dynamic label
    and converts the resource name into a Nebula2 node pointer.
*/
void
DynamicLabel::OnCreate(UI::Element* parent)
{
    // labels must have a parent
    n_assert(0 != parent);

    // create graphics entity, first check if it actually exists...
    n_assert(this->resourceName.IsValid());
    nString resPath;
    resPath.Format("gfxlib:%s.n2", this->resourceName.Get());
    if (nFileServer2::Instance()->FileExists(resPath))
    {
	    this->graphicsEntity = Graphics::Entity::Create();
        this->graphicsEntity->SetTimeSource(Game::GuiTimeSource::Instance());
        this->graphicsEntity->SetResourceName(this->resourceName);
        this->graphicsEntity->SetRenderFlag(nRenderContext::DoOcclusionQuery, false);
        this->graphicsEntity->OnActivate();

        // find the Nebula node which represents the label
        nTransformNode* rootNode   = this->graphicsEntity->GetResource().GetNode();
        nTransformNode* labelNode = this->FindFirstLabelNodeInHierarchy(rootNode);
        n_assert(labelNode);
        this->SetGfxNode(labelNode);

        // establish link between UIServer's light source and new graphics entity
        Graphics::LightEntity* lightEntity = Server::Instance()->GetLightEntity();
        this->graphicsEntity->AddLink(Graphics::Entity::LightLink, lightEntity);
        lightEntity->AddLink(Graphics::Entity::LightLink, this->graphicsEntity);

        // create child elements
        Label::OnCreate(parent);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
DynamicLabel::OnDestroy()
{
    if (this->graphicsEntity.isvalid())
    {
        this->graphicsEntity->OnDeactivate();
        this->graphicsEntity = 0;
    }
    Label::OnDestroy();
}

//------------------------------------------------------------------------------
/**
    Called before rendering happens. This updates the visibility of our
    Nebula2 visual nodes.
*/
void
DynamicLabel::OnRender()
{
    if (this->IsVisible())
    {
        // first distribute to children
        Label::OnRender();
       
        // then render our graphics entity
        this->graphicsEntity->Render();
    }
}

//------------------------------------------------------------------------------
/**
    Recursively finds the first Nebula label node in the hierarchy.
*/
nTransformNode*
DynamicLabel::FindFirstLabelNodeInHierarchy(nTransformNode* root)
{
    n_assert(root);

    // check if root is a label, if yes, break recursion
    if (root->HasAttr("rlGui"))
    {
        // create a new element
        nString guiType = root->GetStringAttr("rlGuiType");
        if ("Label" == guiType)
        {
            return root;
        }
    }

    // recurse into children
    nRoot* child;
    for (child = root->GetHead(); child; child = child->GetSucc())
    {
        if (child->IsA(this->transformNodeClass))
        {
            nTransformNode* result = this->FindFirstLabelNodeInHierarchy((nTransformNode*)child);
            if (result)
            {
                return result;
            }
        }
    }
    return 0;
}

//------------------------------------------------------------------------------
/**
    Returns the view space transform of the element.
*/
matrix44
DynamicLabel::GetViewSpaceTransform() const
{
    matrix44 m = this->graphicsEntity->GetTransform();
    nTransformNode* parentNode = (nTransformNode*) this->gfxNode->GetParent();
    while (parentNode && parentNode->IsA(this->transformNodeClass))
    {
        m = m * parentNode->GetTransform();
        parentNode = (nTransformNode*) parentNode->GetParent();
    }
    return m;
}

//------------------------------------------------------------------------------
/**
    Set the position of the element as view space transformation.
*/
void
DynamicLabel::SetViewSpaceTransform(const matrix44& m)
{
    // get the inverse exclusive matrix for moving the 
    // view space matrix into parent space
    matrix44 invParent;
    nTransformNode* parentNode = (nTransformNode*) this->gfxNode->GetParent();
    while (parentNode && parentNode->IsA(this->transformNodeClass))
    {
        invParent = invParent * parentNode->GetTransform();
        parentNode = (nTransformNode*) parentNode->GetParent();
    }
    invParent.invert_simple();
    matrix44 localMatrix = m * invParent;
    this->graphicsEntity->SetTransform(localMatrix);
}


}; // namespace UI
